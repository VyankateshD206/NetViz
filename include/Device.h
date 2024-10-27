#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>
#include <string>

enum Device_Types { NONE, SWITCH, BRIDGE, ROUTER, END_POINT };

class device {
public:
    Device_Types type;
    std::string public_ip;  // Changed to public_ip
    std::string private_ip;  // Added private_ip
    std::string mac;

    // Default constructor
    device(Device_Types type, const std::string& public_ip, const std::string& private_ip, const std::string& mac)
        : type(type), public_ip(public_ip), private_ip(private_ip), mac(mac) {}

    // Copy constructor
    device(const device& dev)
        : type(dev.type), public_ip(dev.public_ip), private_ip(dev.private_ip), mac(dev.mac) {}

    // Assignment operator overloading
    device& operator=(const device& d) {
        if (this != &d) { // Check for self-assignment
            type = d.type;
            public_ip = d.public_ip;
            private_ip = d.private_ip;
            mac = d.mac;
        }
        return *this;
    }

    // Destructor
    ~device() {
        // No dynamic memory to release in this case
    }
};

#endif // DEVICE_H


#endif