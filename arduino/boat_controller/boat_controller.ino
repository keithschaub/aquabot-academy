#include <WiFiS3.h>
#include <ArduinoJson.h>
#include <Servo.h>

// WiFi credentials
const char* ssid = "thisistheway";     // Your WiFi network name
const char* password = "eatpie69";      // Your WiFi password

// Server settings
const char* server = "192.168.86.32";  // Server address
const int port = 3001;                 // Server TCP port

// Create WiFi client
WiFiClient client;

// Unique ID for this boat
const char* boatId = "boat1";

// Pin definitions
const int MOTOR_PIN = 10;    // ESC control (PWM)
const int RUDDER_PIN = 9;    // Rudder servo

// ESC pulse width settings (in microseconds)
const int STOP_PULSE = 1000;    // Stop position
const int MAX_PULSE = 2000;     // Maximum throttle

// Control variables
Servo rudderServo;
Servo ESC;
int currentSpeed = 0;        // 0-100 for speed control
int currentAngle = 90;       // Center position for rudder

// Telemetry timing
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_MIN_INTERVAL = 100;  // Minimum 100ms between updates

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting up...");
  
  // Initialize servos
  rudderServo.attach(RUDDER_PIN);
  ESC.attach(MOTOR_PIN, STOP_PULSE, MAX_PULSE);
  
  // Center the rudder and ensure motor is stopped
  rudderServo.write(currentAngle);
  setEscPulse(STOP_PULSE);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // If not connected to server, try to connect
  if (!client.connected()) {
    connectToServer();
    return;
  }
  
  // Check for incoming messages
  while (client.available()) {
    String message = client.readStringUntil('\n');
    if (message.length() > 0) {
      handleMessage(message);
    }
  }
  
  // Send telemetry at regular intervals
  unsigned long currentTime = millis();
  if (currentTime - lastTelemetryTime >= TELEMETRY_MIN_INTERVAL) {
    sendTelemetry();
    lastTelemetryTime = currentTime;
  }
}

void connectToServer() {
  Serial.print("\nConnecting to server ");
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);
  
  if (client.connect(server, port)) {
    Serial.println("Connected!");
    
    // Send registration
    StaticJsonDocument<200> doc;
    doc["type"] = "register";
    doc["boatId"] = boatId;
    
    String jsonString;
    serializeJson(doc, jsonString);
    jsonString += "\n";
    
    client.print(jsonString);
  } else {
    Serial.println("Connection failed");
    delay(5000);
  }
}

void handleMessage(String message) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  
  if (!error) {
    const char* type = doc["type"];
    
    if (strcmp(type, "control") == 0) {
      // Extract control values
      int speed = doc["speed"] | 0;     // 0 to 100
      int angle = doc["angle"] | 90;    // 0 to 180
      
      // Update controls
      updateControls(speed, angle);
    }
    else if (strcmp(type, "emergency") == 0) {
      emergencyStop();
    }
  }
}

void updateControls(int speed, int angle) {
  // Update speed (0 to 100)
  currentSpeed = constrain(speed, 0, 100);
  
  // Map speed percentage to ESC pulse width
  int pulseWidth = map(currentSpeed, 0, 100, STOP_PULSE, MAX_PULSE);
  setEscPulse(pulseWidth);
  
  // Update rudder angle (0-180)
  currentAngle = constrain(angle, 45, 135);  // Limit to 45-135 range
  rudderServo.write(currentAngle);
  
  Serial.print("Speed: ");
  Serial.print(currentSpeed);
  Serial.print("% (");
  Serial.print(pulseWidth);
  Serial.print("µs) Angle: ");
  Serial.println(currentAngle);
}

void emergencyStop() {
  currentSpeed = 0;
  currentAngle = 90;
  setEscPulse(STOP_PULSE);
  rudderServo.write(currentAngle);
  Serial.println("EMERGENCY STOP!");
}

void sendTelemetry() {
  StaticJsonDocument<200> doc;
  doc["type"] = "telemetry";
  doc["boatId"] = boatId;
  doc["speed"] = currentSpeed;
  doc["angle"] = currentAngle;
  
  String jsonString;
  serializeJson(doc, jsonString);
  jsonString += "\n";
  
  if (client.print(jsonString)) {
    Serial.print("Sent telemetry: ");
    Serial.println(jsonString);
  } else {
    Serial.println("Failed to send telemetry");
    client.stop();
  }
}

// Safely set ESC speed ensuring we stay within bounds
void setEscPulse(int pulse) {
  pulse = constrain(pulse, STOP_PULSE, MAX_PULSE);
  ESC.writeMicroseconds(pulse);
} 