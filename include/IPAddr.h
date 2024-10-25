#ifndef IPADDR
#define IPADDR

#include <iostream>
#include <unordered_set>
#include <random>
#include <string>
#include <vector>
#include <sstream>

class IP {
    private:
        std::unordered_set<std::string> localIPs;   // Stores unique local IPs
        std::unordered_set<std::string> publicIPs;  // Stores unique public IPs

        // Helper to split IP address into octets
        std::vector<int> splitIP(const std::string& ip) const;

        // Calculate the IP range based on network and subnet mask
        std::pair<std::vector<int>, std::vector<int>> calculateRange(const std::vector<int>& network, const std::vector<int>& mask) const;

        // Convert vector of octets to string format IP address
        std::string vectorToIP(const std::vector<int>& octets) const;

        // Generate a random IP within the specified range
        std::string generateRandomIPInRange(const std::vector<int>& startIP, const std::vector<int>& endIP) const;

    public:
        IP() = default;

        // Generate a unique local IP based on network and subnet mask
        std::string generateUniqueLocalIP(const std::string& networkAddress, const std::string& subnetMask);

        // Generate a unique public IP (ignoring subnet mask)
        std::string generateUniquePublicIP();

        // Generate a unique IP, distinguishing between local and public
        std::string generateUniqueIP(const std::string& networkType, const std::string& networkAddress = "", const std::string& subnetMask = "") {
            if (networkType == "local") {
                return generateUniqueLocalIP(networkAddress, subnetMask);
            } else if (networkType == "public") {
                return generateUniquePublicIP();
            } else {
                throw std::invalid_argument("Invalid network type. Choose 'local' or 'public'.");
            }
        }
};

#endif
