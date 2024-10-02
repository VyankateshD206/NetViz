#include<iostream>
#include "Network.h"
#include "Device.h"

using namespace std;

int main() {
    Network myNetwork;

    // Adding devices to the network
    myNetwork.addDevice(SWITCH, "192.168.1.1", "00:1A:2B:3C:4D:5E");
    myNetwork.addDevice(ROUTER, "192.168.1.2", "00:1A:2B:3C:4D:5F");
    myNetwork.addDevice(END_POINT, "192.168.1.10", "00:1A:2B:3C:4D:60");

    // Connecting devices
    myNetwork.connectDevices("00:1A:2B:3C:4D:5E", "00:1A:2B:3C:4D:5F");
    myNetwork.connectDevices("00:1A:2B:3C:4D:5F", "00:1A:2B:3C:4D:60");

    // Display devices
    myNetwork.displayDevices();

    // Display connections
    myNetwork.displayConnections();

    // Remove a device
    if (myNetwork.removeDevice("00:1A:2B:3C:4D:5F")) {
        std::cout << "Device removed successfully." << std::endl;
    } else {
        std::cout << "Device not found." << std::endl;
    }

    // Display devices after removal
    myNetwork.displayDevices();
    // Display connections after removal
    myNetwork.displayConnections();

    return 0;
}
