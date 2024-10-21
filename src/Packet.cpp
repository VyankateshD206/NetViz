#include "Packet.h"
using namespace std;

// constructor
DataPacket::DataPacket(const std::string &sa,const std::string &da,const std::string &d,const NetIdentifier &id){
    if(id == SAME_NETWORK){
        this->dstMac = da;
        this->srcMac = sa;
        this->dstIP = "NULL";
        this->srcIP = "NULL";
    }
    else if(id == NOT_SAME_NETWORK){
        this->dstIP = da;
        this->srcIP = sa;
        this->dstMac = "NULL";
        this->srcMac = "NULL";
    }

    this->dataBuff = d;
    this->identifier = id;
}

// copy constructor
DataPacket::DataPacket(const DataPacket &p){
    this->identifier = p.identifier;
    this->dataBuff = p.dataBuff;
    this->dstMac = p.dstMac;
    this->dstIP = p.dstIP;
    this->srcMac = p.srcMac;
    this->srcIP = p.srcIP;
}

// method to read packet data
void DataPacket::ReadPacket() const{
    if(this->identifier == NOT_SAME_NETWORK)
        std::cout<<"Packet from "<<this->srcIP<<" To "<<this->dstIP<<" Containing Data "<<this->dataBuff;

    else if(this->identifier == SAME_NETWORK)
        std::cout<<"Packet from "<<this->srcMac<<" To "<<this->dstMac<<" Containing Data "<<this->dataBuff;
}

