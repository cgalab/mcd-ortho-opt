#include <fstream>
#include <iostream>
#include <regex>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#include "file_io.h"

const char *IPE_HEADER_PATH = "ipe/header.ipe";
const char *IPE_FOOTER_PATH = "ipe/footer.ipe";

FileReader::FileReader(const std::string &filePath) {
    std::cout << "Opening file " << filePath << " ...\n";
    std::ifstream ifstr(filePath, std::ifstream::binary);

    if (!ifstr) {
        std::cout << "Error:  Could not open input file.\n";
    } else {
        const auto suffix = filePath.substr(filePath.find_last_of(".") + 1);
        if (suffix == "json") {
            Json::Value root;
            ifstr >> root;
            const Json::Value &pnts = root["points"];
            m_instanceName = root["name"].asString();

            for (const auto &pnt : pnts) {
                int id = pnt["i"].asInt();
                long long x = pnt["x"].asInt64(), y = pnt["y"].asInt64();
                m_verts[id] = Vertex(id, x, y);
            }
        }
    }
}

FileWriter::FileWriter(const std::string &filePath, const std::string &instanceName,
               const std::unordered_map<int, Vertex> &verts, const std::set<Edge> &edges)
: m_filePath(filePath)
, m_instanceName(instanceName)
, m_verts(verts)
, m_edges(edges) {
}

void FileWriter::operator()() const {
    std::cout << "Writing results to " << m_filePath << " ...\n";
    std::ofstream ofstr(m_filePath);

    if (!ofstr) {
        std::cout << "Error:  Could not open input file.\n";
    } else {
        for (size_t i = 0; i < m_verts.size(); i++) {
            const auto v = m_verts.at(i);
            ofstr << "v " << v.x() << " " << v.y() << "\n";
        }

        for (const auto &edge : m_edges) {
            ofstr << "l " << (edge.id().first + 1) << " "
                    << (edge.id().second + 1) << "\n";
        }

        ofstr << "\n";
    }
}

IpeObj::IpeObj(const Vertex &vertex, const std::string &layer,
       double scale, const std::string &color,
       const std::string &size, const std::string &label)
: m_layer{layer}
{
    std::stringstream strs;
    strs << "<use layer=\"" << m_layer << "\" name=\"mark/disk(sx)\" pos=\""
            << vertex.x() * scale << " " 
            << vertex.y() * scale
            << "\" size=\"" << size << "\" stroke=\"" << color << "\"/>\n";

    if (!label.empty()) {
        strs << "<text layer=\"Labels\" transformations=\"translations\" pos=\""
                << vertex.x() * scale << " " 
                << vertex.y() * scale
                << "\" stroke=\"" << color 
                << "\" type=\"Label\" valign=\"baseline\">"
                << label << "</text>\n";
    }
    
    m_str = strs.str();
}

IpeObj::IpeObj(const Vertex &start, const Vertex &end, const std::string &layer, double scale, 
               const std::string &color, const std::string &size)
: m_layer{layer}
{
    std::stringstream strs;
    strs << "<path layer=\"" << m_layer << "\" stroke=\"" << color 
            << "\" pen=\"" << size << "\">\n"
            << start.x() * scale << " " 
            << start.y() * scale << " m\n"
            << end.x() * scale << " " 
            << end.y() * scale << " l\n"
            << "</path>\n";
    
    m_str = strs.str();
}

IpeWriter::IpeWriter(const std::string &filePath) {
    m_file.open(filePath, std::ofstream::out | std::ofstream::app);
    m_layers.insert("Labels");
}

void IpeWriter::write() {
    copyFile(IPE_HEADER_PATH);
    for (const auto &layer : m_layers)
        m_file << "<layer name=\"" << layer << "\"/>\n";

    const auto fToStr = [this]() {
        std::string str;
        for (const auto &layer : m_layers) {
            str.append(layer);
            str.append(" ");
        }

        return str;
    };
    m_file << "<view layers=\"" << fToStr()
            << "\" active=\"" << fToStr() << "\"/>";
    for (const auto &ipeObj : m_ipeObjs)
        m_file << ipeObj.str();
    copyFile(IPE_FOOTER_PATH);
}

void IpeWriter::copyFile(const std::string &filePath) {
    std::ifstream src;
    src.open(filePath, std::ofstream::in);
    m_file << src.rdbuf();
}
