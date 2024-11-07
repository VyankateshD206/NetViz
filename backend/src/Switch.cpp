#include "../include/Switch.h"
#include "../include/Device.h"
#include <algorithm>

Switch::Switch(std::string ip, std::string mac, int ports) 
    : device(SWITCH, ip, "", mac), numPorts(ports) {
    portStatus.resize(numPorts, true);
}

Switch::~Switch() {
    // Clean up any dynamically allocated memory if necessary
}

void Switch::addMacAddress(int port, int macAddress) { //add the mac address to the mac address table
    macAddressTable.push_back(std::make_pair(port, macAddress));
}

void Switch::removeMacAddress(int port) { //remove the mac address from the mac address table
    macAddressTable.erase(
        std::remove_if(macAddressTable.begin(), macAddressTable.end(),
            [port](const std::pair<int, int>& entry) { return entry.first == port; }),
        macAddressTable.end());
}

int Switch::getPortForMac(int macAddress) const { //get the port for a given mac address
    for (const auto& entry : macAddressTable) {
        if (entry.second == macAddress) {
            return entry.first;
        }
    }
    return -1; 
}
// we will use thsese when needed.
// void Switch::enablePort(int port) {
//     if (port >= 0 && port < numPorts) {
//         portStatus[port] = true;
//     }
// }

// void Switch::disablePort(int port) {
//     if (port >= 0 && port < numPorts) {
//         portStatus[port] = false;
//     }
// }

bool Switch::isPortEnabled(int port) const {
    if (port >= 0 && port < numPorts) {
        return portStatus[port];
    }
    return false;
}

int Switch::getNumPorts() const {
    return numPorts;
}

bool Switch::connectDevice(device* dev, int port) {
    if (port >= 0 && port < numPorts && isPortEnabled(port)) {
        if (dev->type == END_POINT || dev->type == ROUTER) {
            connectedDevices.push_back(dev);
            try {
                addMacAddress(port, std::stoi(dev->mac));
            } catch (const std::exception& e) {
                std::cerr << "Error converting MAC address: " << e.what() << std::endl;
                return false;
            }
            return true;
        }
    }
    return false;
}

bool Switch::disconnectDevice(int port) {
    if (port >= 0 && port < numPorts) {
        auto it = std::find_if(connectedDevices.begin(), connectedDevices.end(),
            [this, port](const device* dev) {
                try {
                    return getPortForMac(std::stoi(dev->mac)) == port;
                } catch (const std::exception& e) {
                    std::cerr << "Error converting MAC address: " << e.what() << std::endl;
                    return false;
                }
            });
        
        if (it != connectedDevices.end()) {
            connectedDevices.erase(it);
            removeMacAddress(port);
            return true;
        }
    }
    return false;
}

//list all connected devices
void Switch::listConnectedDevices() const {
    std::cout << "Connected devices:" << std::endl;
    for (const auto& dev : connectedDevices) {
        std::cout << "Type: " << dev->type 
                  << ", IP: " << dev->public_ip  // Changed from ip to public_ip
                  << ", MAC: " << dev->mac << std::endl;
    }
}
