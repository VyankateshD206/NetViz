#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>

// Utility function to generate MAC address
std::string generateMAC() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 255);
    
    std::stringstream ss;
    for (int i = 0; i < 6; ++i) {
        if (i > 0) ss << ":";
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }
    return ss.str();
}

// Utility function to generate IP address
std::string generateIP(bool isPublic) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, 254);
    
    std::stringstream ss;
    if (isPublic) {
        ss << dis(gen) << "." << dis(gen) << "." << dis(gen) << "." << dis(gen);
    } else {
        ss << "192.168.1." << dis(gen);
    }
    return ss.str();
}

class Node {
public:
    sf::CircleShape shape;
    sf::Text label;
    sf::Text infoText;  // For displaying IP and MAC
    std::string type;
    std::string ip;
    std::string mac;
    sf::Vector2f position;
    bool isSelected = false;

    Node(const sf::Font& font, const std::string& nodeType, float x, float y) {
        type = nodeType;
        shape.setRadius(30.f);
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::Black);
        
        label.setFont(font);
        label.setString(nodeType[0]);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::White);
        
        // Center the label
        float labelX = x + shape.getRadius() - label.getGlobalBounds().width / 2;
        float labelY = y + shape.getRadius() - label.getGlobalBounds().height / 2;
        label.setPosition(labelX, labelY);
        
        position = sf::Vector2f(x, y);
        ip = generateIP(type == "Router");
        mac = generateMAC();

        // Setup info text
        infoText.setFont(font);
        infoText.setCharacterSize(12);
        infoText.setFillColor(sf::Color::Black);
        updateInfoText();
    }

    void updateInfoText() {
        std::string info = "IP: " + ip + "\nMAC: " + mac;
        infoText.setString(info);
        float textX = position.x - 50;
        float textY = position.y + shape.getRadius() * 2 + 5;
        infoText.setPosition(textX, textY);
    }

    bool contains(sf::Vector2f point) {
        return shape.getGlobalBounds().contains(point);
    }

    void setPosition(float x, float y) {
        position = sf::Vector2f(x, y);
        shape.setPosition(x, y);
        float labelX = x + shape.getRadius() - label.getGlobalBounds().width / 2;
        float labelY = y + shape.getRadius() - label.getGlobalBounds().height / 2;
        label.setPosition(labelX, labelY);
        updateInfoText();
    }
};

class DraggableIcon {
public:
    sf::CircleShape shape;
    sf::Text label;
    std::string type;

    DraggableIcon(const sf::Font& font, const std::string& nodeType, float x, float y) {
        type = nodeType;
        shape.setRadius(20.f);
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::Black);

        label.setFont(font);
        label.setString(nodeType[0]);
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::White);
        
        float labelX = x + shape.getRadius() - label.getGlobalBounds().width / 2;
        float labelY = y + shape.getRadius() - label.getGlobalBounds().height / 2;
        label.setPosition(labelX, labelY);
    }

    bool contains(sf::Vector2f point) const {  // Make this function const
        return shape.getGlobalBounds().contains(point);
    }
};

class Edge {
public:
    Node* start;
    Node* end;
    sf::RectangleShape line;
    sf::CircleShape packet;
    float animationProgress;
    bool isAnimating;

    Edge(Node* s, Node* e) : start(s), end(e), animationProgress(0.f), isAnimating(false) {
        updatePosition();
        packet.setRadius(5.f);
        packet.setFillColor(sf::Color::Red);
    }

    void updatePosition() {
        sf::Vector2f startPos = start->position + sf::Vector2f(start->shape.getRadius(), start->shape.getRadius());
        sf::Vector2f endPos = end->position + sf::Vector2f(end->shape.getRadius(), end->shape.getRadius());
        
        float length = std::sqrt(
            std::pow(endPos.x - startPos.x, 2) + 
            std::pow(endPos.y - startPos.y, 2)
        );
        
        line.setSize(sf::Vector2f(length, 3.f));
        line.setPosition(startPos);
        line.setFillColor(sf::Color::Black);
        
        float angle = std::atan2(endPos.y - startPos.y, endPos.x - startPos.x) * 180 / 3.14159f;
        line.setRotation(angle);
    }

    void updatePacketPosition() {
        if (isAnimating) {
            sf::Vector2f startPos = start->position + sf::Vector2f(start->shape.getRadius(), start->shape.getRadius());
            sf::Vector2f endPos = end->position + sf::Vector2f(end->shape.getRadius(), end->shape.getRadius());
            
            float x = startPos.x + (endPos.x - startPos.x) * animationProgress;
            float y = startPos.y + (endPos.y - startPos.y) * animationProgress;
            
            packet.setPosition(x - packet.getRadius(), y - packet.getRadius());
        }
    }
};

class MainMenu {
public:
    sf::RectangleShape startButton;
    sf::Text buttonText;
    bool isActive;

    MainMenu(const sf::Font& font) : isActive(true) {
        startButton.setSize(sf::Vector2f(200.f, 50.f));
        startButton.setPosition(300.f, 250.f);
        startButton.setFillColor(sf::Color::Blue);
        
        buttonText.setFont(font);
        buttonText.setString("Start Network Visualizer");
        buttonText.setCharacterSize(20);
        buttonText.setFillColor(sf::Color::White);
        
        float textX = startButton.getPosition().x + (startButton.getSize().x - buttonText.getGlobalBounds().width) / 2;
        float textY = startButton.getPosition().y + (startButton.getSize().y - buttonText.getGlobalBounds().height) / 2;
        buttonText.setPosition(textX, textY);
    }

    bool contains(sf::Vector2f point) {
        return startButton.getGlobalBounds().contains(point);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Network Visualizer");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        return -1;
    }

