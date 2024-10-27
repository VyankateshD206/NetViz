#include "IPAddr.h"
#include <random>
#include <sstream>
#include <stdexcept>

using namespace std;

// Split IPs into octets
vector<int> IP::splitIP(const string& ip) const {
    vector<int> octets;
    stringstream ss(ip);
    string oct;

    while (getline(ss, oct, '.')) {
        octets.push_back(stoi(oct));
    }

    return octets;
}

// Calculate the IP range based on network and subnet mask
pair<vector<int>, vector<int>> IP::calculateRange(const vector<int>& network, const vector<int>& mask) const {
    vector<int> startIP(4), endIP(4);
    for (int i = 0; i < 4; ++i) {
        startIP[i] = network[i] & mask[i];
        endIP[i] = startIP[i] | (~mask[i] & 255);
    }
    return {startIP, endIP};
}

// Convert vector of octets to string format IP address
string IP::vectorToIP(const vector<int>& octets) const {
    stringstream ss;
    for (size_t i = 0; i < octets.size(); ++i) {
        ss << octets[i];
        if (i < octets.size() - 1) ss << ".";
    }
    return ss.str();
}

// Generate a random IP within the specified range
string IP::generateRandomIPInRange(const vector<int>& startIP, const vector<int>& endIP) const {
    random_device rd;
    mt19937 gen(rd());
    vector<int> randomIP(4);
    for (int i = 0; i < 4; ++i) {
        uniform_int_distribution<> dis(startIP[i], endIP[i]);
        randomIP[i] = dis(gen);
    }
    return vectorToIP(randomIP);
}

// Generate a unique local IP based on network and subnet mask
string IP::generateUniqueLocalIP(const string& networkAddress, const string& subnetMask) {
    vector<int> network = splitIP(networkAddress);
    vector<int> mask = splitIP(subnetMask);
    auto [startIP, endIP] = calculateRange(network, mask);

    string ip;
    do {
        ip = generateRandomIPInRange(startIP, endIP);
    } while (localIPs.count(ip) > 0); // Ensure uniqueness within local IPs

    localIPs.insert(ip); // Add IP to local set
    return ip;
}

// Generate a unique public IP (ignoring subnet mask)
string IP::generateUniquePublicIP() {
    vector<int> startIP = {100, 0, 0, 0};
    vector<int> endIP = {200, 255, 255, 255};

    string ip;
    do {
        ip = generateRandomIPInRange(startIP, endIP);
    } while (publicIPs.count(ip) > 0); // Ensure uniqueness within public IPs

    publicIPs.insert(ip); // Add IP to public set
    return ip;
}

// Generate a unique IP, distinguishing between local and public
string IP::generateUniqueIP(const string& networkType, const string& networkAddress = "", const string& subnetMask = "") {
    if (networkType == "local") {
        return generateUniqueLocalIP(networkAddress, subnetMask);
    } else if (networkType == "public") {
        return generateUniquePublicIP();
    } else {
        throw invalid_argument("Invalid network type. Choose 'local' or 'public'.");
    }
}
