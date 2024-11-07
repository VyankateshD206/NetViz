#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <string>
#include <vector>

class Packet{
	public:
        	std::string source_ip; // ip of the sender
        	std::string destination_ip; // ip of the target
        	int         source_port; // port number of the sender
        	int         destination_port; // port number of the target
	
        	std::string data; // data content of the packet
        	int packetID; // id of the packet
		int fragmentID; // id of the fragment of the packet
		int ttl; // Time-To-Live Of the packet
		int checkSum; // simulated checkSum to verify data integrity
	
		int priority; // the priority of the data packet

        	//constructor
        	Packet(std::string &sIP,std::string &dIP,std::string &content,int id,int tl,int sp,int dp);

	

        	//display packet info
        	void display_packet_info() const;

	private:
		
		int calculatecheckSum(const std::string &data) const;
		

        
};
#endif
