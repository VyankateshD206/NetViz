#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <random>
#include "Device.h"
#include "Network.h"
#include "IPAddr.h"

using namespace std;

// Function to generate a random device type
Device_Types randomDeviceType() {
    return static_cast<Device_Types>(rand() % 4);
}

// Generates a random MAC address in the format XX:XX:XX:XX:XX:XX
string generateRandomMAC() {
    string mac = "";
    for (int i = 0; i < 6; i++) {
        if (i != 0) mac += ":";
        mac += to_string(rand() % 256);
    }
    return mac;
}

// Function to generate a random graph of devices and connections
void generateRandomGraph(Network& network, IP& ipGenerator, int numDevices) {
    for (int i = 0; i < numDevices; ++i) {
        Device_Types type = randomDeviceType();
        string mac = generateRandomMAC();
        string publicIP = ipGenerator.generateUniqueIP("public");
        network.addDevice(type, publicIP, mac); // Assign public IP here
    }

    // Randomly connect devices
    for (const auto& dev1 : network.devices) {
        for (const auto& dev2 : network.devices) {
            if (dev1.mac != dev2.mac && rand() % 2) {
                network.connectDevices(dev1.mac, dev2.mac);
            }
        }
    }
}

// Create network from user input
Network createNetworkFromUserInput(IP& ipGenerator) {
    Network network;
    int numDevices;
    cout << "Enter the number of devices to add: ";
    cin >> numDevices;

    for (int i = 0; i < numDevices; ++i) {
        int typeInput;
        cout << "Enter device type (0 for PC, 1 for BRIDGE, 2 for SWITCH, 3 for ROUTER): ";
        cin >> typeInput;
        Device_Types type = static_cast<Device_Types>(typeInput);

        string mac = generateRandomMAC();
        string publicIP = ipGenerator.generateUniqueIP("public");
        network.addDevice(type, publicIP, mac); // Assign public IP here
    }

    return network;
}

// Function to create a LAN without bridges
Network removeBridges(const Network& originalNetwork) {
    Network newNetwork;

    for (const auto& dev : originalNetwork.devices) {
        if (dev.type != BRIDGE) {
            newNetwork.addDevice(dev.type, dev.publicIP, dev.mac);
        }
    }

    for (const auto& conn : originalNetwork.connections) {
        if (newNetwork.connections.find(conn.first) != newNetwork.connections.end()) {
            for (const auto& neighbor : conn.second) {
                if (newNetwork.connections.find(neighbor) != newNetwork.connections.end()) {
                    newNetwork.connectDevices(conn.first, neighbor);
                }
            }
        }
    }

    return newNetwork;
}

// Helper function to assign public IPs
void dfsAssignPublicIP(Network& network, const string& mac, const string& publicIP, map<string, bool>& visited) {
    visited[mac] = true;

    // Assign the public IP to the device if it's a PC
    auto& device = network.findDeviceByMAC(mac);
    if (device.type == PC) {
        device.publicIP = publicIP;
    }

    // Continue DFS to all neighbors
    for (const auto& neighbor : network.connections[mac]) {
        if (!visited[neighbor]) {
            dfsAssignPublicIP(network, neighbor, publicIP, visited);
        }
    }
}

void assignPublicIPs(Network& network, IP& ipGenerator) {
    map<string, bool> visited;

    for (auto& device : network.devices) {
        if (device.type == ROUTER && !visited[device.mac]) {
            // Generate a unique public IP for the router
            device.publicIP = ipGenerator.generateUniqueIP("public");
            // Assign this public IP to all reachable PCs
            dfsAssignPublicIP(network, device.mac, device.publicIP, visited);
        }
    }
}

// Helper function to run DFS on LANs and return private IPs of each LAN
vector<vector<string>> getLANPrivateIPs(Network& lanNetwork, IP& ipGenerator) {
    vector<vector<string>> lanPrivateIPs;
    map<string, bool> visited;

    for (const auto& device : lanNetwork.devices) {
        if (!visited[device.mac]) {
            vector<string> currentLAN;
            dfsCollectLANIPs(lanNetwork, device.mac, visited, currentLAN, ipGenerator);
            lanPrivateIPs.push_back(currentLAN);
        }
    }
    return lanPrivateIPs;
}

// DFS helper to collect IPs in each LAN
void dfsCollectLANIPs(Network& network, const string& mac, map<string, bool>& visited, 
                      vector<string>& lanIPs, IP& ipGenerator) {
    visited[mac] = true;

    // Assign private IP and store it
    auto& device = network.findDeviceByMAC(mac);
    device.privateIP = ipGenerator.generateUniqueIP("local", "10.0.0.0", "255.255.255.0");
    lanIPs.push_back(device.privateIP);

    for (const auto& neighbor : network.connections[mac]) {
        if (!visited[neighbor]) {
            dfsCollectLANIPs(network, neighbor, visited, lanIPs, ipGenerator);
        }
    }
}

