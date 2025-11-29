#include "../include/Graph.h"
#include "../../Source/third_party/nlohmann/json.hpp"

using json = nlohmann::json;

nlohmann::json Graph::ToJson() const
{
    json j;
    j["nodes"] = json::array();
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const Node& n = nodes[i];
        json nj;
        nj["id"] = n.id;
        nj["type"] = n.type;
        nj["x"] = n.x;
        nj["y"] = n.y;
        j["nodes"].push_back(nj);
    }
    return j;
}

Graph Graph::FromJson(const nlohmann::json& j)
{
    Graph g;
    try {
        const json nodes = j["nodes"];
        if (nodes.is_array()) {
            for (size_t idx = 0; idx < nodes.size(); ++idx) {
                const json& nj = nodes[idx];
                Node n;
                try { n.id = nj["id"].get<int>(); } catch(...) { n.id = 0; }
                try { n.type = nj["type"].get<std::string>(); } catch(...) { n.type = std::string(); }
                try { n.x = static_cast<float>(nj["x"].get<double>()); } catch(...) { n.x = 0.0f; }
                try { n.y = static_cast<float>(nj["y"].get<double>()); } catch(...) { n.y = 0.0f; }
                g.nodes.push_back(n);
            }
        }
    } catch(...) {
        // parsing failed or structure not present - return empty graph
    }
    return g;
}