    MainMenu mainMenu(font);
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    
    // Create draggable icons
    std::vector<DraggableIcon> draggableIcons;
    draggableIcons.emplace_back(font, "Router", 10.f, 10.f);
    draggableIcons.emplace_back(font, "PC", 10.f, 60.f);
    draggableIcons.emplace_back(font, "Switch", 10.f, 110.f);

    Node* selectedNode = nullptr;
    Node* sourceNode = nullptr;
    Node* destNode = nullptr;
    bool isDragging = false;
    bool isCreatingNode = false;
    std::string draggingNodeType;
    bool isAnimating = false;
    sf::Vector2f dragOffset;

    // Animation control button
    sf::RectangleShape animationButton(sf::Vector2f(120.f, 30.f));
    animationButton.setPosition(660.f, 10.f);
    animationButton.setFillColor(sf::Color::Blue);
    
    sf::Text animationButtonText;
    animationButtonText.setFont(font);
    animationButtonText.setString("Toggle Packet");
    animationButtonText.setCharacterSize(16);
    animationButtonText.setFillColor(sf::Color::White);
    animationButtonText.setPosition(
        animationButton.getPosition().x + 10.f,
        animationButton.getPosition().y + 5.f
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

                if (mainMenu.isActive) {
                    if (mainMenu.contains(mousePos)) {
                        mainMenu.isActive = false;
                    }
                    continue;
                }

                // Check for draggable icons first
                for (const auto& icon : draggableIcons) {
                    if (icon.contains(mousePos)) {
                        isCreatingNode = true;
                        draggingNodeType = icon.type;
                        nodes.emplace_back(font, icon.type, mousePos.x, mousePos.y);
                        selectedNode = &nodes.back();
                        isDragging = true;
                        dragOffset = mousePos - sf::Vector2f(mousePos.x, mousePos.y);
                        break;
                    }
                }

                // If not creating a new node, check for existing node selection
                if (!isCreatingNode) {
                    for (auto& node : nodes) {
                        if (node.contains(mousePos)) {
                            if (isAnimating) {
                                if (!sourceNode) {
                                    sourceNode = &node;
                                    node.shape.setOutlineColor(sf::Color::Green);
                                } else if (!destNode && &node != sourceNode) {
                                    destNode = &node;
                                    node.shape.setOutlineColor(sf::Color::Red);
                                    // Find or create edge between nodes
                                    bool foundEdge = false;
                                    for (auto& edge : edges) {
                                        if ((edge.start == sourceNode && edge.end == destNode) ||
                                            (edge.start == destNode && edge.end == sourceNode)) {
                                            edge.isAnimating = true;
                                            edge.animationProgress = 0.f;
                                            foundEdge = true;
                                            break;
                                        }
                                    }
                                    if (!foundEdge) {
                                        edges.emplace_back(sourceNode, destNode);
                                        edges.back().isAnimating = true;
                                    }
                                }
                            } else {
                                selectedNode = &node;
                                isDragging = true;
                                dragOffset = mousePos - node.position;
                            }
                            break;
                        }
                    }
                }

                // Check animation button
                if (animationButton.getGlobalBounds().contains(mousePos)) {
                    isAnimating = !isAnimating;
                    if (!isAnimating) {
                        // Reset animation state
                        for (auto& edge : edges) {
                            edge.isAnimating = false;
                        }
                        if (sourceNode) sourceNode->shape.setOutlineColor(sf::Color::Black);
                        if (destNode) destNode->shape.setOutlineColor(sf::Color::Black);
                        sourceNode = nullptr;
                        destNode = nullptr;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (selectedNode != nullptr && !isCreatingNode) {
                    // Check for edge creation
                    for (auto& node : nodes) {
                        if (&node != selectedNode && node.contains(
                            sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                            bool edgeExists = false;
                            for (const auto& edge : edges) {
                                if ((edge.start == selectedNode && edge.end == &node) ||
                                    (edge.start == &node && edge.end == selectedNode)) {
                                    edgeExists = true;
                                    break;
                                }
                            }
                            if (!edgeExists) {
                                edges.emplace_back(selectedNode, &node);
                            }
                            break;
                        }
                    }
                }
                selectedNode = nullptr;
                isDragging = false;
                isCreatingNode = false;
            }

            if (event.type == sf::Event::MouseMoved) {
                if (isDragging && selectedNode != nullptr) {
                    sf::Vector2f newPos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y) - dragOffset;
                    selectedNode->setPosition(newPos.x, newPos.y);
                    
                    // Update connected edges
                    for (auto& edge : edges) {
                        if (edge.start == selectedNode || edge.end == selectedNode) {
                            edge.updatePosition();
                        }
                    }
                }
            }
        }

        // Update packet animation
        for (auto& edge : edges) {
            if (edge.isAnimating) {
                edge.animationProgress += 0.02f;
                if (edge.animationProgress >= 1.f) {
                    edge.animationProgress = 0.f;
                }
                edge.updatePacketPosition();
            }
        }

        window.clear(sf::Color::White);  // White background

        if (mainMenu.isActive) {
            window.draw(mainMenu.startButton);
            window.draw(mainMenu.buttonText);
        } else {
            // Draw edges
            for (const auto& edge : edges) {
                window.draw(edge.line);
                if (edge.isAnimating) {
                    window.draw(edge.packet);
                }
            }

            // Draw nodes
            for (const auto& node : nodes) {
                window.draw(node.shape);
                window.draw(node.label);
                window.draw(node.infoText);
            }

            // Draw draggable icons
            for (const auto& icon : draggableIcons) {
                window.draw(icon.shape);
                window.draw(icon.label);
            }

            // Draw animation control button
            window.draw(animationButton);
            window.draw(animationButtonText);
        }

        window.display();
    }

    return 0;
}
