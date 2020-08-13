#include "LEDA/graph/graph.h"
#include "LEDA/graph/ugraph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/p_queue.h"
#include "LEDA/graph/graph_alg.h"
#include "LEDA/core/dynamic_trees.h"
#include "LEDA/graph/graph_misc.h"
#include "LEDA/system/timer.h"

using namespace leda;

struct indexobj                                 // Object Saved in Index Array
{
    node* edge_target;
    node* closure_source;

    unsigned int refcount;
};



int main() {
    graph G;                                        // Initial Graph Building Section

    int nn;

    std::cout << "Input Initial Graph Vertex Number: ";
    std::cin >> nn;

    complete_graph(G, nn);

    edge e;
    forall_edges(e, G)                              // Print Initial Graph Edges
    {
        G.print_edge(e);
        std::cout << "\n";
    }

    std::cout << "\nBuilt Initial Graph G with " << nn << " Vertices!\n\n";

    return 0;
}
