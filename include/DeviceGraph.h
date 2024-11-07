#ifndef DEVICEGRAPH_H
#define DEVICEGRAPH_H

#include "Device.h"
#include <unordered_map>
#include <string>
#include <stdexcept>

using namespace std;

// Type alias for the Device Graph
using DeviceGraph = unordered_map<string, device*>;  // A map to store devices by their public IP address

// Function declarations for searching devices
device* searchDeviceByIP(DeviceGraph& graph, const string& ip);
device* searchDeviceByPrivateIP(DeviceGraph& graph, const string& ip);
device* searchDeviceByMAC(DeviceGraph& graph, const string& mac);

#endif // DEVICEGRAPH_H
