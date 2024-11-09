// NetworkVisualizer.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <map>
#include "Device.h"
#include "Network.h"
#include "IPAddr.h"

class NodeShape {
public:
    sf::CircleShape shape;
    sf::Text label;
    sf::Text info;
    Device_Types type;
    std::string mac;
    std::string publicIP;
    std::string privateIP;
    bool isDragging;
    sf::Vector2f dragOffset;

    NodeShape(Device_Types t, const sf::Font& font, const std::string& mac, 
              const std::string& pubIP, const std::string& privIP);
    void setPosition(float x, float y);
    bool contains(sf::Vector2f point);
    void updateInfo();
};

class EdgeShape {
public:
    sf::VertexArray line;
    NodeShape* source;
    NodeShape* destination;

    EdgeShape(NodeShape* src, NodeShape* dest);
    void update();
};

class PacketAnimation {
public:
    sf::CircleShape shape;
    EdgeShape* currentEdge;
    float progress;
    std::vector<EdgeShape*> path;
    size_t currentPathIndex;
    bool isActive;

    PacketAnimation();
    void update(float deltaTime);
    void start(const std::vector<EdgeShape*>& path);
    void reset();
};

class NetworkVisualizer {
private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<std::unique_ptr<NodeShape>> nodes;
    std::vector<std::unique_ptr<EdgeShape>> edges;
    sf::RectangleShape menuButton;
    bool menuOpen;
    sf::RectangleShape nodeMenu;
    std::vector<sf::RectangleShape> nodeButtons;
    std::vector<sf::Text> nodeLabels;
    NodeShape* selectedNode;
    NodeShape* draggedNode;
    PacketAnimation packetAnimation;
    bool animationMode;
    NodeShape* sourceNode;
    IP ipGenerator;

    void handleEvents();
    void update();
    void draw();
    void createNodeMenu();
    NodeShape* createNode(Device_Types type, float x, float y);
    void tryCreateEdge(NodeShape* source, NodeShape* destination);
    std::vector<EdgeShape*> findPath(NodeShape* source, NodeShape* destination);

public:
    NetworkVisualizer();
    void run();
};
