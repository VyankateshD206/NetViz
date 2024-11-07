#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <string>
#include <vector>

class Packet{
    public:
        std::string source_ip;
        std::string destination_ip;
        int         source_port;
        int         destination_port;

        std::string data;
        std::string protocol;

        //constructor
        Packet(std::string source_ip, std::string destination_ip, int source_port,int destination_port);

        //display packet info
        void display_packet_info() const;


        
};
#endif