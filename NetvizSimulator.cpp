#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// [Previous utility functions remain the same]
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
    bool isPartOfPath;
    bool isReversed;  // New field to track if edge is being animated in reverse
    
    Edge(Node* s, Node* e) : start(s), end(e), animationProgress(0.f), 
                            isAnimating(false), isPartOfPath(false), isReversed(false) {
        updatePosition();
        packet.setRadius(5.f);
        packet.setFillColor(sf::Color::Red);
        line.setFillColor(sf::Color::Black);
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
        line.setFillColor(isPartOfPath ? sf::Color::Green : sf::Color::Black);
        
        float angle = std::atan2(endPos.y - startPos.y, endPos.x - startPos.x) * 180 / 3.14159f;
        line.setRotation(angle);
    }

    void updatePacketPosition() {
        if (isAnimating) {
            sf::Vector2f actualStart, actualEnd;
            if (!isReversed) {
                actualStart = start->position + sf::Vector2f(start->shape.getRadius(), start->shape.getRadius());
                actualEnd = end->position + sf::Vector2f(end->shape.getRadius(), end->shape.getRadius());
            } else {
                actualStart = end->position + sf::Vector2f(end->shape.getRadius(), end->shape.getRadius());
                actualEnd = start->position + sf::Vector2f(start->shape.getRadius(), start->shape.getRadius());
            }
            
            float x = actualStart.x + (actualEnd.x - actualStart.x) * animationProgress;
            float y = actualStart.y + (actualEnd.y - actualStart.y) * animationProgress;
            
            packet.setPosition(x - packet.getRadius(), y - packet.getRadius());
        }
    }
};

// Modified to return path with correct edge directions
std::vector<std::pair<Edge*, bool>> findPath(Node* start, Node* end, std::vector<Edge>& edges) {
    std::unordered_map<Node*, Node*> previousNode;
    std::unordered_set<Node*> visited;
    std::queue<Node*> queue;

    queue.push(start);
    visited.insert(start);

    while (!queue.empty()) {
        Node* current = queue.front();
        queue.pop();

        if (current == end) {
            // Path found, reconstruct it with direction information
            std::vector<std::pair<Edge*, bool>> path;
            Node* node = end;
            while (node != start) {
                Node* prev = previousNode[node];
                // Find the edge between prev and node
                for (auto& edge : edges) {
                    if (edge.start == prev && edge.end == node) {
                        // Forward direction
                        path.insert(path.begin(), {&edge, false});
                        break;
                    } else if (edge.end == prev && edge.start == node) {
                        // Reverse direction
                        path.insert(path.begin(), {&edge, true});
                        break;
                    }
                }
                node = prev;
            }
            return path;
        }

        // Find all connected nodes
        for (auto& edge : edges) {
            Node* next = nullptr;
            if (edge.start == current) next = edge.end;
            else if (edge.end == current) next = edge.start;

            if (next && visited.find(next) == visited.end()) {
                queue.push(next);
                visited.insert(next);
                previousNode[next] = current;
            }
        }
    }

    return std::vector<std::pair<Edge*, bool>>();  // No path found
}

// [MainMenu class remains the same]
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

    // [Previous initialization code remains the same]
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
    std::vector<std::pair<Edge*, bool>> currentPath;
    size_t currentEdgeIndex = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

                // [Previous menu and draggable icons code remains the same]
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
                // Modified node selection for animation
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
                                    
                                    // Reset previous path
                                    for (auto& edge : edges) {
                                        edge.isAnimating = false;
                                        edge.isPartOfPath = false;
                                        edge.isReversed = false;
                                        edge.updatePosition();
                                    }

                                    // Find path between nodes
                                    currentPath = findPath(sourceNode, destNode, edges);
                                    currentEdgeIndex = 0;
                                    
                                    if (!currentPath.empty()) {
                                        // Mark all edges in path
                                        for (auto& [edge, isReversed] : currentPath) {
                                            edge->isPartOfPath = true;
                                            edge->isReversed = isReversed;
                                            edge->updatePosition();
                                        }
                                        // Start animation with first edge
                                        currentPath[0].first->isAnimating = true;
                                        currentPath[0].first->animationProgress = 0.f;
                                    } else {
                                        // No path found, reset selection
                                        sourceNode->shape.setOutlineColor(sf::Color::Black);
                                        destNode->shape.setOutlineColor(sf::Color::Black);
                                        sourceNode = nullptr;
                                        destNode = nullptr;
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
                            edge.isPartOfPath = false;
                            edge.isReversed = false;
                            edge.updatePosition();
                        }
                        if (sourceNode) sourceNode->shape.setOutlineColor(sf::Color::Black);
                        if (destNode) destNode->shape.setOutlineColor(sf::Color::Black);
                        sourceNode = nullptr;
                        destNode = nullptr;
                        currentPath.clear();
                        currentEdgeIndex = 0;
                    }
                }
            }

            // [Previous event handling code remains the same]
            // [Previous event handling code remains the same]
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

        // Update packet animation with sequential path handling
        if (!currentPath.empty() && currentEdgeIndex < currentPath.size()) {
            Edge* currentEdge = currentPath[currentEdgeIndex].first;
            if (currentEdge->isAnimating) {
                currentEdge->animationProgress += 0.02f;
                if (currentEdge->animationProgress >= 1.f) {
                    // Current edge animation complete
                    currentEdge->animationProgress = 1.f;
                    currentEdge->isAnimating = false;
                    
                    // Move to next edge if available
                    currentEdgeIndex++;
                    if (currentEdgeIndex < currentPath.size()) {
                        // Start animation of next edge
                        auto [nextEdge, isReversed] = currentPath[currentEdgeIndex];
                        nextEdge->isAnimating = true;
                        nextEdge->isReversed = isReversed;
                        nextEdge->animationProgress = 0.f;
                    } else {
                        // Path complete, reset to start
                        currentEdgeIndex = 0;
                        for (auto& [edge, isReversed] : currentPath) {
                            edge->animationProgress = 0.f;
                            edge->isReversed = isReversed;
                        }
                        currentPath[0].first->isAnimating = true;
                    }
                }
                currentEdge->updatePacketPosition();
            }
        }

        // [Rest of rendering code remains the same]
        window.clear(sf::Color::White);

        // [Rest of rendering code remains the same]

        if (!mainMenu.isActive) {
            // Draw edges first
            for (const auto& edge : edges) {
                window.draw(edge.line);
                if (edge.isAnimating) {
                    window.draw(edge.packet);
                }
            }

            // Draw nodes and other elements
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

            window.draw(animationButton);
            window.draw(animationButtonText);
        } else {
            window.draw(mainMenu.startButton);
            window.draw(mainMenu.buttonText);
        }

        window.display();
    }

    return 0;
}