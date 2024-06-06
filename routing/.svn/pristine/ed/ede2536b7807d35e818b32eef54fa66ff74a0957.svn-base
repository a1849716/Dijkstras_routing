#include "node.h"

node::node(){};

node::node(std::string x){
  name = x;
};

void add_neighbours(node current, std::string neighbour_name, int weight){
  current.neighbour_nodes[neighbour_name] = weight;
};

std::vector<std::string> remove_element(std::vector<std::string> vec, std::string val){
  std::vector<std::string>::iterator position = find(vec.begin(), vec.end(), val);
  if (position != vec.end()){ // == myVector.end() means the element was not found
    vec.erase(position);
  }
  return vec;
};

std::vector<std::string> process_input(){
  std::vector<std::string> input;
  std::string str;
  while (getline(cin, str)) { 
        if (str.empty()) { 
            break; 
        } 
        input.push_back(str); 
    } 
    
    for(int i = 0; i < input.size(); i++){
      cout << input[i] << endl;
    }
  return input;
};

string prev_node(unordered_map<string, int> cost_arr){
  
}

int calc_cost(node start, node dest){

};

void neighbour_table(node x){};

void lsdb_table(node x){};

void routing_table(node x){};
