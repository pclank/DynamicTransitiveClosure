#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/array2.h"
#include "LEDA/graph/graph_alg.h"
#include "LEDA/graph/graph_misc.h"
#include <LEDA/graph/graph_gen.h>
#include "LEDA/system/timer.h"

#include <utility>

using namespace leda;

struct indexobj                                 // Object Saved in Index Array
{
    node edge_target;
    node closure_source;

    unsigned int refcount;
};

// Target Vertex to Adjacent of Source & Index.edgeTarget Point to Target Vertex
void makeEdge(const graph& G, node s, node t, array<list<node>>& adjacent, array2<indexobj>& index_arr)
{
    adjacent[s->id()].push(t);                      // Target to Adjacent List of Source
    index_arr(s->id(), t->id()).edge_target = t;    // edgeTarget Points to Target
}

// Find Target Vertex in Adjacent List of Source & Remove It From Adjacent List - Using Index.edgeTarget
void removeEdge(const graph& G, node s, node t, array<list<node>>& adjacent, array2<indexobj>& index_arr)
{
    adjacent[s->id()].remove(t);                    // Delete Target from Adjacent List of Source
    index_arr(s->id(), t->id()).edge_target = NULL; // edgeTarget to Null
}

// Insert Source Vertex into Reaches List of Target & Index.closureSource Point to This Element
void makeClosure(const graph& G, node s, node t, array<list<node>>& reaches, array2<indexobj>& index_arr)
{
    reaches[t->id()].push(s);                       // Source to Reaches List of Target
    index_arr(s->id(), t->id()).closure_source = s; // closureSource Points to Source
}

// Find Source Vertex in Reaches List of Target & Remove It From Reaches List - Using Index.closureSource
void removeClosure(const graph& G, node s, node t, array<list<node>>& reaches, array2<indexobj>& index_arr)
{
    reaches[t->id()].remove(s);                     // Delete Source from Reaches List of Target
    index_arr(s->id(), t->id()).closure_source = NULL;  // closureSource to Null
}

// Function to Insert Edge
void insertEdge(const graph& G, node s, node t, array<list<node>>& reaches, array<list<node>>& adjacent, array2<indexobj>& index_arr)
{
    // Variable Declaration
    list<std::pair<node, node>> worklist;
    node x, y ,z;

    if (index_arr(s->id(), t->id()).refcount == 0)
    {
        makeClosure(G, s, t, reaches, index_arr);
        worklist.push(std::make_pair(s, t));
    }

    makeEdge(G, s, t, adjacent, index_arr);
    index_arr(s->id(), t->id()).refcount++;

    for (int i = 0; i < reaches[s->id()].length(); i++)
    {
        x = reaches[s->id()].inf(reaches[s->id()].get_item(i));
        if (index_arr(x->id(), t->id()).refcount == 0)
        {
            makeClosure(G, x, t, reaches, index_arr);
            worklist.push(std::make_pair(x, t));
        }

        index_arr(x->id(), t->id()).refcount++;
    }

    std::pair<node, node> tmp;
    while (!worklist.empty())
    {
        tmp = worklist.pop();
        x = tmp.first;
        y = tmp.second;

        for (int i = 0; i < adjacent[y->id()].length(); i++)
        {
            z = adjacent[y->id()].inf(adjacent[y->id()].get_item(i));
            if (index_arr(x->id(), z->id()).refcount == 0)
            {
                makeClosure(G, x, z, reaches, index_arr);
                worklist.push(std::make_pair(x, z));
            }

            index_arr(x->id(), z->id()).refcount++;
        }
    }
}

// Function to Delete Edge
void deleteEdge(const graph& G, node s, node t, array<list<node>>& reaches, array<list<node>>& adjacent, array2<indexobj>& index_arr)
{
    // Variable Declaration
    list<std::pair<node, node>> worklist;
    node x, y ,z;

    removeEdge(G, s, t, adjacent, index_arr);
    index_arr(s->id(), t->id()).refcount--;

    if (index_arr(s->id(), t->id()).refcount == 0)
    {
        removeClosure(G, s, t, reaches, index_arr);
        worklist.push(std::make_pair(s, t));
    }

    for (int i = 0; i < reaches[s->id()].length(); i++)
    {
        index_arr(x->id(), t->id()).refcount--;

        x = reaches[s->id()].inf(reaches[s->id()].get_item(i));
        if (index_arr(x->id(), t->id()).refcount == 0)
        {
            removeClosure(G, x, t, reaches, index_arr);
            worklist.push(std::make_pair(x, t));
        }
    }

    std::pair<node, node> tmp;
    while (!worklist.empty())
    {
        tmp = worklist.pop();
        x = tmp.first;
        y = tmp.second;

        for (int i = 0; i < adjacent[y->id()].length(); i++)
        {
            index_arr(x->id(), z->id()).refcount--;

            z = adjacent[y->id()].inf(adjacent[y->id()].get_item(i));
            if (index_arr(x->id(), z->id()).refcount == 0)
            {
                removeClosure(G, x, z, reaches, index_arr);
                worklist.push(std::make_pair(x, z));
            }
        }
    }
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

    int num_of_edges =1;
    std::cout << "How many Edges to Add? (DEFAULT = 1) ";
    std::cin >> num_of_edges;

    for (int i = 0; i < num_of_edges; i++)          // Add Select Number of Edges
    {
        G.new_edge(n1 = G.choose_node(), n2 = G.choose_node());     // Random Source and Target Vertices

        if (n1 != n2)
        {
            makeEdge(G, n1, n2, adjacent, index_arr);
            std::cout << "\n\nEdge " << n1->id() << " - " << n2->id() << " added!\n";

            node tn = index_arr(n1->id(), n2->id()).edge_target;
            std::cout << tn->id() << "\n";
        }
        else
        {
            std::cout << "\nThey Be the Same Chief!\n";
        }
    }

    edge e;
    forall_edges(e, G)                                  // Print all Edges
    {
        G.print_edge(e);
        std::cout << "\n";
    }

    return 0;
}
