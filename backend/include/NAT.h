#ifndef NAT_H
#define NAT_H

#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<unordered_map>
#include<unordered_set>
#include<thread>
#include<chrono>
#include<cstdlib>
#include<ctime>
#include<utility>

class NATTable{
	unordered_map<string,pair<string,int>> natTable;

	public:
		void addMapping(const std::string &privateIP,int privatePort,const std::string &publicIP,int publicPort);
		std::pair<string,int> getMapping(const std::string &privateIP,int privatePort);
};


class PortManager{
	unordered_map<string,unordered_set<int>> assignedPorts;

	public:
		int assignPort(const string &deviceIP);
};







#endif
