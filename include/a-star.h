#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

// Forward declaration of PC class
class PC;

// Base class for network devices
struct Device {
    std::string name;
};

// PC class definition
class PC : public Device {
public:
    PC(std::string name);
};

// Edge structure for the graph
struct Edge {
    PC* target; // Target PC
    double cost; // Cost to traverse this edge
};

// Node structure for A* algorithm
struct Node {
    PC* device; // PC device
    double gCost; // Cost from start to this node
    double hCost; // Heuristic cost to target
    double fCost() const; // Total cost
};

// Comparison for priority queue
struct NodeCompare {
    bool operator()(Node* a, Node* b);
};

// Network class with A* implementation
class Network {
public:
    std::unordered_map<std::string, std::vector<Edge>> graph; // Adjacency list

    void addEdge(PC* from, PC* to, double cost);
    double heuristic(PC* a, PC* b);
    std::vector<PC*> aStar(PC* start, PC* goal);
};

#endif // NETWORK_H

