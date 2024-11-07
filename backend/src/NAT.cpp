#include "NAT.h"

using namespace std;

void NATTable::addMapping(const string &privateIP,int privatePort,const string &publicIP,int publicPort){
	
	string key = privateIP+":"+to_string(privatePort);
	natTable[key] = {publicIP,publicPort};
}


pair<string,int> NATTable::getMapping(const string &privateIP,int privatePort){
	
	string key = privateIP+":"+to_string(privatePort);
	if(natTable[key] != natTable.end()){
		return natTable[key];
	}
	return {"",-1};
}



int PortManager::assignPort(const string &deviceIP){
        int port;
        do {
            port = rand() % 64512 + 1024; // Assign ports in the range 1024-65535
        } while (assignedPorts[deviceIP].count(port)); // Ensure port is unique for this IP

        assignedPorts[deviceIP].insert(port);
        return port;
}
