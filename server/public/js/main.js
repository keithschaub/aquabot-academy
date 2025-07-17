// WebSocket connection
let ws = null;
let selectedBoatId = null;
let joystick = null;
let raceTimer = null;
let raceInterval = null;

// Connect to WebSocket server
function connect() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}`;
    
    ws = new WebSocket(wsUrl);
    
    ws.onopen = () => {
        console.log('Connected to server');
        // Register as a controller
        ws.send(JSON.stringify({
            type: 'register',
            controllerId: generateControllerId()
        }));
        
        document.getElementById('connectionStatus').textContent = 'Connected';
        document.getElementById('connectionStatus').classList.add('connected');
        document.getElementById('connectionStatus').classList.remove('disconnected');
    };
    
    ws.onclose = () => {
        console.log('Disconnected from server');
        document.getElementById('connectionStatus').textContent = 'Disconnected';
        document.getElementById('connectionStatus').classList.add('disconnected');
        document.getElementById('connectionStatus').classList.remove('connected');
        
        // Attempt to reconnect
        setTimeout(connect, 1000);
    };
    
    ws.onmessage = (event) => {
        const data = JSON.parse(event.data);
        handleServerMessage(data);
    };
}

// Generate a unique controller ID
function generateControllerId() {
    return 'controller_' + Math.random().toString(36).substr(2, 9);
}

// Handle incoming server messages
function handleServerMessage(data) {
    switch(data.type) {
        case 'boatStatus':
            updateBoatList(data);
            break;
            
        case 'telemetry':
            if (data.boatId === selectedBoatId) {
                updateTelemetry(data);
            }
            break;
            
        case 'raceStatus':
            handleRaceStatus(data);
            break;
    }
}

// Update boat selection dropdown
function updateBoatList(data) {
    const select = document.getElementById('boatSelect');
    
    if (data.status === 'connected') {
        // Add new boat if not exists
        if (!Array.from(select.options).some(opt => opt.value === data.boatId.toString())) {
            const option = document.createElement('option');
            option.value = data.boatId;
            option.textContent = `Boat ${data.boatId}`;
            select.appendChild(option);
        }
    } else {
        // Remove disconnected boat
        Array.from(select.options).forEach(opt => {
            if (opt.value === data.boatId.toString()) {
                select.removeChild(opt);
            }
        });
    }
}

// Update telemetry display
function updateTelemetry(data) {
    document.getElementById('speedValue').textContent = data.speed;
    document.getElementById('directionValue').textContent = getDirectionText(data.angle);
    
    const batterySpan = document.getElementById('batteryValue');
    batterySpan.textContent = data.battery.toFixed(1);
    
    // Update battery status colors
    batterySpan.className = '';
    if (data.battery < 3.3) {
        batterySpan.classList.add('battery-low');
    } else if (data.battery < 3.7) {
        batterySpan.classList.add('battery-medium');
    } else {
        batterySpan.classList.add('battery-good');
    }
}

// Convert angle to text direction
function getDirectionText(angle) {
    if (angle < 60) return 'Left';
    if (angle > 120) return 'Right';
    return 'Center';
}

// Initialize joystick control
function initJoystick() {
    const options = {
        zone: document.getElementById('joystickArea'),
        mode: 'static',
        position: { left: '50%', top: '50%' },
        color: 'blue',
        size: 150
    };
    
    joystick = nipplejs.create(options);
    
    joystick.on('move', (evt, data) => {
        if (!selectedBoatId) return;
        
        // Convert joystick data to boat controls
        const angle = ((data.angle.degree + 90) % 360) * (180/360);
        const speed = Math.min(100, (data.distance * 2));
        
        sendControl(speed, angle);
    });
    
    joystick.on('end', () => {
        if (!selectedBoatId) return;
        sendControl(0, 90);  // Center position, stop
    });
}

// Send control command to server
function sendControl(speed, angle) {
    if (!ws || ws.readyState !== WebSocket.OPEN) return;
    
    ws.send(JSON.stringify({
        type: 'control',
        boatId: selectedBoatId,
        speed: speed,
        angle: angle
    }));
}

// Handle race status updates
function handleRaceStatus(data) {
    const startButton = document.getElementById('startRace');
    const endButton = document.getElementById('endRace');
    const timer = document.getElementById('raceTimer');
    
    if (data.status === 'started') {
        startButton.disabled = true;
        endButton.disabled = false;
        
        // Start race timer
        const endTime = data.startTime + data.duration;
        
        if (raceInterval) clearInterval(raceInterval);
        raceInterval = setInterval(() => {
            const remaining = Math.max(0, endTime - Date.now());
            const minutes = Math.floor(remaining / 60000);
            const seconds = Math.floor((remaining % 60000) / 1000);
            timer.textContent = `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
            
            if (remaining <= 0) {
                clearInterval(raceInterval);
                raceInterval = null;
            }
        }, 100);
    } else if (data.status === 'ended') {
        startButton.disabled = false;
        endButton.disabled = true;
        if (raceInterval) {
            clearInterval(raceInterval);
            raceInterval = null;
        }
        timer.textContent = '00:00';
    }
}

// Initialize the application
document.addEventListener('DOMContentLoaded', () => {
    // Connect to WebSocket server
    connect();
    
    // Initialize joystick
    initJoystick();
    
    // Setup boat selection handler
    document.getElementById('boatSelect').addEventListener('change', (e) => {
        selectedBoatId = parseInt(e.target.value);
    });
    
    // Setup emergency stop button
    document.getElementById('emergencyStop').addEventListener('click', () => {
        if (!selectedBoatId) return;
        
        ws.send(JSON.stringify({
            type: 'emergency',
            boatId: selectedBoatId
        }));
    });
    
    // Setup race control buttons
    document.getElementById('startRace').addEventListener('click', () => {
        ws.send(JSON.stringify({
            type: 'startRace',
            duration: 180000  // 3 minutes
        }));
    });
    
    document.getElementById('endRace').addEventListener('click', () => {
        ws.send(JSON.stringify({
            type: 'emergency'  // Emergency stop all boats
        }));
    });
}); 