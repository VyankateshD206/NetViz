#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <unordered_map>
#include "Device.h"
#include "Network.h"

// Node structure for A* algorithm
struct Node {
    device* dev; // Device object
    double gCost; // Cost from start to this node
    double hCost; // Heuristic cost to target
    double fCost() const { return gCost + hCost; } // Total cost
};

// Compare nodes for priority queue
struct NodeCompare {
    bool operator()(Node* a, Node* b);
};

// A* algorithm implementation
class AStarAlgorithm {
public:
    Network network; // Using the Network class

    // Method to find the shortest path using A* algorithm
    std::vector<device*> aStar(device* start, device* goal);

    // Heuristic function to estimate cost
    double heuristic(device* a, device* b);

    // Function to find a device by MAC address
    device* findDeviceByMac(const std::string& mac);
};

#endif // ASTAR_H
