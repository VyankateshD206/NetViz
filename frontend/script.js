// D3.js Configuration
const D3Config = {
    simulation: {
        strength: -300,
        distance: 100,
        collideRadius: 50,
        alphaDecay: 0.05,
        velocityDecay: 0.4
    },
    zoom: {
        min: 0.1,
        max: 4,
        defaultScale: 0.8
    },
    node: {
        colors: {
            router: '#ff6b6b',
            switch: '#4ecdc4',
            device: '#45b7d1',
            default: '#69b3a2'
        },
        sizes: {
            router: 25,
            switch: 20,
            device: 15,
            default: 10
        },
        symbols: {
            router: '🌐',
            switch: '⚡',
            device: '💻',
            default: '●'
        }
    },
    link: {
        color: '#999',
        opacity: 0.6,
        width: 2,
        highlightWidth: 3
    }
};

async function fetchNetworkData() {
    try {
        const response = await fetch('http://localhost:9080/api/network');
        const data = await response.json();
        console.log('Fetched data:', data);
        return data;
    } catch (error) {
        console.error('Error fetching data:', error);
        return { nodes: [], edges: [] };
    }
}
// Add this function for testing
function getTestData() {
    return {
        nodes: [
            { id: "1", label: "Router 1", device_type: "router", properties: { public_ip: "192.168.1.1", private_ip: "10.0.0.1", mac: "00:11:22:33:44:55" } },
            { id: "2", label: "Switch 1", device_type: "switch", properties: { public_ip: "192.168.1.2", private_ip: "10.0.0.2", mac: "00:11:22:33:44:56" } },
            { id: "3", label: "Device 1", device_type: "device", properties: { public_ip: "192.168.1.3", private_ip: "10.0.0.3", mac: "00:11:22:33:44:57" } }
        ],
        edges: [
            { source: "1", target: "2" },
            { source: "2", target: "3" }
        ]
    };
}

function createSimulation(nodes, edges, width, height) {
    return d3.forceSimulation(nodes)
        .force("link", d3.forceLink(edges)
            .id(d => d.id)
            .distance(D3Config.simulation.distance))
        .force("charge", d3.forceManyBody()
            .strength(D3Config.simulation.strength))
        .force("center", d3.forceCenter(width / 2, height / 2))
        .force("collision", d3.forceCollide()
            .radius(D3Config.simulation.collideRadius))
        .alphaDecay(D3Config.simulation.alphaDecay)
        .velocityDecay(D3Config.simulation.velocityDecay);
}

function createZoom(svg, g) {
    return d3.zoom()
        .scaleExtent([D3Config.zoom.min, D3Config.zoom.max])
        .on("zoom", (event) => {
            g.attr("transform", event.transform);
        });
}

