#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits.h>
#include <algorithm>
#include <string>
using namespace std;

class node{
  public:
    node();
    node(std::string x);
    std::string name;
    // A vector of a string int pair storing its neighbour nodes and weight
    std::unordered_map<std::string, int> neighbour_nodes;
    // LSDB table storing all links the node has access to
    std::unordered_map<std::string, int> lsdb;
    // Routing table storing the shortest route the nodes can take for different destination nodes;
    std::vector<std::vector<std::string> > routing;

};

//function to add all neighbours to node
void add_neighbours(node current, std::string neighbour_name, int weight);

//removing a specific element from a string vector
std::vector<std::string> remove_element(std::vector<std::string> vec, std::string val);

//splits and places input into a vector<string>
std::vector<std::string> process_input();

int calc_cost(node start, node dest);

string prev_node(unordered_map<string, int> cost_arr);
/*
prints out neighbour table for node x 
expected output: 
  X Neighbour Table:
  Y|5
  Z|1
*/
void neighbour_table(node x);

/*
prints out LSDB table for node x 
expected output:
  X LSDB:
    X|Y|2
    X|Z|7
    Y|Z|1
*/
void lsdb_table(node x);

/*
prints out routing table for node x 
expected output:
  X Routing Table:
    Y|Y|2
    Z|Y|3

*/
void routing_table(node x);

#endif

