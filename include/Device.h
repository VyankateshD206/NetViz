#ifndef DEVICE_H
#define DEVICE_H

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
        device(device *dev);
        //assignment operator overloading
        device operator=(device *d);
        //destructor
        ~device();
};

#endif