#include "../include/Device.h"

using namespace std;

// Constructor implementation
device::device(Device_Types type, const string &public_ip, const string &private_ip, const string &mac) 
    : type(type), public_ip(public_ip), private_ip(private_ip), mac(mac), port(-1) {
}

// Copy constructor
device::device(const device& dev) 
    : type(dev.type), public_ip(dev.public_ip), private_ip(dev.private_ip), 
      mac(dev.mac), port(dev.port) {
}

// Assignment operator
device& device::operator=(const device& d) {
    if (this != &d) {
        type = d.type;
        public_ip = d.public_ip;
        private_ip = d.private_ip;
        mac = d.mac;
        port = d.port;
    }
    return *this;
}

// Destructor
device::~device() {
    type = NONE;
    public_ip = "";
    private_ip = "";
    mac = "";
    port = -1;
}
