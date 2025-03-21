#include <iostream>
#include "Network.h"
#include "Device.h"
#include "a-star.h"
#include "IPAddr.h" // Include the IPAddr class header if needed

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

    // A* algorithm instance with access to the network
    AStarAlgorithm aStar;
    aStar.network = myNetwork; // Ensure network is accessible in AStarAlgorithm

    // Finding path from SWITCH to END_POINT
    device* startDevice = aStar.findDeviceByMac("00:1A:2B:3C:4D:5E");
    device* goalDevice = aStar.findDeviceByMac("00:1A:2B:3C:4D:60");

    if (startDevice && goalDevice) {
        vector<device*> path = aStar.aStar(startDevice, goalDevice);

        // Output the path
        cout << "Path from " << startDevice->mac << " to " << goalDevice->mac << ": ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i]->mac;
            if (i < path.size() - 1) {
                cout << " -> "; // Use an arrow to indicate path direction
            }
        }
        cout << endl;
    } else {
        cout << "Start or Goal device not found!" << endl;
    }

    // Remove a device
    if (myNetwork.removeDevice("00:1A:2B:3C:4D:5F")) {
        cout << "Device removed successfully." << endl;
    } else {
        cout << "Device not found." << endl;
    }

    // Display devices after removal
    myNetwork.displayDevices();
    
    // Display connections after removal
    myNetwork.displayConnections();

    return 0;
}


