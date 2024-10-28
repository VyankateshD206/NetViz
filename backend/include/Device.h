#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>
#include <string>

enum Device_Types { NONE, SWITCH, BRIDGE, ROUTER, END_POINT };

class device {
public:
    Device_Types type;
    std::string public_ip;
    std::string private_ip;
    std::string mac;
    int port;  // Add port member

    // Default constructor - remove implementation from here
    device(Device_Types type, const std::string& public_ip, const std::string& private_ip, const std::string& mac);

    // Copy constructor
    device(const device& dev);

    // Assignment operator overloading
    device& operator=(const device& d);

    // Destructor
    ~device();
};

#endif // DEVICE_H
