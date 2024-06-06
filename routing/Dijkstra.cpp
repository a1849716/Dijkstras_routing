#include <limits.h>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

// A node object used store the name, neighbours + costs etc
class node {
 public:
  node();
  node(std::string x);
  std::string name;
  // A vector of a string int pair storing its neighbour nodes and weight
  std::map<std::string, int> neighbour_nodes;
  // LSDB table storing all links the node has access to
  std::set<std::pair<std::string, std::pair<std::string, int> > > lsdb;  // LSDB
  // Routing table storing the shortest route the nodes can take for different
  // destination nodes;
  std::vector<std::vector<std::string> > routing;

  void update_lsdb(std::string node1, std::string node2, int weight,
                   std::map<std::string, node>& all_nodes);
  void remove_lsdb(std::string node1, std::string node2);
};

// default constructor
node::node(){};

// constructor with name parameter
node::node(std::string x) { name = x; };

// function to remove node from the lsdb
void node::remove_lsdb(std::string node1, std::string node2) {
  // iterate through the lsdb and remove when finds node2
  for (auto it = lsdb.begin(); it != lsdb.end();) {
    if (it->first == node1 && it->second.first == node2) {
      it = lsdb.erase(it);
    } else {
      ++it;
    }
  }
}

// Update LSDB with a link
void node::update_lsdb(std::string node1, std::string node2, int weight,
                       std::map<std::string, node>& all_nodes) {
  // Remove existing entry if it exists
  remove_lsdb(node1, node2);

  // insert new link entry
  lsdb.insert(std::make_pair(node1, std::make_pair(node2, weight)));

  // Syncing for all neighbours
  for (const auto& neighbour : neighbour_nodes) {
    all_nodes[neighbour.first].lsdb.insert(
        std::make_pair(node1, std::make_pair(node2, weight)));
  }
};

// helper function used to add neighbour to specific node
void update_neighbours_lsdb(node& node1, node& node2, int weight,
                            std::map<std::string, node>& all_nodes) {
  // adds neighbours to both nodes
  node1.neighbour_nodes[node2.name] = weight;
  node2.neighbour_nodes[node1.name] = weight;

  // adds link to both node lsdb
  node1.update_lsdb(node1.name, node2.name, weight, all_nodes);
  node2.update_lsdb(node1.name, node2.name, weight, all_nodes);
};

// helper function used to remove neighbour and
void remove_neighbours_lsdb(node& node1, node& node2) {
  // removes neighbour in both nodes
  node1.neighbour_nodes.erase(node2.name);
  node2.neighbour_nodes.erase(node1.name);

  // removes link in both node lsdb
  node1.remove_lsdb(node1.name, node2.name);
  node2.remove_lsdb(node1.name, node2.name);
}

// helper function used to remove an element from a string vector
std::vector<std::string> remove_element(std::vector<std::string> vec,
                                        std::string val) {
  std::vector<std::string>::iterator position =
      find(vec.begin(), vec.end(), val);
  if (position !=
      vec.end()) {  // == myVector.end() means the element was not found
    vec.erase(position);
  }
  return vec;
};

node create_node(std::string input) {
  std::vector<node> all_nodes;
  node temp(input);
  return temp;
};

void print_neighbour_table(node& x) {
  std::vector<std::pair<std::string, int> > neighbours_sorted(
      x.neighbour_nodes.begin(), x.neighbour_nodes.end());
  std::cout << x.name << " Neighbour Table:" << std::endl;
  if (x.neighbour_nodes.empty()) {
    std::cout << std::endl;
  } else {
    for (auto& neighbour_node : neighbours_sorted) {
      std::cout << neighbour_node.first << "|" << neighbour_node.second
                << std::endl;
    }
  }
  std::cout << std::endl;
};

void print_lsdb_table(node& x) {
  std::cout << x.name << " LSDB:" << std::endl;
  if (x.lsdb.empty()) {
    std::cout << std::endl;
  } else {
    for (auto& link : x.lsdb) {
      std::cout << link.first << "|" << link.second.first << "|"
                << link.second.second << std::endl;
    }
    std::cout << std::endl;  // Blank line at the end
  }
};


