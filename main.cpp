#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/array2.h"
#include "LEDA/graph/graph_alg.h"
#include "LEDA/graph/graph_misc.h"
#include <LEDA/graph/graph_gen.h>
#include "LEDA/system/timer.h"

#include <utility>
#include <stdlib.h>

using namespace leda;

struct indexobj                                 // Object Saved in Index Array
{
    node edge_target;
    node closure_source;

    unsigned int refcount;
};

// Helper Functions Section

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

// Usable Functions Section

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

    if (index_arr(s->id(), t->id()).refcount == 0 || (s->id() == t->id()))      // Personal Fix-Improvement for Graphs Containing Self-Loops
    {
        removeClosure(G, s, t, reaches, index_arr);
        worklist.push(std::make_pair(s, t));
    }

    for (int i = 0; i < reaches[s->id()].length(); i++)
    {
        x = reaches[s->id()].inf(reaches[s->id()].get_item(i));

        if (index_arr(x->id(), t->id()).refcount != 0)
        {
            index_arr(x->id(), t->id()).refcount--;
        }

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
            z = adjacent[y->id()].inf(adjacent[y->id()].get_item(i));
            index_arr(x->id(), z->id()).refcount--;

            if (index_arr(x->id(), z->id()).refcount == 0)
            {
                removeClosure(G, x, z, reaches, index_arr);
                worklist.push(std::make_pair(x, z));
            }
        }
    }
}

