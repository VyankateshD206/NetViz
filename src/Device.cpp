#include "Device.h"

using namespace std;

// Constructor for device, accepts both public and private IPs
device::device(Device_Types type, const string &publicIp, const string &privateIp, const string &mac) {
    this->type = type;
    this->publicIp = publicIp;   // Store public IP
    this->privateIp = privateIp;  // Store private IP
    this->mac = mac;              // Store MAC address
}

// Copy constructor
device::device(const device* dev) {
    this->type = dev->type;
    this->publicIp = dev->publicIp;   // Correctly copy public IP
    this->privateIp = dev->privateIp; // Correctly copy private IP
    this->mac = dev->mac;
}

// Assignment operator
device& device::operator=(const device *d) {
    if (this != d) { // Avoid self-assignment
        this->type = d->type;
        this->publicIp = d->publicIp;    // Assign public IP
        this->privateIp = d->privateIp;  // Assign private IP
        this->mac = d->mac;
    }
    return *this;
}

// Destructor
device::~device() {
    this->type = NONE;
    this->publicIp = "";   // Clear public IP
    this->privateIp = "";  // Clear private IP
    this->mac = "";        // Clear MAC address
}
