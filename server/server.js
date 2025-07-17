const net = require('net');
const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');

// Create Express app and HTTP server for web clients
const app = express();
const httpServer = http.createServer(app);
const wss = new WebSocket.Server({ server: httpServer });

// Serve static files
app.use(express.static(path.join(__dirname, 'public')));

// Store connected clients
const boats = new Map();        // boatId -> tcp socket
const controllers = new Map();   // controllerId -> websocket

// Create TCP server for Arduino connections
const tcpServer = net.createServer((socket) => {
    console.log('Arduino connected');
    let messageBuffer = '';
    let boatId = null;

    socket.on('data', (data) => {
        // Add received data to buffer
        messageBuffer += data.toString();
        
        // Process complete messages (split by newlines)
        let newlineIndex;
        while ((newlineIndex = messageBuffer.indexOf('\n')) !== -1) {
            const message = messageBuffer.substring(0, newlineIndex);
            messageBuffer = messageBuffer.substring(newlineIndex + 1);
            
            try {
                const data = JSON.parse(message);
                console.log('Received from Arduino:', data);

                if (data.type === 'register') {
                    boatId = data.boatId;
                    boats.set(boatId, socket);
                    console.log(`Boat ${boatId} registered`);
                    
                    // Notify web clients
                    wss.clients.forEach((client) => {
                        if (client.readyState === WebSocket.OPEN) {
                            client.send(JSON.stringify({
                                type: 'boatStatus',
                                boatId: boatId,
                                status: 'connected'
                            }));
                        }
                    });
                }
                else if (data.type === 'telemetry') {
                    // Forward telemetry to web clients
                    wss.clients.forEach((client) => {
                        if (client.readyState === WebSocket.OPEN) {
                            client.send(JSON.stringify(data));
                        }
                    });
                }
            } catch (error) {
                console.error('Error processing message:', error);
            }
        }
    });

    socket.on('close', () => {
        if (boatId) {
            console.log(`Boat ${boatId} disconnected`);
            boats.delete(boatId);
            
            // Notify web clients
            wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify({
                        type: 'boatStatus',
                        boatId: boatId,
                        status: 'disconnected'
                    }));
                }
            });
        }
    });

    socket.on('error', (error) => {
        console.error('Socket error:', error);
    });
});

// Handle web client connections
wss.on('connection', (ws) => {
    console.log('Web client connected');

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);
            console.log('Received from web:', data);

            if (data.type === 'control') {
                // Forward control commands to specific boat
                const boat = boats.get(data.boatId);
                if (boat) {
                    boat.write(JSON.stringify(data) + '\n');
                }
            }
            else if (data.type === 'emergency') {
                // Send emergency stop to all boats
                boats.forEach((boat) => {
                    boat.write(JSON.stringify({ type: 'emergency' }) + '\n');
                });
            }
        } catch (error) {
            console.error('Error processing message:', error);
        }
    });
});

// Start servers on different ports
const WEB_PORT = 3000;    // Web/WebSocket port
const TCP_PORT = 3001;    // Arduino TCP port

httpServer.listen(WEB_PORT, () => {
    console.log(`Web server running on port ${WEB_PORT}`);
});

tcpServer.listen(TCP_PORT, () => {
    console.log(`TCP server for Arduino running on port ${TCP_PORT}`);
});