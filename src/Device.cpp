#include "../include/Device.h"
using namespace std;

device::device(Device_Types type,string ip,string mac){
    this->type = type;
    this->ip = ip;
    this->mac = mac;
}

device::device(device* dev){
    this->type = dev->type;
    this->ip = dev->type;
    this->mac = dev->mac;
}

device device::operator=(device *d){
    device mydev = device(d->type,d->ip,d->mac);
    return mydev;
}

device::~device(){
    this->type = NONE;
    this->ip = "";
    this->mac = "";
}