function renderGraph(data) {
    // Clear existing SVG
    d3.select("#graph svg").remove();

    // Set up SVG dimensions
    const width = document.getElementById('graph').clientWidth;
    const height = document.getElementById('graph').clientHeight;

    // Create SVG container
    const svg = d3.select("#graph")
        .append("svg")
        .attr("width", width)
        .attr("height", height)
        .attr("viewBox", [0, 0, width, height]);

    // Create main group for zoom/pan
    const g = svg.append("g");

    // Add zoom behavior
    const zoom = createZoom(svg, g);
    svg.call(zoom);
    
    // Initial zoom transform
    svg.call(zoom.transform, d3.zoomIdentity.scale(D3Config.zoom.defaultScale));

    // Create arrow marker for directed edges
    svg.append("defs").selectAll("marker")
        .data(["end"])
        .join("marker")
        .attr("id", "arrow")
        .attr("viewBox", "0 -5 10 10")
        .attr("refX", 15)
        .attr("refY", 0)
        .attr("markerWidth", 6)
        .attr("markerHeight", 6)
        .attr("orient", "auto")
        .append("path")
        .attr("fill", D3Config.link.color)
        .attr("d", "M0,-5L10,0L0,5");

    // Create links
    const link = g.append("g")
        .attr("class", "links")
        .selectAll("line")
        .data(data.edges)
        .join("line")
        .attr("stroke", D3Config.link.color)
        .attr("stroke-opacity", D3Config.link.opacity)
        .attr("stroke-width", D3Config.link.width)
        .attr("marker-end", "url(#arrow)");

    // Create node containers
    const node = g.append("g")
        .attr("class", "nodes")
        .selectAll("g")
        .data(data.nodes)
        .join("g")
        .attr("class", d => `node ${d.device_type}`);

    // Add circles for nodes
    node.append("circle")
        .attr("r", d => D3Config.node.sizes[d.device_type] || D3Config.node.sizes.default)
        .attr("fill", d => D3Config.node.colors[d.device_type] || D3Config.node.colors.default)
        .attr("stroke", "#fff")
        .attr("stroke-width", 2);

    // Add device symbols
    node.append("text")
        .attr("class", "device-symbol")
        .attr("text-anchor", "middle")
        .attr("dominant-baseline", "central")
        .text(d => D3Config.node.symbols[d.device_type] || D3Config.node.symbols.default);

    // Add labels
    node.append("text")
        .attr("class", "label")
        .attr("dx", 20)
        .attr("dy", ".35em")
        .text(d => d.label);

    // Add tooltips
    node.append("title")
        .text(d => createTooltipText(d));

    // Create simulation
    const simulation = createSimulation(data.nodes, data.edges, width, height);

    // Add drag behavior
    node.call(d3.drag()
        .on("start", dragstarted)
        .on("drag", dragged)
        .on("end", dragended));

    // Update positions on simulation tick
    simulation.on("tick", () => {
        link
            .attr("x1", d => d.source.x)
            .attr("y1", d => d.source.y)
            .attr("x2", d => d.target.x)
            .attr("y2", d => d.target.y);

        node.attr("transform", d => `translate(${d.x},${d.y})`);
    });

    // Drag functions
    function dragstarted(event, d) {
        if (!event.active) simulation.alphaTarget(0.3).restart();
        d.fx = d.x;
        d.fy = d.y;
    }

    function dragged(event, d) {
        d.fx = event.x;
        d.fy = event.y;
    }

    function dragended(event, d) {
        if (!event.active) simulation.alphaTarget(0);
        d.fx = null;
        d.fy = null;
    }

    // Add double-click behavior for zoom reset
    svg.on("dblclick.zoom", () => {
        svg.transition()
            .duration(750)
            .call(zoom.transform, d3.zoomIdentity.scale(D3Config.zoom.defaultScale));
    });

    // Add context menu to nodes
    node.on('contextmenu', (event, d) => {
        showContextMenu(d, event);
    });

    // Update node click handling
    node.on('click', (event, d) => {
        if (isLinkMode) {
            if (selectedNode === null) {
                // First node selection
                selectedNode = d;
                d3.select(event.currentTarget)
                    .select('circle')
                    .style('stroke', '#0d6efd')
                    .style('stroke-width', '3px');
            } else if (selectedNode.id !== d.id) {
                // Second node selection - create link
                addNewEdge(selectedNode.id, d.id);
                // Reset selection
                d3.selectAll('circle')
                    .style('stroke', '#fff')
                    .style('stroke-width', '2px');
                selectedNode = null;
            }
        }
    });
}

function createTooltipText(d) {
    let tooltip = `${d.label}\n`;
    tooltip += `Type: ${d.device_type}\n`;
    if (d.properties) {
        tooltip += `Public IP: ${d.properties.public_ip || 'N/A'}\n`;
        tooltip += `Private IP: ${d.properties.private_ip || 'N/A'}\n`;
        tooltip += `MAC: ${d.properties.mac || 'N/A'}\n`;
        if (d.properties.ports) {
            tooltip += `Ports: ${d.properties.ports}\n`;
        }
    }
    return tooltip;
}

