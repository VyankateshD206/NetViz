// NetworkVisualizer.cpp
#include "../include/NetworkVisualizer.hpp"
#include "../include/ipass.h"
#include <string>

NodeShape::NodeShape(Device_Types t, const sf::Font& font, const std::string& m,
                    const std::string& pubIP, const std::string& privIP) 
    : type(t), mac(m), publicIP(pubIP), privateIP(privIP), isDragging(false) {
    
    shape.setRadius(30.f);
    shape.setOrigin(30.f, 30.f);
    
    switch(type) {
        case Device_Types::PC:
            shape.setFillColor(sf::Color::Green);
            label.setString("PC");
            break;
        case Device_Types::ROUTER:
            shape.setFillColor(sf::Color::Red);
            label.setString("R");
            break;
        case Device_Types::SWITCH:
            shape.setFillColor(sf::Color::Blue);
            label.setString("S");
            break;
        default:
            shape.setFillColor(sf::Color::White);
    }
    
    label.setFont(font);
    label.setCharacterSize(20);
    label.setFillColor(sf::Color::White);
    
    info.setFont(font);
    info.setCharacterSize(12);
    info.setFillColor(sf::Color::White);
    updateInfo();
}

void NodeShape::setPosition(float x, float y) {
    shape.setPosition(x, y);
    
    // Center the label in the node
    sf::FloatRect labelBounds = label.getLocalBounds();
    label.setPosition(x - labelBounds.width/2, y - labelBounds.height/2);
    
    // Position info text below the node
    sf::FloatRect infoBounds = info.getLocalBounds();
    info.setPosition(x - infoBounds.width/2, y + shape.getRadius() + 5);
}

bool NodeShape::contains(sf::Vector2f point) {
    return shape.getGlobalBounds().contains(point);
}

void NodeShape::updateInfo() {
    info.setString("MAC: " + mac + "\nPublic IP: " + publicIP + "\nPrivate IP: " + privateIP);
}

EdgeShape::EdgeShape(NodeShape* src, NodeShape* dest) : source(src), destination(dest) {
    line.setPrimitiveType(sf::Lines);
    line.resize(2);
    line[0].color = sf::Color::White;
    line[1].color = sf::Color::White;
    update();
}

void EdgeShape::update() {
    line[0].position = source->shape.getPosition();
    line[1].position = destination->shape.getPosition();
}

PacketAnimation::PacketAnimation() : progress(0.f), currentPathIndex(0), isActive(false) {
    shape.setRadius(5.f);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(5.f, 5.f);
}

void PacketAnimation::update(float deltaTime) {
    if (!isActive || path.empty()) return;
    
    progress += deltaTime * 0.5f; // Adjust speed here
    
    if (progress >= 1.0f) {
        progress = 0.f;
        currentPathIndex++;
        
        if (currentPathIndex >= path.size()) {
            isActive = false;
            return;
        }
    }
    
    EdgeShape* edge = path[currentPathIndex];
    sf::Vector2f start = edge->line[0].position;
    sf::Vector2f end = edge->line[1].position;
    sf::Vector2f position = start + (end - start) * progress;
    shape.setPosition(position);
}

NetworkVisualizer::NetworkVisualizer() : window(sf::VideoMode(1024, 768), "Network Visualizer"),
    menuOpen(false), selectedNode(nullptr), draggedNode(nullptr), animationMode(false), sourceNode(nullptr) {
    
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        throw std::runtime_error("Could not load font");
    }
    
    // Initialize menu button
    menuButton.setSize(sf::Vector2f(50, 50));
    menuButton.setPosition(10, 10);
    menuButton.setFillColor(sf::Color::White);
    
    createNodeMenu();
}

void NetworkVisualizer::createNodeMenu() {
    nodeMenu.setSize(sf::Vector2f(200, 300));
    nodeMenu.setFillColor(sf::Color(50, 50, 50));
    nodeMenu.setPosition(-200, 0); // Hidden by default
    
    // Create node type buttons
    std::vector<std::string> types = {"PC", "Router", "Switch"};
    for (size_t i = 0; i < types.size(); i++) {
        sf::RectangleShape button(sf::Vector2f(180, 50));
        button.setPosition(10, 10 + i * 60);
        button.setFillColor(sf::Color::White);
        nodeButtons.push_back(button);
        
        sf::Text label;
        label.setFont(font);
        label.setString(types[i]);
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::Black);
        nodeLabels.push_back(label);
    }
}

