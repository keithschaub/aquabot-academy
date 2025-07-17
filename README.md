# RC Boat Control System

This project implements a WiFi-based RC boat control system using Arduino R4 WiFi and web-based controls.

## Project Structure
```
rc_boat/
├── arduino/
│   ├── boat_controller/        # Arduino sketch for boat control
│   └── libraries/              # Required Arduino libraries
├── server/
│   ├── package.json           # Node.js server dependencies
│   ├── server.js              # Main server file
│   └── public/                # Web interface files
│       ├── index.html         # Main control interface
│       ├── css/              
│       └── js/               
└── README.md                  # This file
```

## Quick Start for Single Boat Testing

1. Hardware Setup:
   - Arduino R4 WiFi
   - Motor controller
   - Servos/motors for your boat
   - Battery pack
   - Waterproof housing

2. Software Setup:
   - Install Arduino IDE
   - Install required libraries (see arduino/libraries/README.md)
   - Install Node.js for the server

3. Configuration:
   - Update WiFi credentials in boat_controller.ino
   - Set your boat's static IP in boat_controller.ino
   - Update server IP in config.js

4. Running the System:
   - Upload boat_controller.ino to Arduino R4
   - Start the Node.js server: `npm start`
   - Open web browser to http://localhost:3000

## Testing Your Boat

1. Power up your boat
2. Connect your computer/phone to the same WiFi network
3. Open the control interface
4. Select "Boat 1" from the dropdown
5. Use the on-screen controls to operate your boat

## Scaling to Classroom Use

For classroom deployment with multiple boats:

1. Assign each boat a unique static IP (101-110)
2. Update each Arduino with its specific boat number
3. Connect all devices to the same WiFi network
4. Teacher can access dashboard at /dashboard

## Features

- Real-time boat control via WebSocket
- Low-latency communication
- Battery monitoring
- Emergency stop
- Race management interface
- Position tracking
- Scoreboard

## Troubleshooting

See the [Wiki](wiki/) for common issues and solutions.

## Contributing

Feel free to submit issues and enhancement requests! 

Now that I've created all the necessary files, let me explain how to test this with your single boat:

### Testing Steps

1. **Hardware Setup**
   - Connect your Arduino R4 WiFi to your boat's motors/servos
   - Connect pins as defined in `boat_controller.ino`:
     - Motor control: Pin 9 (PWM)
     - Rudder servo: Pin 10
     - Battery monitoring: Pin A0

2. **Software Setup**
   ```bash
   # Create project directory structure
   mkdir -p rc_boat/arduino/boat_controller
   mkdir -p rc_boat/server/public/{css,js}
   
   # Copy all files to their respective locations
   ```

3. **Arduino Setup**
   - Open Arduino IDE
   - Install required libraries:
     - WiFiS3
     - WebSocketClient
     - ArduinoJson
     - Servo
   - Open `boat_controller.ino`
   - Update WiFi credentials:
     ```cpp
     const char* ssid = "thisistheway";
     const char* password = "eatpie69";
     ```
   - Update server IP to match your computer's IP
   - Upload to Arduino R4

4. **Server Setup**
   ```bash
   # Navigate to server directory
   cd rc_boat/server
   
   # Install Node.js dependencies
   npm install
   
   # Start the server
   npm start
   ```

5. **Testing**
   - Open web browser to `http://localhost:3000`
   - Select "Boat 1" from dropdown
   - Use on-screen joystick to control the boat
   - Monitor telemetry data (speed, direction, battery)
   - Test emergency stop button

### Troubleshooting Tips

1. **Connection Issues**
   - Ensure all devices are on same WiFi network
   - Check Arduino serial monitor for connection status
   - Verify server IP address is correct
   - Check WebSocket connection in browser dev tools

2. **Control Issues**
   - Monitor serial output on Arduino for received commands
   - Verify motor/servo connections
   - Test motor/servo directly with simple Arduino sketch first

3. **Battery Monitoring**
   - Ensure proper voltage divider if battery voltage > 5V
   - Calibrate battery readings if needed

### Next Steps for Classroom Implementation

Once your single boat test is successful, scaling to multiple boats is straightforward:

1. Clone the Arduino code for each boat
2. Update `BOAT_ID` and `staticIP` for each boat
3. Deploy the server on a dedicated computer/laptop
4. Connect all devices to same WiFi network
5. Test race management features

Would you like me to provide more details about any part of the setup or explain specific components in more detail? 