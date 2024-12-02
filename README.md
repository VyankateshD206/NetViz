# NetViz: Computer Network Visualizer

The **NetViz** is a C++ program designed to simulate the working of computer networks. It allows users to create their own custom network topologies using drag-and-drop components. The tool provides a visual representation of how data packets are transferred across a network, enabling users to explore the dynamics of computer networks in an interactive and engaging way.<br>

![NetViz](https://github.com/user-attachments/assets/cb559dfa-3899-4d27-941b-5d79b969ad42)
![NetViz GIF](https://github.com/user-attachments/assets/340bd134-60bf-4266-b952-00e5d77ddb1e)





## Features
- **Custom Network Topologies**: Create custom networks using drag-and-drop components.
- **Packet Transfer Simulation**: Visualize how data packets move through a network. Used TCP/IP Protocol for transfer of Data Packets.
- **Interactive Interface**: Engage with your network design in real-time.
- **Network Algorithms**: Implement and visualize algorithms like A* for pathfinding, Kruskal's for minimum spanning trees, and more.
- **DHCP**: Implemented Dynamic Host Configuration Protocol (DHCP), that automatically assigns IP addresses and other configuration parameters to devices on a network.

## Data Structures
- **Graphs**: Used for representing networks, where edges represent connections and nodes serve as endpoints.
- **Priority Queues**: Utilized for Packet Transfer.
- **Queues**: Utilized for packet broadcasting.
- **Stacks**: For loop detection in network paths.
- **HashMaps**: Employed for collision avoidance in network communication.
- **Vectors**: Used for Storing Data of nodes, edges, ip_addresses, mac_address, ports, etc.

## Algorithms
- **A\***: Finds the shortest path in a network.
- **Kruskal's Algorithm**: Used for finding the minimum spanning tree in the network.

## Installation

1. Clone the repository to your local machine:
   ```bash
   git clone https://github.com/VyankateshD206/NetViz.git
   
2. Pistache Installation for Backend:
   [Pistache Website](https://pistacheio.github.io/pistache/docs/)

3. Run these commands:
   ```bash
   meson setup builddir
   cd builddir
   meson compile
   ./network_server

4. Start the backend Server on browser:
   ```bash
   http://localhost:9080/api/network 

5. Start the Frontend:
   ```bash
   cd frontend
   python -m http.server 8080
  - Go to site : http://localhost:8080/
