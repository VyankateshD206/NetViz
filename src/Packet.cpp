#include "Packet.h"

//constructor
Packet::Packet(std::string source_ip, std::string destination_ip, int source_port,int destination_port){
    this->source_ip = source_ip;
    this->destination_ip = destination_ip;
    this->source_port = source_port;
    this->destination_port = destination_port;
}


//display packet info
void Packet::display_packet_info() const{
    std::cout << "Source IP: " << source_ip << std::endl;
    std::cout << "Destination IP: " << destination_ip << std::endl;
    std::cout << "Source Port: " << source_port << std::endl;
    std::cout << "Destination Port: " << destination_port << std::endl;
    std::cout << "Protocol: " << protocol << std::endl;
    std::cout << "Data: " << data << std::endl;
}