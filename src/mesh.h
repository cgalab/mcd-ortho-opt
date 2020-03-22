#ifndef MESH_H
#define MESH_H

#include <memory>
#include <set>
#include <boost/operators.hpp>

class Vertex 
: private boost::equivalent<Vertex>
, boost::totally_ordered<Vertex> {
public:
    Vertex(int id = -1, long long x = -1, long long y = -1);
    
    bool operator<(const Vertex &other) const;
    int id() const;
    
    long long x() const {
        return m_x;
    }
    
    long long y() const {
        return m_y;
    }
    
private:
    int m_id;
    long long m_x;
    long long m_y;
};

class Edge {
public:
    Edge(int startId = -1, int endId = -1);
    
    bool operator<(const Edge &other) const;
    
    const std::pair<int, int> & id() const;
    
private:
    std::pair<int, int> m_id;
};

#endif /* MESH_H */