//dijkstras algorithm used to find the least cost paths of node start
std::map<std::string, int> dijkstra(node &start, std::map<std::string, node> &all_nodes){
  //creating necessary data structures
  std::set<std::string> visited;
  std::set<std::string> unvisited;
  std::map<std::string, int> cost;
  
  //filling the cost and unvisited structures
  for(auto& nodes : all_nodes){
    unvisited.insert(nodes.first);
    cost[nodes.first] = INT_MAX;
  }

  //initiation process
  for(auto& neighbour : start.neighbour_nodes){
    cost[neighbour.first] = neighbour.second;
    visited.insert(neighbour.first);
    //std::cout << "inserted: " << neighbour.first << std::endl;
  }
  cost[start.name] = 0;
  visited.insert(start.name);
  unvisited.erase(start.name);

  // Find the node with the smallest cost in unvisited set while there is still nodes not visited
  while (!unvisited.empty()) {
    std::string closest;
    int min_cost = INT_MAX;
    for (auto& node : unvisited) {
        if (cost[node] < min_cost) {
            min_cost = cost[node];
            closest = node;
        }
    }

    //update un/visited sets
    visited.insert(closest);
    unvisited.erase(closest);
    
    // for each neighbour of the closest node, check if the total cost from the closest is cheaper than what it was previously
    // D(v) = min(D(v), D(w) + c(w,v))  
    for (auto& node : all_nodes[closest].neighbour_nodes){
      if(unvisited.find(node.first) != unvisited.end()){
        int new_cost = cost[closest] + node.second;
        if(new_cost < cost[node.first]){
          cost[node.first] = new_cost;
        } 
      }
    }
  }
  return cost;
};


void print_tables(std::string token, std::map<std::string, node> all_nodes) {
  std::string print_variables = token;
  std::stringstream check3(print_variables);
  std::string curr;
  while (getline(check3, print_variables, ',')) {
    print_neighbour_table(all_nodes[print_variables]);
    print_lsdb_table(all_nodes[print_variables]);
  }
};

// processes input and update with each line
void process_input() {
  // Storing the nodes in a
  std::map<std::string, node> all_nodes;
  std::vector<std::string> input;
  std::string str;
  while (getline(std::cin, str)) {
    if (str.empty()) {
      break;
    }
    input.push_back(str);
  }

  int i = 0;
  //std::cout <<"input: "<< input[i] << std::endl;
  // creating new nodes for every line until LINKSTATE
  while (input[i] != "LINKSTATE") {
    node temp(input[i]);
    all_nodes[input[i]] = temp;
    i++;
    //std::cout << input[i] << std::endl;
  }
  i++;
  // creating links until UPDATE
  while (input[i] != "END") {
    // if it hits UPDATE, continue
    if (input[i] == "UPDATE") {
      i++;
    }

    // initialising variables to tokenise
    std::string line = input[i];
    // Vector of string to save tokens
    std::vector<std::string> tokens;
    std::stringstream check1(line);
    std::string current;

    // tokenises by the space into X-Z, 1, x,y
    while (getline(check1, current, ' ')) {
      tokens.push_back(current);
    }
    // get the first and second node sperated by the -
    std::string node1 = tokens[0].substr(0, tokens[0].find("-"));
    std::string node2 = tokens[0].substr(tokens[0].find("-") + 1);

    // find the weight and add to tables accordingly
    int weight = std::stoi(tokens[1]);
    // is the weight is -1 and the neighbour exists
    if (weight == -1 && all_nodes[node1].neighbour_nodes.find(node2) !=
                            all_nodes[node1].neighbour_nodes.end()) {
      remove_neighbours_lsdb(all_nodes[node1], all_nodes[node2]);
    } else {
      // add in the neighbour
      update_neighbours_lsdb(all_nodes[node1], all_nodes[node2], weight,
                             all_nodes);
    }
    // if there is an optional list, print each variable's tables
    tokens.push_back(" ");
    if (tokens[2] != " ") {
      print_tables(tokens[2], all_nodes);
    }
    i++;
  }
  i++;
}

int main() {
  process_input();
  return 0;
}
