#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include <unordered_map>
#include <fstream>
#include "mesh.h"

class HashPair {
public:
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2> &p) const {
        auto hash1 = std::hash<T1>()(p.first),
                hash2 = std::hash<T2>()(p.second);
        return hash1 ^ hash2;
    }
};

class FileReader {
public:
    FileReader(const std::string &filePath);
    
    const std::unordered_map<int, Vertex> & vertices() const {
        return m_verts;
    }
    
    const std::string & instanceName() const {
        return m_instanceName;
    }
    
private:
    std::unordered_map<int, Vertex> m_verts;
    std::string m_instanceName;
};

class FileWriter {
public:
    FileWriter(const std::string &filePath, const std::string &instanceName,
               const std::unordered_map<int, Vertex> &verts,
               const std::set<Edge> &edges);
    
    void operator()() const;
    
private:
    std::string m_filePath;
    std::string m_instanceName;
    std::unordered_map<int, Vertex> m_verts;
    std::set<Edge> m_edges;
};

class IpeObj {
public:
    IpeObj(const Vertex &vertex, const std::string &layer,
           double scale = 1., const std::string &color = "black",
           const std::string &size = "normal", const std::string &label = "");
    IpeObj(const Vertex &start, const Vertex &end, const std::string &layer,
           double scale = 1., const std::string &color = "black",
           const std::string &size = "normal");

    const std::string & str() const {
        return m_str;
    }

    const std::string & layer() const {
        return m_layer;
    }

private:
    std::string m_str;
    double m_thickness;
    std::string m_layer;
};

class IpeWriter {
public:
    IpeWriter(const std::string &filePath);

    void addIpeObj(const IpeObj &ipeObj) {
        m_ipeObjs.push_back(ipeObj);
        m_layers.insert(ipeObj.layer());
    }

    void write();

private:
    void copyFile(const std::string &filePath);

    std::vector<IpeObj> m_ipeObjs;
    std::set<std::string> m_layers;
    std::ofstream m_file;
};

#endif /* FILE_IO_H */
