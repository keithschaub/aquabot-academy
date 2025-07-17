const net = require('net');

const server = net.createServer((socket) => {
    console.log('Client connected from:', socket.remoteAddress);
    
    socket.on('data', (data) => {
        console.log('Received:', data.toString());
        socket.write('Server received: ' + data.toString());
    });
    
    socket.on('error', (err) => {
        console.log('Socket error:', err);
    });
    
    socket.on('close', () => {
        console.log('Client disconnected');
    });
});

server.listen(8080, '0.0.0.0', () => {
    console.log('Test server listening on port 8080');
    console.log('Server IP addresses:');
    const { networkInterfaces } = require('os');
    const nets = networkInterfaces();
    for (const name of Object.keys(nets)) {
        for (const net of nets[name]) {
            if (net.family === 'IPv4' && !net.internal) {
                console.log(`  - ${name}: ${net.address}`);
            }
        }
    }
}); 