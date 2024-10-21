#ifndef NETWORK_H
#define NETWORK_H

#include<iostream>
#include<vector>
#include<unordered_map>
#include<algorithm>
#include"../include/Device.h"

class Network{
    private:
        std::vector<device> devices;
        std::unordered_map<std::string,std::vector<std::string>> connections;

    public:
        Network() = default;

        void addDevice(Device_Types type,const std::string &ip,const std::string &mac);

        // Method to remove a device from the network by its MAC address
        bool removeDevice(const std::string &mac);

        // Method to connect two devices
        void connectDevices(const std::string &mac1, const std::string &mac2);

        // Method to display all devices in the network
        void displayDevices() const;

        // Method to display connections between devices
        void displayConnections() const;

        // Optional: Method to clear the network
        void clearNetwork();
        const std::vector<device>& getDevices() const { return devices; }
        const std::unordered_map<std::string, std::vector<std::string>>& getConnections() const { return connections; }

};

#endif