#include "Packet.h"

using namespace std;

//constructor
Packet::Packet(string &sIP,string &dIP,stirng &content,int id,int tl,int sp,int dp):
source_ip(sIP),destination_ip(dIP),packetID(id),fragmentID(0),ttl(tl),source_port(sp),destination_port(dp),data(content){
	priority = data.size()>20?2:1; // smaller data packets are given higher priority
	checkSum = calculatecheckSum(data);	
}

// method to calculate the checkSum
int Packet::calculatecheckSum(const string &data){
	int sum = 0;
	for(char c:data) sum+= c;
	return sum%256;
}


//display packet info
void Packet::display_packet_info() const{
    cout<<"-----PACKET_INFO-----"<<endl;
    cout<<"[+] PACKET-ID: "<<packetID<<endl;
    cout<<"[+] FRAGMENT-ID: "<<fragmentID<<endl;
    cout<<"[+] TIME-TO-LIVE: "<<ttl<<endl;
    cout<<"[+] PRIORITY: "<<priority<<endl;
    cout<<"[+] CHECKSUM: "<<checkSum<<endl;
    cout<<"[+] DATA: "<<data<<endl;

    cout<<"-----SOURCE-INFO-----"<<endl;
    cout<<"[+] SOURCE-IP: "<<source_ip<<endl;
    cout<<"[+] SOURCE-PORT: "<<source_port<<endl;
    

    cout<<"-----DESTINATION-INFO-----"<<endl;
    cout<<"[+] DESTINATION-IP: "<<destination_ip<<endl;
    cout<<"[+] DESTINATION-PORT: "<<destination_port<<endl;
}
