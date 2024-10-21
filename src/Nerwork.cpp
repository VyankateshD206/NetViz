#include"Network.h"

Network::Network() = default;

void Network::addDevice(Device_Types type,const std::string &ip,const std::string &mac){
    devices.emplace_back(type,ip,mac);
    connections[mac];
}

// Method to remove a device from the network by its MAC address
bool Network::removeDevice(const std::string &mac) {
    auto it = std::remove_if(devices.begin(), devices.end(), [&mac](const device &dev) {
        return dev.mac == mac;
    });

    if (it != devices.end()) {
        // Remove the device from connections
        connections.erase(mac);
        devices.erase(it, devices.end());
        // Remove any connections to this device
        for (auto &conn : connections) {
            conn.second.erase(std::remove(conn.second.begin(), conn.second.end(), mac), conn.second.end());
        }
        return true; // Device found and removed
    }
    return false; // Device not found
}

// Method to connect two devices
void Network::connectDevices(const std::string &mac1, const std::string &mac2) {
    connections[mac1].push_back(mac2);
    connections[mac2].push_back(mac1); // For undirected graph; remove if directed
}

// Method to display all devices in the network
void Network::displayDevices() const {
    if (devices.empty()) {
        std::cout << "No devices in the network." << std::endl;
        return;
    }

    std::cout << "Devices in the network:" << std::endl;
    for (const auto &dev : devices) {
        std::cout << "Type: " << dev.type << ", IP: " << dev.ip << ", MAC: " << dev.mac << std::endl;
    }
}

// Method to display connections between devices
void Network::displayConnections() const {
    std::cout << "Device Connections:" << std::endl;
    for (const auto &conn : connections) {
        std::cout << "Device (" << conn.first << ") is connected to: ";
        for (const auto &neighbor : conn.second) {
            std::cout << neighbor << " ";
        }
        std::cout << std::endl;
    }
}

// Optional: Method to clear the network
void Network::clearNetwork() {
    devices.clear();
    connections.clear();
}