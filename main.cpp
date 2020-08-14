#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/array2.h"
#include "LEDA/graph/graph_alg.h"
#include "LEDA/graph/graph_misc.h"
#include <LEDA/graph/graph_gen.h>
#include "LEDA/system/timer.h"

using namespace leda;

struct indexobj                                 // Object Saved in Index Array
{
    node edge_target;
    node closure_source;

    unsigned int refcount;
};

// Target Vertex to Adjacent of Source & Index.edgeTarget Points to Target Vertex
void makeEdge(const graph& G, node s, node t, array<list<node>>& adjacent, array2<indexobj>& index_arr)
{
    adjacent[s->id()].push(t);                      // Target to Adjacent List of Source
    index_arr(s->id(), t->id()).edge_target = t;   // edgeTarget Points to Target
}

void insertEdge(edge n_edge)                    // Function to Insert Edge
{

}

int main() {
    graph G;                                        // Initial Graph Building Section

    int nn;

    std::cout << "\n\nInput Initial Graph Vertex Number: ";
    std::cin >> nn;

    random_graph(G, nn, 1);

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

    G.new_edge(n1 = G.first_node(), n2 = G.choose_node());

    if (n1 != n2)
    {
        makeEdge(G, n1, n2, adjacent, index_arr);
        std::cout << "Edge " << n1->id() << " - " << n2->id() << " added!\n";

        node tn = index_arr(n1->id(), n2->id()).edge_target;
        std::cout << tn->id() << "\n";
    }
    else
    {
        std::cout << "They Be the Same Chief!\n";
    }

    return 0;
}
