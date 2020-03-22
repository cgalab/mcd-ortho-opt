#ifndef CONVEX_PART_H
#define CONVEX_PART_H

#include <set>
#ifdef ENABLE_VIEW
#include <QWidget>
#include <QPainter>
#include <CGAL/Qt/GraphicsItem.h>
#include <unordered_map>
#endif
#include "file_io.h"

class TrapDecomp {
public:
    TrapDecomp(const std::string &inFilePath, const std::string &outFilePath = "",
               const std::string &ipeFilePath = "");
    
    const std::unordered_map<int, Vertex> & verts() const {
        return m_verts;
    }

    const std::set<Edge> & edges() const {
        return m_edges;
    }
    
private:
    void compDelaunay();
    
    std::unordered_map<int, Vertex> m_verts;
    std::set<Edge> m_edges;
    std::set<Vertex> m_orderedVerts;
    std::vector<Vertex> m_lowerChain;
    std::vector<Vertex> m_upperChain;
    int m_nFaces = 0;
};

#ifdef ENABLE_VIEW

class TrapDecompGraphicsItem : public CGAL::Qt::GraphicsItem {
    Q_OBJECT

public:
    TrapDecompGraphicsItem(const TrapDecomp &cd);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

public slots:
    void modelChanged() override;
    void onPrevEv();
    void onNextEv();
    
private:
    TrapDecomp m_td;
    int m_index = 0;
    double m_scale = 0.;
    QRectF m_boundingRect;
};

#endif

#endif /* TRAP_PART_H */
