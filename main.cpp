#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/array2.h"
#include "LEDA/graph/graph_alg.h"
#include "LEDA/graph/graph_misc.h"
#include "LEDA/system/timer.h"

using namespace leda;

struct indexobj                                 // Object Saved in Index Array
{
    node* edge_target;
    node* closure_source;

    unsigned int refcount;
};

void insertEdge(edge n_edge)                    // Function to Insert Edge
{

}

int main() {
    graph G;                                        // Initial Graph Building Section

    int nn;

    std::cout << "Input Initial Graph Vertex Number: ";
    std::cin >> nn;

    // complete_graph(G, nn);
    random_graph(G, nn, 4);

    edge e;
    forall_edges(e, G)                              // Print Initial Graph Edges
    {
        G.print_edge(e);
        std::cout << "\n";
    }

    std::cout << "\nBuilt Initial Graph G with " << nn << " Vertices!\n\n";

    array<list<node>> adjacent(nn);                 // Array of Adjacent Lists
    array<list<node>> reaches(nn);                  // Array of Reaches Lists

    array2<indexobj> index_arr(nn, nn);             // Index Array

    node n1;
    node n2;
    forall_nodes(n1, G)                              // Initialize Index Info for Each Row
    {
        forall_nodes(n2, G)                             // Initialize Index Info for Each Column
        {
            index_arr(n1->id(), n2->id()).edge_target = NULL;
            index_arr(n1->id(), n2->id()).closure_source = NULL;
            index_arr(n1->id(), n2->id()).refcount = 0;
        }
    }

    return 0;
}