async function main() {
    initializeModeSelector();
    const data = await fetchNetworkData();
    if (data && data.nodes && data.edges) {
        renderGraph(data);
        
        // Add window resize handler with debounce
        let resizeTimeout;
        window.addEventListener('resize', () => {
            clearTimeout(resizeTimeout);
            resizeTimeout = setTimeout(() => renderGraph(data), 250);
        });
    } else {
        console.error('Invalid or no data received');
        document.getElementById('graph').innerHTML = 
            '<div class="error-message">No network data available</div>';
    }
}

// Start the application
main();

async function addNewNode(deviceType, x, y) {
    try {
        const response = await fetch('http://localhost:9080/api/nodes', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                device_type: deviceType,
                position: {
                    x: x || window.innerWidth/2,
                    y: y || window.innerHeight/2
                }
            })
        });

        if (!response.ok) {
            throw new Error('Failed to add node');
        }

        const result = await response.json();
        // Refresh the network data
        const data = await fetchNetworkData();
        renderGraph(data);
        return result.id;
    } catch (error) {
        console.error('Error adding node:', error);
        showError('Failed to add node');
        return null;
    }
}

async function addNewEdge(sourceId, targetId) {
    try {
        const response = await fetch('http://localhost:9080/api/edges', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                source: sourceId,
                target: targetId
            })
        });

        if (!response.ok) {
            throw new Error('Failed to add edge');
        }

        // Refresh the network data
        const data = await fetchNetworkData();
        renderGraph(data);
    } catch (error) {
        console.error('Error adding edge:', error);
        showError('Failed to add edge');
    }
}

async function deleteNode(nodeId) {
    try {
        const response = await fetch(`http://localhost:9080/api/nodes/${nodeId}`, {
            method: 'DELETE'
        });

        if (!response.ok) {
            throw new Error('Failed to delete node');
        }

        // Refresh the network data
        const data = await fetchNetworkData();
        renderGraph(data);
    } catch (error) {
        console.error('Error deleting node:', error);
        showError('Failed to delete node');
    }
}

// Add context menu functionality
function showContextMenu(d, event) {
    event.preventDefault();
    
    // Remove any existing context menu
    d3.select('.context-menu').remove();
    
    const contextMenu = d3.select('body')
        .append('div')
        .attr('class', 'context-menu')
        .style('left', `${event.pageX}px`)
        .style('top', `${event.pageY}px`);
    
    // Add menu items
    contextMenu.append('div')
        .text('Delete Node')
        .on('click', () => {
            deleteNode(d.id);
            contextMenu.remove();
        });

    contextMenu.append('div')
        .text('Delete Connected Links')
        .on('click', () => {
            deleteConnectedLinks(d.id);
            contextMenu.remove();
        });
    
    // Close menu when clicking outside
    d3.select('body').on('click.context-menu', () => {
        contextMenu.remove();
    });
}

// Add function to delete connected links
async function deleteConnectedLinks(nodeId) {
    try {
        const response = await fetch(`http://localhost:9080/api/nodes/${nodeId}/links`, {
            method: 'DELETE'
        });

        if (!response.ok) {
            throw new Error('Failed to delete links');
        }

        const data = await fetchNetworkData();
        renderGraph(data);
    } catch (error) {
        console.error('Error deleting links:', error);
        showError('Failed to delete links');
    }
}

// Add these variables at the top of your file
let isLinkMode = false;
let selectedNode = null;

// Add this function to handle mode switching
function initializeModeSelector() {
    const addNodeModeBtn = document.getElementById('addNodeMode');
    const addLinkModeBtn = document.getElementById('addLinkMode');
    const nodeButtons = document.getElementById('nodeButtons');

    addNodeModeBtn.addEventListener('click', () => {
        isLinkMode = false;
        selectedNode = null;
        addNodeModeBtn.classList.add('active');
        addLinkModeBtn.classList.remove('active');
        nodeButtons.style.display = 'flex';
    });

    addLinkModeBtn.addEventListener('click', () => {
        isLinkMode = true;
        selectedNode = null;
        addLinkModeBtn.classList.add('active');
        addNodeModeBtn.classList.remove('active');
        nodeButtons.style.display = 'none';
    });
}