int main()
{
    system("clear");                      // Clear Console

    // Timer Initialization

    timer dtc_timer;
    timer dfs_timer;

    // Initial Graph Building Section

    graph G;

    int nn;

    std::cout << "\n\nInput Initial Graph Vertex Number: ";
    std::cin >> nn;

    random_graph(G, nn, 0);

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

    int num_of_edges = nn * (nn - 1);               // Possible Number of Edges
    std::cout << "Input Edges to Add (Max = " << num_of_edges << "): ";
    std::cin >> num_of_edges;

    // Add Select Number of Edges

    std::cout << "\nInserting " << num_of_edges <<  " Edges...\n";

    dtc_timer.start();                                        // Start Timer

    for (int i = 0; i < num_of_edges; i++)              // TODO Exclude Already Added Edges
    {
        G.new_edge(n1 = G.choose_node(), n2 = G.choose_node());     // Random Source and Target Vertices

        insertEdge(G, n1, n2, reaches, adjacent, index_arr);
    }

    dtc_timer.stop();                                         // Stop Timer

    char choice;
    std::cout << "Done!\n\n" << "\nPrint All Edges? (y/n) ";
    std::cin >> choice;

    edge e;
    if (choice == 'y')
    {
        // Print all Edges

        std::cout << "\nPrinting All Edges: \n";


        forall_edges(e, G)
        {
            G.print_edge(e);
            std::cout << "\n";
        }
    }

    char ref_choice;
    std::cout << "\nPrint RefCount Matrix? (y/n) ";
    std::cin >> ref_choice;

    // Printing RefCount Matrix for Testing after Insertion

    if (ref_choice == 'y')
    {
        std::cout << "\nPrinting RefCount Matrix:\n";
        for (int i = 0; i < nn; i++)                    // RefCount for All Rows
        {
            for (int j = 0; j < nn; j++)                    // RefCount for All Columns
            {
                std::cout << index_arr(i, j).refcount;
            }

            std::cout << "\n";
        }
    }

    std::cout << "\nTime Elapsed (" << nn << " Vertices and " << num_of_edges << " Edges): " << dtc_timer.elapsed_time() << " Seconds.\n\n";    // Print Elapsed Time for Creation

    std::cout << "\nPrint Reachability (Transitive Closure) of Vertices? (y/n) ";
    std::cin >> choice;

    if (choice == 'y')
    {
        forall_nodes(n1, G)
        {
            std::cout << "\nVertex " << n1->id() << " is Reachable by: ";

            for (int i = 0; i < reaches[n1->id()].length(); i++)    // Print All Vertices in Reaches List of n1
            {
                std::cout << reaches[n1->id()].inf(reaches[n1->id()].get_item(i))->id() << "  ";
            }
        }
    }

    std::cout << "\n";

    // Static Algorithm Test Comparison Using DFS Prior to Deletion

    std::cout << "\nPrint Reachability (Transitive Closure) of Vertices after DFS? (y/n) ";
    std::cin >> choice;

    std::cout << "\nTesting using DFS...\n";

    forall_nodes(n1, G)
    {
        node_array<bool> st(G,false);

        dfs_timer.start();                                  // Start Timer

        DFS(G, n1, st);                                     // Run DFS

        dfs_timer.stop();                                   // Pause Timer

        if (choice == 'y')                                  // Printing Vertices n1 can Reach
        {
            std::cout << "\nVertex " << n1->id() << " Reaches: ";
            forall_nodes(n2, G)
            {
                if (st[n2]) {
                    std::cout << n2->id() << "  ";
                }
            }
        }
    }

    std::cout << "\n\nDone!\n\nTime Elapsed for DFS Prior to Deletion: " << dfs_timer.elapsed_time() << " Seconds.\n\n";    // Print Elapsed Time for DFS

    // Deletion Decision Section

    std::cout << "\n\nDelete Random Edges? (y/n) ";
    std::cin >> choice;

    char del_print_choice;
    std::cout << "\nPrint Edges Being Deleted? (y/n) ";
    std::cin >> del_print_choice;

    if (choice == 'y')
    {
        std::cout << "\nDeleting Edges...\n";

        dtc_timer.restart();                            // Reset Timer to Zero and Start Counting

        // Remove Random Edges

        for (int i = 0; i < num_of_edges / 2; i++)      // Half the Edges Are Removed
        {
            e = G.choose_edge();                            // Randomly Choose an Edge

            if (del_print_choice == 'y')                    // Conditionally Print Edge Being Deleted
            {
                std::cout << "\nRemoving Edge: ";
                G.print_edge(e);
                std::cout << "\n";
            }

            deleteEdge(G, G.source(e), G.target(e), reaches, adjacent, index_arr);  // Update DTC
            G.del_edge(e);                                  // Delete Edge from G
        }

        dtc_timer.stop();                               // Stop Timer

        // Printing RefCount Matrix for Testing after Deletion

        if (ref_choice == 'y')
        {
            std::cout << "\nPrinting RefCount Matrix:\n";
            for (int i = 0; i < nn; i++)                    // RefCount for All Rows
            {
                for (int j = 0; j < nn; j++)                    // RefCount for All Columns
                {
                    std::cout << index_arr(i, j).refcount;
                }

                std::cout << "\n";
            }
        }

        std::cout << "\nDone!\n\nTime Elapsed for Deletion (" << num_of_edges / 2 << " Edges): " << dtc_timer.elapsed_time() << " Seconds.\n\n"; // Print Elapsed Time for Deletion
    }

    // Reachability Testing After Possible Deletion Section

    std::cout << "\nPrint Reachability (Transitive Closure) of Vertices? (y/n) ";
    std::cin >> choice;

    if (choice == 'y')
    {
        forall_nodes(n1, G)
        {
            std::cout << "\nVertex " << n1->id() << " is Reachable by: ";

            for (int i = 0; i < reaches[n1->id()].length(); i++)    // Print All Vertices in Reaches List of n1
            {
                std::cout << reaches[n1->id()].inf(reaches[n1->id()].get_item(i))->id() << "  ";
            }
        }
    }

    std::cout << "\n\n";

    // Static Algorithm Test Comparison Using DFS Post Deletion

    std::cout << "\nPrint Reachability (Transitive Closure) of Vertices after DFS? (y/n) ";
    std::cin >> choice;

    std::cout << "\nTesting using DFS...\n";

    forall_nodes(n1, G)
    {
        node_array<bool> st(G,false);

        dfs_timer.start();                                  // Start Timer

        DFS(G, n1, st);                                     // Run DFS

        dfs_timer.stop();                                   // Pause Timer

        if (choice == 'y')                                  // Printing Vertices n1 can Reach
        {
            std::cout << "\nVertex " << n1->id() << " Reaches: ";
            forall_nodes(n2, G)
            {
                if (st[n2]) {
                    std::cout << n2->id() << "  ";
                }
            }
        }
    }

    std::cout << "\n\nDone!\n\nTime Elapsed for DFS Post Deletion: " << dfs_timer.elapsed_time() << " Seconds.\n\n";    // Print Elapsed Time for DFS

    return 0;
}
