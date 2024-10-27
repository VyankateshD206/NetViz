#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "Device.h"

class Network {
private:
    std::vector<device> devices;  // List of devices in the network
    std::unordered_map<std::string, std::vector<std::string>> connections;  // Connections between devices

public:
    Network() = default;

    // Method to add a device to the network
    void addDevice(Device_Types type, const std::string& public_ip, const std::string& private_ip, const std::string& mac);

    // Method to remove a device from the network by its MAC address
    bool removeDevice(const std::string& mac);

    // Method to connect two devices
    void connectDevices(const std::string& mac1, const std::string& mac2);

    // Method to display all devices in the network
    void displayDevices() const;

    // Method to display connections between devices
    void displayConnections() const;

    // Optional: Method to clear the network
    void clearNetwork();

    // Getters for devices and connections
    const std::vector<device>& getDevices() const { return devices; }
    const std::unordered_map<std::string, std::vector<std::string>>& getConnections() const { return connections; }
};

#endif // NETWORK_H