void NetworkVisualizer::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        handleEvents();
        
        float deltaTime = clock.restart().asSeconds();
        packetAnimation.update(deltaTime);
        
        update();
        draw();
    }
}

void NetworkVisualizer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
            
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                
            if (menuButton.getGlobalBounds().contains(mousePos)) {
                menuOpen = !menuOpen;
                nodeMenu.setPosition(menuOpen ? 0 : -200, 0);
            }
            
            // Handle node menu clicks
            if (menuOpen) {
                for (size_t i = 0; i < nodeButtons.size(); i++) {
                    if (nodeButtons[i].getGlobalBounds().contains(mousePos)) {
                        Device_Types type;
                        switch(i) {
                            case 0: type = Device_Types::PC; break;
                            case 1: type = Device_Types::ROUTER; break;
                            case 2: type = Device_Types::SWITCH; break;
                            default: type = Device_Types::NONE;
                        }
                        createNode(type, mousePos.x, mousePos.y);
                    }
                }
            }
            
            // Handle node selection/dragging
            for (auto& node : nodes) {
                if (node->contains(mousePos)) {
                    if (animationMode) {
                        if (!sourceNode) {
                            sourceNode = node.get();
                        } else {
                            auto path = findPath(sourceNode, node.get());
                            if (!path.empty()) {
                                packetAnimation.start(path);
                            }
                            sourceNode = nullptr;
                        }
                    } else {
                        draggedNode = node.get();
                        draggedNode->isDragging = true;
                        draggedNode->dragOffset = draggedNode->shape.getPosition() - mousePos;
                    }
                    break;
                }
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            if (draggedNode) {
                draggedNode->isDragging = false;
                draggedNode = nullptr;
            }
        }
        
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            
            if (draggedNode && draggedNode->isDragging) {
                draggedNode->setPosition(
                    mousePos.x + draggedNode->dragOffset.x,
                    mousePos.y + draggedNode->dragOffset.y
                );
            }
        }
    }
}

void NetworkVisualizer::update() {
    // Update edge positions
    for (auto& edge : edges) {
        edge->update();
    }
}

void NetworkVisualizer::draw() {
    window.clear(sf::Color::Black);
    
    // Draw edges
    for (auto& edge : edges) {
        window.draw(edge->line);
    }
    
    // Draw nodes
    for (auto& node : nodes) {
        window.draw(node->shape);
        window.draw(node->label);
        window.draw(node->info);
    }
    
    // Draw UI elements
    window.draw(menuButton);
    
    if (menuOpen) {
        window.draw(nodeMenu);
        for (size_t i = 0; i < nodeButtons.size(); i++) {
            window.draw(nodeButtons[i]);
            window.draw(nodeLabels[i]);
        }
    }
    
    // Draw packet animation
    if (packetAnimation.isActive) {
        window.draw(packetAnimation.shape);
    }
    
    window.display();
}

std::string generateRandomMAC() {
    std::string mac = "";
    for (int i = 0; i < 6; i++) {
        if (i != 0) mac += ":";
        mac += std::to_string(rand() % 256);
    }
    return mac;
}

NodeShape* NetworkVisualizer::createNode(Device_Types type, float x, float y) {
    std::string mac = generateRandomMAC(); // Your existing function
    std::string publicIP = ipGenerator.generateUniqueIP("public");
    std::string privateIP = ipGenerator.generateUniqueIP("local", "192.168.1.0", "255.255.255.0");
    
    auto node = std::make_unique<NodeShape>(type, font, mac, publicIP, privateIP);
    node->setPosition(x, y);
    
    NodeShape* nodePtr = node.get();
    nodes.push_back(std::move(node));
    return nodePtr;
}

void NetworkVisualizer::tryCreateEdge(NodeShape* source, NodeShape* destination) {
    // Check if edge already exists
    for (auto& edge : edges) {
        if ((edge->source == source && edge->destination == destination) ||
            (edge->source == destination && edge->destination == source)) {
            return;
        }
    }
    
    edges.push_back(std::make_unique<EdgeShape>(source, destination));
}

std::vector<EdgeShape*> NetworkVisualizer::findPath(NodeShape* source, NodeShape* destination) {
    // Simple implementation - just return direct edge if it exists
    for (auto& edge : edges) {
        if ((edge->source == source && edge->destination == destination) ||
            (edge->source == destination && edge->destination == source)) {
            return {edge.get()};
        }
    }
    return {};
}
