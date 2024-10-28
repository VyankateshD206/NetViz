#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Device.h"

class Router : public device {
private:
    int numPorts;
    std::vector<bool> portStatus;
    std::map<std::string, std::string> routingTable; // <destination IP, next hop IP>
    std::vector<device*> connectedDevices;

public:
    Router(std::string ip, std::string mac, int ports);
    ~Router();

    // Router-specific methods
    void addRoute(const std::string& destIP, const std::string& nextHopIP); //add a route to the routing table
    void removeRoute(const std::string& destIP); //remove a route from the routing table
    std::string getNextHop(const std::string& destIP) const; //get the next hop for a given destination IP
    void enablePort(int port); //enable a port
    void disablePort(int port); //disable a port
    bool isPortEnabled(int port) const; //check if a port is enabled
    int getNumPorts() const; //get the number of ports

    // Methods for device connections
    bool connectDevice(device* dev, int port);
    bool disconnectDevice(int port);
    void listConnectedDevices() const;

    // Routing functionality
    bool forwardPacket(const std::string& sourceIP, const std::string& destIP);
};

#endif
