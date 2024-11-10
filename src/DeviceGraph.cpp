#include "DeviceGraph.h"
#include "Device.h"
#include <stdexcept>

using namespace std;

// Function to search for a device by public IP
device* searchDeviceByIP(DeviceGraph& graph, const string& ip) {
    auto it = graph.find(ip);  // Look up the IP in the graph
    if (it != graph.end()) {
        return it->second;  // Return pointer to the device
    } else {
        return nullptr;  // Device not found
    }
}

// Function to search for a device by private IP
device* searchDeviceByPrivateIP(DeviceGraph& graph, const string& ip) {
    for (auto& pair : graph) {
        if (pair.second->privateIp == ip) {
            return pair.second;  // Return device pointer if private IP matches
        }
    }
    throw runtime_error("Device not found with private IP " + ip);  // Exception if not found
}

// Function to search for a device by MAC address
device* searchDeviceByMAC(DeviceGraph& graph, const string& mac) {
    for (auto& pair : graph) {
        if (pair.second->mac == mac) {
            return pair.second;  // Return device pointer if MAC matches
        }
    }
    throw runtime_error("Device not found with MAC address " + mac);  // Exception if not found
}
