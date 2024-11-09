#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/http_headers.h>
#include <memory>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

// Add this line at the top after includes
using namespace Pistache;

// Update include paths to match your include directory
#include "include/Router.h"
#include "include/Switch.h"
#include "include/Device.h"
#include <any>

// Modify Node struct to include device type and properties
struct Node {
    std::string id;
    std::string label;
    std::string device_type;  // "router", "switch", or "device"
    std::map<std::string, std::string> properties;  // Store device-specific properties
};

struct Edge {
    std::string source;
    std::string target;
};

class NetworkData {
private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::map<std::string, std::shared_ptr<Router>> routers;
    std::map<std::string, std::shared_ptr<Switch>> switches;
    std::map<std::string, std::shared_ptr<device>> devices;
    
    // Add counter for generating unique IDs
    int deviceCounter = 0;

    // Helper function to generate unique IDs
    std::string generateUniqueId(const std::string& prefix) {
        return prefix + "_" + std::to_string(++deviceCounter);
    }

public:
    void addRouter(const std::string& id, const std::string& public_ip, const std::string& private_ip, const std::string& mac, int ports) {
        auto router = std::make_shared<Router>(public_ip, mac, ports);
        routers[id] = router;
        Node node{
            id,
            "Router " + id,
            "router",
            {
                {"public_ip", public_ip},
                {"private_ip", private_ip},
                {"mac", mac},
                {"ports", std::to_string(ports)},
                {"enabled_ports", std::to_string(router->getNumPorts())},
                {"type", "ROUTER"}
            }
        };
        nodes.push_back(node);
    }

    void addSwitch(const std::string& id, const std::string& public_ip, const std::string& private_ip, const std::string& mac, int ports) {
        auto sw = std::make_shared<Switch>(public_ip, mac, ports);
        switches[id] = sw;
        Node node{
            id,
            "Switch " + id,
            "switch",
            {
                {"public_ip", public_ip},
                {"private_ip", private_ip},
                {"mac", mac},
                {"ports", std::to_string(ports)},
                {"enabled_ports", std::to_string(sw->getNumPorts())},
                {"type", "SWITCH"}
            }
        };
        nodes.push_back(node);
    }

    void addEndPoint(const std::string& id, const std::string& public_ip, const std::string& private_ip, const std::string& mac) {
        auto endpoint = std::make_shared<device>(Device_Types::END_POINT, public_ip, private_ip, mac);
        devices[id] = endpoint;
        Node node{
            id,
            "Device " + id,
            "device",
            {
                {"public_ip", public_ip},
                {"private_ip", private_ip},
                {"mac", mac},
                {"type", "END_POINT"}
            }
        };
        nodes.push_back(node);
    }

    // Add method to create connections between devices
    void connectDevices(const std::string& sourceId, const std::string& targetId) {
        // Verify that both devices exist before creating the connection
        bool sourceExists = (routers.count(sourceId) > 0) || 
                          (switches.count(sourceId) > 0) || 
                          (devices.count(sourceId) > 0);
        
        bool targetExists = (routers.count(targetId) > 0) || 
                          (switches.count(targetId) > 0) || 
                          (devices.count(targetId) > 0);

        if (sourceExists && targetExists) {
            edges.push_back({sourceId, targetId});
        }
    }

    nlohmann::json toJson() const {
        nlohmann::json j;
        j["nodes"] = nlohmann::json::array();
        for (const auto& node : nodes) {
            j["nodes"].push_back({
                {"id", node.id},
                {"label", node.label},
                {"device_type", node.device_type},
                {"properties", node.properties}
            });
        }
        j["edges"] = nlohmann::json::array();
        for (const auto& edge : edges) {
            j["edges"].push_back({{"source", edge.source}, {"target", edge.target}});
        }
        return j;
    }

    // Add method to add nodes
    void addNode(const Node& node) {
        nodes.push_back(node);
    }

    // Add new method to handle dynamic node creation
    std::string addNewNode(const std::string& device_type, const nlohmann::json& data) {
        std::string id;
        std::string mac = generateRandomMac(); // New helper function
        
        if (device_type == "router") {
            id = generateUniqueId("R");
            addRouter(id, 
                     "192.168." + std::to_string(deviceCounter) + ".1", // Generate unique IPs
                     "10.0." + std::to_string(deviceCounter) + ".1",
                     mac,
                     4);
        }
        else if (device_type == "switch") {
            id = generateUniqueId("S");
            addSwitch(id,
                     "192.168." + std::to_string(deviceCounter) + ".2",
                     "10.0." + std::to_string(deviceCounter) + ".2",
                     mac,
                     8);
        }
        else if (device_type == "device") {
            id = generateUniqueId("D");
            addEndPoint(id,
                       "192.168." + std::to_string(deviceCounter) + ".100",
                       "10.0." + std::to_string(deviceCounter) + ".100",
                       mac);
        }
        
        return id;
    }

    // Helper function to generate random MAC address
    std::string generateRandomMac() {
        std::stringstream ss;
        for (int i = 0; i < 6; ++i) {
            if (i > 0) ss << ":";
            ss << std::hex << std::setfill('0') << std::setw(2) 
               << (rand() % 256);
        }
        return ss.str();
    }

    // Add method to check if node exists
    bool nodeExists(const std::string& id) const {
        return std::any_of(nodes.begin(), nodes.end(),
            [&id](const Node& node) { return node.id == id; });
    }

