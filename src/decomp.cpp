#include <math.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <random>
#include <map>
extern "C" {
#include <triangle.h>
}
#include "decomp.h"

TrapDecomp::TrapDecomp(const std::string &inFilePath, const std::string &outFilePath, 
                       const std::string &ipeFilePath) {
    const auto fr = FileReader(inFilePath);
    m_verts = fr.vertices();

    const auto position
            = [&](const Vertex &v1, const Vertex &v2, const Vertex & v3) {
                return (v2.x() - v1.x()) * (v3.y() - v1.y()) - (v2.y() - v1.y()) * (v3.x() - v1.x());
            };
    
    for (const auto &entry : m_verts) {
        m_orderedVerts.insert(entry.second);
    }
    
    Vertex v0;
    for (const auto &v : m_orderedVerts) {
#ifdef SORT_BY_X
        if (v.x() == v0.x()) {
#else
        if (v.y() == v0.y()) {
#endif
            const auto edge = Edge(v0.id(), v.id());
            m_edges.insert(edge);
        } else {
            if (v0.id() != -1) {
                m_upperChain.push_back(v0);
            }

            if (v.id() != -1) {
                m_lowerChain.push_back(v);
            }
        }
        
        v0 = v;
    }
    
    m_upperChain.push_back(*m_orderedVerts.rbegin());
    
    v0 = Vertex();
    for (const auto &v : m_upperChain) {
        if (v0.id() != -1) {
            m_edges.insert(Edge(v0.id(), v.id()));
        }
        
        v0 = v;
    }
    
    v0 = Vertex();
    for (const auto &v : m_lowerChain) {
        if (v0.id() != -1) {
            m_edges.insert(Edge(v0.id(), v.id()));
        }

        v0 = v;
    }

    const auto fRefineChain
            = [&](bool &bRefined, const std::vector<Vertex> &chain, bool bIsUpper) {
                std::vector<Vertex> refinedChain({chain.at(0)});
                bRefined = false;
                for (size_t i = 1; i < chain.size() - 1; i++) {
                    auto v1 = *refinedChain.rbegin(),
                            v2 = chain.at(i),
                            v3 = chain.at(i + 1);
#ifdef SORT_BY_X
                    bool bSkip = bIsUpper ? position(v1, v2, v3) > 0 : position(v1, v2, v3) < 0;
#else
                    bool bSkip = bIsUpper ? position(v1, v2, v3) < 0 : position(v1, v2, v3) > 0;
#endif
                    if (bSkip) {
                        m_edges.insert(Edge(v1.id(), v3.id()));
                        bRefined = true;
                    } else {
                        refinedChain.push_back(v2);
                    }
                }

                refinedChain.push_back(*chain.rbegin());

                return refinedChain;
            };

    auto chain = m_upperChain;
    bool bRefined = true;
    while (bRefined) {
        chain = fRefineChain(bRefined, chain, true);
    }
    
    chain = m_lowerChain;
    bRefined = true;
    while (bRefined) {
        chain = fRefineChain(bRefined, chain, false);
    }
    
    size_t nFaces = (m_edges.size() - m_verts.size() + 1);
    std::cout << "num_cvx_areas:   " << nFaces << "\n";

    if (!outFilePath.empty()) {
        const auto name = fr.instanceName();
        FileWriter(outFilePath, name, m_verts, m_edges)();
    }
    
    if (!ipeFilePath.empty()) {
        IpeWriter iw(ipeFilePath);
        for (const auto &entry : m_verts) {
            const auto vertex = entry.second;
            iw.addIpeObj(IpeObj(vertex, "Verts", 7, "black", "normal"));
        }
        
        for (const auto &edge : m_edges) {
            const auto v1 = m_verts.at(edge.id().first),
                    v2 = m_verts.at(edge.id().second);
            iw.addIpeObj(IpeObj(v1, v2, "Edges", 7, "black", "normal"));
        }
        
        iw.write();
    }
}

#ifdef ENABLE_VIEW

TrapDecompGraphicsItem::TrapDecompGraphicsItem(const TrapDecomp &cd)
: m_td(cd) {
    double width = 0., height = 0.;
    for (const auto &entry : m_td.verts()) {
        const auto v = entry.second;
        
        if (v.x() > width) {
            width = v.x();
        }
        
        if (v.y() > height) {
            height = v.y();
        }
    }
    
    m_scale = 1000. / std::max(width, height);
    m_boundingRect = QRectF(0., 0., 1000. * width, 1000. * height);
}

QRectF TrapDecompGraphicsItem::boundingRect() const {
    return m_boundingRect;
}

void TrapDecompGraphicsItem::paint(QPainter *painter,
                                     const QStyleOptionGraphicsItem*,
                                     QWidget*) {
    painter->setPen(QPen(Qt::black, 1.));
    painter->setBrush(Qt::black);

    for (const auto &edge : m_td.edges()) {
        int id1 = edge.id().first, id2 = edge.id().second;

        if (m_td.verts().find(id1) != m_td.verts().end()
            && m_td.verts().find(id2) != m_td.verts().end()) {
            const auto from = m_td.verts().at(id1), to = m_td.verts().at(id2);
            painter->drawLine(QPointF(from.x() * m_scale, from.y() * m_scale),
                              QPointF(to.x() * m_scale, to.y() * m_scale));
        }
    }

    for (const auto &entry : m_td.verts()) {
        const auto v = entry.second;
        double x = v.x(), y = v.y();
        painter->drawEllipse(QPointF(x, y) * m_scale, 1., 1.);
    }
}

void TrapDecompGraphicsItem::modelChanged() {
    update();
}

void TrapDecompGraphicsItem::onPrevEv() {
    m_index--;
    update();
}

void TrapDecompGraphicsItem::onNextEv() {
    m_index++;
    update();
}

#include "moc_decomp.cpp"

#endif
