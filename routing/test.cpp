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
  std::map<std::string, std::pair<std::string, int>> routing;

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

// Update the routing table using Dijkstra's algorithm
void update_routing_table(node &x, std::map<std::string, node>& all_nodes) {
  // Initialize structures for Dijkstra's algorithm
  std::map<std::string, int> dist;
  std::map<std::string, std::string> prev;
  std::set<std::string> unvisited;

  // Initialize distances and previous nodes
  for (const auto& entry : all_nodes) {
    dist[entry.first] = INT_MAX;
    prev[entry.first] = "";
    unvisited.insert(entry.first);
  }

  // Set distance to self as 0
  dist[x.name] = 0;

  // Dijkstra's algorithm to compute shortest paths
  while (!unvisited.empty()) {
    // Find node with smallest distance in unvisited set
    std::string u;
    int min_dist = INT_MAX;
    for (const auto& node_name : unvisited) {
      if (dist[node_name] < min_dist) {
        min_dist = dist[node_name];
        u = node_name;
      }
    }

    // If all remaining nodes are inaccessible, break
    if (min_dist == INT_MAX) break;

    // Remove current node from unvisited set
    unvisited.erase(u);

    // Update distances to neighbours
    for (const auto& neighbour : all_nodes[u].neighbour_nodes) {
      std::string v = neighbour.first;
      int weight = neighbour.second;
      int alt = dist[u] + weight;
      if (alt < dist[v]) {
        dist[v] = alt;
        prev[v] = u;
      }
    }
  }

  // Build the routing table for the node
  x.routing.clear();
  for (const auto& entry : all_nodes) {
    std::string dest = entry.first;
    if (dest == x.name || dist[dest] == INT_MAX) continue;

    // Determine next hop and distance for each destination
    std::string next_hop = dest;
    while (prev[next_hop] != x.name && prev[next_hop] != "") {
      next_hop = prev[next_hop];
    }
    x.routing[dest] = std::make_pair(next_hop, dist[dest]);
  }
}

// Print the routing table
void print_routing_table(node &x) {
  std::cout << x.name << " Routing Table:" << std::endl;
  if (x.routing.empty()) {
    std::cout << std::endl;
  } else {
    // Sort destinations for consistency
    std::vector<std::string> destinations;
    for (const auto& route : x.routing) {
      destinations.push_back(route.first);
    }
    std::sort(destinations.begin(), destinations.end());
    // Print each destination, next hop, and distance
    for (const auto& dest : destinations) {
      std::cout << dest << "|" << x.routing[dest].first << "|" << x.routing[dest].second << std::endl;
    }
    std::cout << std::endl;
  }
}


// Helper function used to add neighbour to specific node
void update_neighbours_lsdb(node& node1, node& node2, int weight,
                            std::map<std::string, node>& all_nodes) {
  // Adds neighbours to both nodes
  node1.neighbour_nodes[node2.name] = weight;
  node2.neighbour_nodes[node1.name] = weight;

  // Adds link to both node lsdb
  node1.update_lsdb(node1.name, node2.name, weight, all_nodes);
  node2.update_lsdb(node1.name, node2.name, weight, all_nodes);
}

// Helper function used to remove neighbour and
void remove_neighbours_lsdb(node& node1, node& node2) {
  // Removes neighbour in both nodes
  node1.neighbour_nodes.erase(node2.name);
  node2.neighbour_nodes.erase(node1.name);

  // Removes link in both node lsdb
  node1.remove_lsdb(node1.name, node2.name);
  node2.remove_lsdb(node1.name, node2.name);
}

// Helper function used to remove an element from a string vector
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

void print_tables(std::string token, std::map<std::string, node> all_nodes) {
  std::stringstream check3(token);
  std::string curr;
  while (getline(check3, curr, ',')) {
    print_neighbour_table(all_nodes[curr]);
    print_lsdb_table(all_nodes[curr]);
    update_routing_table(all_nodes[curr], all_nodes);
    print_routing_table(all_nodes[curr]);
  }
};

// Processes input and updates with each line
void process_input() {
  // Storing the nodes in a map
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
  // Creating new nodes for every line until LINKSTATE
  while (input[i] != "LINKSTATE") {
    node temp(input[i]);
    all_nodes[input[i]] = temp;
    i++;
  }
  i++;
  // Creating links until END
  while (input[i] != "END") {
    // If it hits UPDATE, continue
    if (input[i] == "UPDATE") {
      i++;
    }

    // Initializing variables to tokenize
    std::string line = input[i];
    // Vector of string to save tokens
    std::vector<std::string> tokens;
    std::stringstream check1(line);
    std::string current;

    // Tokenizes by the space into X-Z, 1, x,y
    while (getline(check1, current, ' ')) {
      tokens.push_back(current);
    }
    // Get the first and second node separated by the -
    std::string node1 = tokens[0].substr(0, tokens[0].find("-"));
    std::string node2 = tokens[0].substr(tokens[0].find("-") + 1);

    // Find the weight and add to tables accordingly
    int weight = std::stoi(tokens[1]);
    // If the weight is -1 and the neighbour exists
    if (weight == -1 && all_nodes[node1].neighbour_nodes.find(node2) !=
                            all_nodes[node1].neighbour_nodes.end()) {
      remove_neighbours_lsdb(all_nodes[node1], all_nodes[node2]);
    } else {
      // Add in the neighbour
      update_neighbours_lsdb(all_nodes[node1], all_nodes[node2], weight,
                             all_nodes);
    }
    // If there is an optional list, print each variable's tables
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
