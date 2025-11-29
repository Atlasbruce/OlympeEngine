// Minimal entry for Olympe Blueprint Editor
#include <iostream>
#include <fstream>
#include "../include/Graph.h"
#include "../include/imnodes_stub.h"

/*int main(int argc, char** argv)
{
    std::cout << "Olympe Blueprint Editor (stub)" << std::endl;
    imnodes_stub::Initialize();

    Graph g;
    g.nodes.push_back({1, "Start", 100.0f, 100.0f});
    g.nodes.push_back({2, "Print", 300.0f, 100.0f});

    // save example
    {
        auto j = g.ToJson();
        std::ofstream ofs("example_graph.json");
        if (ofs) ofs << j.dump(4);
    }

    imnodes_stub::Shutdown();
    return 0;
}/**/
