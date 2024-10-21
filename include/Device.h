#ifndef DEVICE_H
#define DEVICE_H

#include<iostream>
#include<string>

enum Device_Types{NONE,SWITCH,BRIDGE,ROUTER,END_POINT};

class device{
    public:
        Device_Types type;
        std::string ip;
        std::string mac;

        //default constructor
        device(Device_Types type,std::string ip,std::string mac);
        //copy constructor
        device(const device *dev);
        //assignment operator overloading
        device operator=(const device *d);
        //destructor
        ~device();
};

#endif