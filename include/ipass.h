#ifndef IPASS_H
#define IPASS_H

#include <string>
#include <random>
#include <unordered_set>

class IPAddr {
private:
    std::unordered_set<std::string> usedPublicIPs; // To track used public IPs
    std::unordered_set<std::string> usedPrivateIPs; // To track used private IPs

    // Function to generate a random public IP address
    std::string generatePublicIP() {
        // Randomly generate a public IP address
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 254); // Valid public IP range

        return std::to_string(dist(gen)) + "." +
               std::to_string(dist(gen)) + "." +
               std::to_string(dist(gen)) + "." +
               std::to_string(dist(gen));
    }

    // Function to generate a random private IP address
    std::string generatePrivateIP() {
        // Randomly generate a private IP address
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 254); // Valid private IP range

        return "192.168." + std::to_string(dist(gen)) + "." + std::to_string(dist(gen));
    }

public:
    // Constructor
    IPAddr() = default;

    // Method to assign a unique public IP
    std::string assignPublicIP() {
        std::string ip;
        do {
            ip = generatePublicIP();
        } while (usedPublicIPs.find(ip) != usedPublicIPs.end());

        usedPublicIPs.insert(ip);
        return ip;
    }

    // Method to assign a unique private IP
    std::string assignPrivateIP() {
        std::string ip;
        do {
            ip = generatePrivateIP();
        } while (usedPrivateIPs.find(ip) != usedPrivateIPs.end());

        usedPrivateIPs.insert(ip);
        return ip;
    }

    // Method to clear used IP addresses (if needed)
    void clearUsedIPs() {
        usedPublicIPs.clear();
        usedPrivateIPs.clear();
    }
};

#endif // IPASS_H
