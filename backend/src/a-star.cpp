#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <cmath>

using namespace std;

// Define a device structure with MAC and other device information
struct Device {
    string mac;
    string type;  // Type: router, switch, etc.
    vector<Device*> neighbors;  // List of neighboring devices

    Device(string macAddr, string devType) : mac(macAddr), type(devType) {}
};

// Define a Node structure for A* algorithm (for priority queue)
struct Node {
    Device* device;
    float g_cost; // Cost from start to current node
    float h_cost; // Estimated cost to goal
    float f_cost; // g_cost + h_cost
    Node* parent; // To reconstruct the path

    Node(Device* dev, float g, float h, Node* parentNode)
        : device(dev), g_cost(g), h_cost(h), parent(parentNode) {
            f_cost = g_cost + h_cost;
    }

    bool operator>(const Node& other) const {
        return f_cost > other.f_cost;
    }
};

// A* Search Algorithm
vector<Device*> aStar(Device* start, Device* goal) {
    // Priority queue to store nodes, ordered by f_cost
    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    unordered_map<string, Node*> allNodes;  // To track nodes for quick access
    set<Device*> closedSet;  // To track already visited nodes

    openSet.push(Node(start, 0, 0, nullptr));
    allNodes[start->mac] = &openSet.top();

    while (!openSet.empty()) {
        Node* currentNode = &openSet.top();
        openSet.pop();

        if (currentNode->device == goal) {
            // Reconstruct the path
            vector<Device*> path;
            Node* pathNode = currentNode;
            while (pathNode) {
                path.push_back(pathNode->device);
                pathNode = pathNode->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(currentNode->device);

        for (Device* neighbor : currentNode->device->neighbors) {
            if (closedSet.count(neighbor)) continue;

            float g_cost = currentNode->g_cost + 1;  // Assume a cost of 1 for each edge
            float h_cost = abs(stoi(neighbor->mac.substr(9, 2)) - stoi(goal->mac.substr(9, 2)));  // A simple heuristic (difference in MAC addresses)

            Node* neighborNode = new Node(neighbor, g_cost, h_cost, currentNode);
            openSet.push(*neighborNode);
            allNodes[neighbor->mac] = neighborNode;
        }
    }

    return {};  // Return empty path if no path found
}


