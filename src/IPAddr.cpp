#include "IPAddr.h"
using namespace std;

// method to split ip's into it's octets
vector<int> IP::splitIP(const string& ip) const{
    std::vector<int> octets;
    std::stringstream ss(ip);
    std::string oct;

    while(std::getline(ss,oct,'.')){
        octets.push_back(std::stoi(oct));
    }

    return octets;
}

// Calculate the IP range based on network and subnet mask
std::pair<std::vector<int>, std::vector<int>> IP::calculateRange(const std::vector<int>& network, const std::vector<int>& mask) const {
    std::vector<int> startIP(4), endIP(4);
    for (int i = 0; i < 4; ++i) {
        startIP[i] = network[i] & mask[i];
        endIP[i] = startIP[i] | (~mask[i] & 255);
    }
    return {startIP, endIP};
}

// Convert vector of octets to string format IP address
std::string IP::vectorToIP(const std::vector<int>& octets) const {
    std::stringstream ss;
    for (size_t i = 0; i < octets.size(); ++i) {
        ss << octets[i];
        if (i < octets.size() - 1) ss << ".";
    }
    return ss.str();
}

// Generate a random IP within the specified range
std::string IP::generateRandomIPInRange(const std::vector<int>& startIP, const std::vector<int>& endIP) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> randomIP(4);
    for (int i = 0; i < 4; ++i) {
        std::uniform_int_distribution<> dis(startIP[i], endIP[i]);
        randomIP[i] = dis(gen);
    }
    return vectorToIP(randomIP);
}

// Generate a unique local IP based on network and subnet mask
std::string IP::generateUniqueLocalIP(const std::string& networkAddress, const std::string& subnetMask) {
    std::vector<int> network = splitIP(networkAddress);
    std::vector<int> mask = splitIP(subnetMask);
    auto [startIP, endIP] = calculateRange(network, mask);

    std::string ip;
    do {
        ip = generateRandomIPInRange(startIP, endIP);
    } while (localIPs.count(ip) > 0);  // Ensure uniqueness within local IPs

    localIPs.insert(ip);  // Add IP to local set
    return ip;
}

// Generate a unique public IP (ignoring subnet mask)
std::string IP::generateUniquePublicIP() {
    // Define a reasonable public IP range, e.g., "100.0.0.0" to "200.255.255.255"
    std::vector<int> startIP = {100, 0, 0, 0};
    std::vector<int> endIP = {200, 255, 255, 255};

    std::string ip;
    do {
        ip = generateRandomIPInRange(startIP, endIP);
    } while (publicIPs.count(ip) > 0);  // Ensure uniqueness within public IPs

    publicIPs.insert(ip);  // Add IP to public set
    return ip;
}

// Generate a unique IP, distinguishing between local and public
std::string IP::generateUniqueIP(const std::string& networkType, const std::string& networkAddress = "", const std::string& subnetMask = "") {
    if (networkType == "local") {
        return generateUniqueLocalIP(networkAddress, subnetMask);
    } else if (networkType == "public") {
        return generateUniquePublicIP();
    } else {
        throw std::invalid_argument("Invalid network type. Choose 'local' or 'public'.");
    }
}

