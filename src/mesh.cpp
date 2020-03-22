#include <iostream>
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "mesh.h"

Vertex::Vertex(int id, long long x, long long y)
: m_id(id)
, m_x(x)
, m_y(y) {
}

bool Vertex::operator<(const Vertex &other) const {
#ifdef SORT_BY_X
    if (m_x != other.x()) {
        return m_x < other.x();
    }
    
    if (m_y != other.y()) {
        return m_y < other.y();
    }
#else
    if (m_y != other.y()) {
        return m_y < other.y();
    }
    
    if (m_x != other.x()) {
        return m_x < other.x();
    }
#endif
    
    return false;
}

int Vertex::id() const {
    return m_id;
}

Edge::Edge(int startId, int endId) {
    m_id = std::minmax({startId, endId});
}

bool Edge::operator<(const Edge &other) const {
    return m_id < other.id();
}

const std::pair<int, int> & Edge::id() const {
    return m_id;
}
