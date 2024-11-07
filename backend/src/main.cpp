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
};

class NetworkHandler : public Pistache::Http::Handler {
private:
    std::shared_ptr<NetworkData> network_data;

public:
    HTTP_PROTOTYPE(NetworkHandler)

    NetworkHandler() : network_data(std::make_shared<NetworkData>()) {
        // Create sample network devices
        network_data->addRouter("R1", "192.168.1.1", "10.0.0.1", "00:11:22:33:44:55", 4);
        network_data->addRouter("R2", "192.168.2.1", "10.0.0.2", "00:11:22:33:44:56", 4);
        
        network_data->addSwitch("S1", "192.168.1.2", "10.0.0.3", "00:11:22:33:44:57", 8);
        network_data->addSwitch("S2", "192.168.2.2", "10.0.0.4", "00:11:22:33:44:58", 8);
        
        network_data->addEndPoint("D1", "192.168.1.100", "10.0.0.100", "00:11:22:33:44:59");
        network_data->addEndPoint("D2", "192.168.1.101", "10.0.0.101", "00:11:22:33:44:60");

        // Create connections
        network_data->connectDevices("R1", "S1");
        network_data->connectDevices("S1", "D1");
        network_data->connectDevices("S1", "D2");
        network_data->connectDevices("R1", "R2");
        network_data->connectDevices("R2", "S2");
    }

    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) override {
        response.headers()
            .add<Pistache::Http::Header::AccessControlAllowOrigin>("*")
            .add<Pistache::Http::Header::AccessControlAllowMethods>("GET, POST, OPTIONS")
            .add<Pistache::Http::Header::AccessControlAllowHeaders>("Content-Type");

        if (request.resource() == "/api/network" && request.method() == Pistache::Http::Method::Get) {
            std::string jsonData = network_data->toJson().dump();
            response.send(Pistache::Http::Code::Ok, jsonData, MIME(Application, Json));
            std::cout << "Sending data: " << jsonData << std::endl;
        } else if (request.resource() == "/api/nodes" && request.method() == Pistache::Http::Method::Post) {
            try {
                auto body = nlohmann::json::parse(request.body());
                std::string id = body["id"];
                std::string label = body["label"];
                Node node{
                    id,
                    label,
                    "device",  // default device type
                    {}        // empty properties map
                };
                network_data->addNode(node);
                response.send(Pistache::Http::Code::Ok, "Node added successfully");
            } catch (const std::exception& e) {
                response.send(Pistache::Http::Code::Bad_Request, "Invalid input");
            }
        } else {
            response.send(Pistache::Http::Code::Not_Found, "Not Found");
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
