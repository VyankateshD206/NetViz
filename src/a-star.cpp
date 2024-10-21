#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm> // Include this for std::reverse
#include "Device.h"
#include "Network.h"

using namespace std;

// Node structure for A* algorithm
struct Node {
    device* dev; // Device object
    double gCost; // Cost from start to this node
    double hCost; // Heuristic cost to target
    double fCost() const { return gCost + hCost; } // Total cost
};

// Compare nodes for priority queue
struct NodeCompare {
    bool operator()(Node* a, Node* b) {
        return a->fCost() > b->fCost();
    }
};

// A* algorithm implementation
class AStarAlgorithm {
public:
    Network network; // Using the Network class

    vector<device*> aStar(device* start, device* goal) {
        priority_queue<Node*, vector<Node*>, NodeCompare> openSet;
        unordered_map<string, double> gScores;
        unordered_map<string, device*> cameFrom;

        // Initialize costs
        gScores[start->mac] = 0.0;
        openSet.push(new Node{start, 0.0, heuristic(start, goal)});

        while (!openSet.empty()) {
            Node* current = openSet.top();
            openSet.pop();

            // If the goal is reached
            if (current->dev->mac == goal->mac) {
                vector<device*> path;
                device* step = goal;
                while (step != nullptr) {
                    path.push_back(step);
                    step = cameFrom[step->mac];
                }
                reverse(path.begin(), path.end());
                return path;
            }

            // Explore neighbors
            for (const auto& neighborMac : network.getConnections().at(current->dev->mac)) {
                double tentative_gScore = gScores[current->dev->mac] + 1.0; // Assuming uniform cost
                if (gScores.find(neighborMac) == gScores.end() || tentative_gScore < gScores[neighborMac]) {
                    cameFrom[neighborMac] = current->dev;
                    gScores[neighborMac] = tentative_gScore;
                    device* neighborDev = findDeviceByMac(neighborMac); // Find the device by MAC
                    if (neighborDev) {
                        openSet.push(new Node{neighborDev, tentative_gScore, heuristic(neighborDev, goal)});
                    }
                }
            }
        }
        return {}; // Return empty path if no path is found
    }

    // Heuristic function (fixed for simplicity)
    double heuristic(device* a, device* b) {
        return 1.0; // Replace with actual heuristic logic if needed
    }

    // Function to find a device by MAC address
    device* findDeviceByMac(const string& mac) {
        for (const auto& dev : network.getDevices()) {
            if (dev.mac == mac) {
                return new device(dev); // Return a new device object
            }
        }
        return nullptr; // Device not found
    }
};
