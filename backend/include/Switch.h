#ifndef SWITCH_H
#define SWITCH_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Device.h"

class Switch : public device {
private:
    int numPorts;
    std::vector<bool> portStatus;
    std::vector<std::pair<int, int>> macAddressTable; // <port, MAC address>
    std::vector<device*> connectedDevices;

public:
    Switch(std::string ip, std::string mac, int ports);
    ~Switch();

    // Switch-specific methods:
    void addMacAddress(int port, int macAddress); //add mac address to the switch
    void removeMacAddress(int port); //remove mac address from the switch
    int getPortForMac(int macAddress) const; //get the port for a given mac address
    void enablePort(int port); //enable a port
    void disablePort(int port); //disable a port
    bool isPortEnabled(int port) const; //check if a port is enabled
    int getNumPorts() const; //get the number of ports

    // methods for device connections:
    bool connectDevice(device* dev, int port); //connect a device to a port
    bool disconnectDevice(int port); //disconnect a device from a port
    void listConnectedDevices() const; //list all connected devices
};

#endif
