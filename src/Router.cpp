#include "Router.h"
#include <algorithm>

Router::Router(std::string ip, std::string mac, int ports) : device(ROUTER, ip, mac), numPorts(ports) {
    portStatus.resize(numPorts, true);  // Initialize all ports as enabled
}

Router::~Router() {
    // Clean up any dynamically allocated memory if necessary
}

void Router::addRoute(const std::string& destIP, const std::string& nextHopIP) {
    routingTable[destIP] = nextHopIP;
}

void Router::removeRoute(const std::string& destIP) {
    routingTable.erase(destIP);
}

std::string Router::getNextHop(const std::string& destIP) const {
    auto it = routingTable.find(destIP);
    if (it != routingTable.end()) {
        return it->second;
    }
    return "";  // Return empty string if no route found
}

//we will use if needed.
// void Router::enablePort(int port) {
//     if (port >= 0 && port < numPorts) {
//         portStatus[port] = true;
//     }
// }

// void Router::disablePort(int port) {
//     if (port >= 0 && port < numPorts) {
//         portStatus[port] = false;
//     }
// }

bool Router::isPortEnabled(int port) const {
    if (port >= 0 && port < numPorts) {
        return portStatus[port];
    }
    return false;
}

int Router::getNumPorts() const {
    return numPorts;
}

bool Router::connectDevice(device* dev, int port) {
    if (port >= 0 && port < numPorts && isPortEnabled(port)) {
        connectedDevices.push_back(dev);
        return true;
    }
    return false;
}

bool Router::disconnectDevice(int port) {
    if (port >= 0 && port < numPorts) {
        auto it = std::find_if(connectedDevices.begin(), connectedDevices.end(),
            [port](const device* dev) { return dev->port == port; });
        
        if (it != connectedDevices.end()) {
            connectedDevices.erase(it);
            return true;
        }
    }
    return false;
}

void Router::listConnectedDevices() const {
    std::cout << "Connected devices:" << std::endl;
    for (const auto& dev : connectedDevices) {
        std::cout << "Type: " << dev->type << ", IP: " << dev->ip << ", MAC: " << dev->mac << std::endl;
    }
}

bool Router::forwardPacket(const std::string& sourceIP, const std::string& destIP) { 
    std::string nextHop = getNextHop(destIP);
    if (!nextHop.empty()) {
        std::cout << "Forwarding packet from " << sourceIP << " to " << destIP << " via " << nextHop << std::endl;
        return true;
    } else {
        std::cout << "No route found for destination " << destIP << std::endl;
        return false;
    }
}