    // Add method to remove node and its connections
    bool removeNode(const std::string& id) {
        // Remove all edges connected to this node
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [&id](const Edge& edge) {
                    return edge.source == id || edge.target == id;
                }),
            edges.end());

        // Remove the node
        auto it = std::find_if(nodes.begin(), nodes.end(),
            [&id](const Node& node) { return node.id == id; });
        
        if (it != nodes.end()) {
            nodes.erase(it);
            routers.erase(id);
            switches.erase(id);
            devices.erase(id);
            return true;
        }
        return false;
    }

    bool removeConnectedLinks(const std::string& nodeId) {
        bool linksRemoved = false;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [&nodeId, &linksRemoved](const Edge& edge) {
                    if (edge.source == nodeId || edge.target == nodeId) {
                        linksRemoved = true;
                        return true;
                    }
                    return false;
                }),
            edges.end());
        return linksRemoved;
    }
};

class NetworkHandler : public Pistache::Http::Handler {
private:
    std::shared_ptr<NetworkData> network_data;

public:
    HTTP_PROTOTYPE(NetworkHandler)

    NetworkHandler() : network_data(std::make_shared<NetworkData>()) {
        // Initialize with some sample data
        initializeSampleNetwork();
    }

    void initializeSampleNetwork() {
        // Create sample network devices
        network_data->addRouter("R1", "192.168.1.1", "10.0.0.1", "00:11:22:33:44:55", 4);
        network_data->addSwitch("S1", "192.168.1.2", "10.0.0.2", "00:11:22:33:44:56", 8);
        network_data->addEndPoint("D1", "192.168.1.100", "10.0.0.100", "00:11:22:33:44:57");
        
        // Create some initial connections
        network_data->connectDevices("R1", "S1");
        network_data->connectDevices("S1", "D1");
    }

    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) override {
        response.headers()
            .add<Pistache::Http::Header::AccessControlAllowOrigin>("*")
            .add<Pistache::Http::Header::AccessControlAllowMethods>("GET, POST, DELETE, OPTIONS")
            .add<Pistache::Http::Header::AccessControlAllowHeaders>("Content-Type");

        if (request.method() == Pistache::Http::Method::Options) {
            response.send(Pistache::Http::Code::Ok);
            return;
        }

        try {
            if (request.resource() == "/api/network" && request.method() == Pistache::Http::Method::Get) {
                handleGetNetwork(response);
            }
            else if (request.resource() == "/api/nodes" && request.method() == Pistache::Http::Method::Post) {
                handleAddNode(request, response);
            }
            else if (request.resource() == "/api/edges" && request.method() == Pistache::Http::Method::Post) {
                handleAddEdge(request, response);
            }
            else if (request.resource().find("/api/nodes/") == 0 && request.method() == Pistache::Http::Method::Delete) {
                handleDeleteNode(request, response);
            }
            else if (request.resource().find("/api/nodes/") == 0 && 
                     request.resource().find("/links") != std::string::npos && 
                     request.method() == Pistache::Http::Method::Delete) {
                handleDeleteLinks(request, response);
            }
            else {
                response.send(Pistache::Http::Code::Not_Found, "Not Found");
            }
        }
        catch (const std::exception& e) {
            response.send(Pistache::Http::Code::Internal_Server_Error, 
                         std::string("Internal Server Error: ") + e.what());
        }
    }

private:
    void handleGetNetwork(Pistache::Http::ResponseWriter& response) {
        std::string jsonData = network_data->toJson().dump();
        response.send(Pistache::Http::Code::Ok, jsonData, MIME(Application, Json));
    }

    void handleAddNode(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter& response) {
        try {
            auto body = nlohmann::json::parse(request.body());
            std::string device_type = body["device_type"].get<std::string>();
            
            std::string newId = network_data->addNewNode(device_type, body);
            
            nlohmann::json responseJson = {
                {"status", "success"},
                {"id", newId}
            };
            
            response.send(Pistache::Http::Code::Ok, responseJson.dump(), MIME(Application, Json));
        }
        catch (const std::exception& e) {
            response.send(Pistache::Http::Code::Bad_Request, "Invalid input");
        }
    }

    void handleAddEdge(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter& response) {
        try {
            auto body = nlohmann::json::parse(request.body());
            std::string source = body["source"].get<std::string>();
            std::string target = body["target"].get<std::string>();
            
            network_data->connectDevices(source, target);
            response.send(Pistache::Http::Code::Ok, "Edge added successfully");
        }
        catch (const std::exception& e) {
            response.send(Pistache::Http::Code::Bad_Request, "Invalid input");
        }
    }

    void handleDeleteNode(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter& response) {
        std::string path = request.resource();
        std::string id = path.substr(path.find_last_of('/') + 1);
        
        if (network_data->removeNode(id)) {
            response.send(Pistache::Http::Code::Ok, "Node removed successfully");
        } else {
            response.send(Pistache::Http::Code::Not_Found, "Node not found");
        }
    }

    void handleDeleteLinks(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter& response) {
        std::string path = request.resource();
        std::string nodeId = path.substr(path.find("/nodes/") + 7);
        nodeId = nodeId.substr(0, nodeId.find("/links"));
        
        if (network_data->removeConnectedLinks(nodeId)) {
            response.send(Pistache::Http::Code::Ok, "Links removed successfully");
        } else {
            response.send(Pistache::Http::Code::Not_Found, "No links found");
        }
    }
};

int main() {
    try {
        Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
        auto opts = Pistache::Http::Endpoint::options()
            .threads(1)
            .flags(Pistache::Tcp::Options::ReuseAddr);

        Pistache::Http::Endpoint server(addr);
        server.init(opts);
        server.setHandler(Pistache::Http::make_handler<NetworkHandler>());
        server.serve();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
