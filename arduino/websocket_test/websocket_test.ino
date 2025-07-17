#include <WiFiS3.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "thisistheway";     // Your WiFi network name
const char* password = "eatpie69";      // Your WiFi password

// Server settings
const char* server = "192.168.86.20";  // Server address
const int port = 3001;                 // Server TCP port

// Create WiFi client
WiFiClient client;

// Unique ID for this boat
const char* boatId = "boat1";

void setup() {
    Serial.begin(115200);
    Serial.println("\nStarting up...");
    
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
            Serial.print("Sent: ");
            Serial.println(jsonString);
        } else {
            Serial.println("Connection failed");
            delay(5000);
            return;
        }
    }
    
    // Send telemetry every 2 seconds
    static unsigned long lastTelemetry = 0;
    if (millis() - lastTelemetry > 2000) {
        StaticJsonDocument<200> doc;
        doc["type"] = "telemetry";
        doc["boatId"] = boatId;
        doc["batteryLevel"] = random(0, 100);  // Simulated battery
        doc["speed"] = random(0, 30);          // Simulated speed
        
        String jsonString;
        serializeJson(doc, jsonString);
        jsonString += "\n";
        
        if (client.print(jsonString)) {
            Serial.print("Sent: ");
            Serial.println(jsonString);
        } else {
            Serial.println("Failed to send telemetry");
            client.stop();
        }
        
        lastTelemetry = millis();
    }
    
    // Check for incoming messages
    while (client.available()) {
        String message = client.readStringUntil('\n');
        if (message.length() > 0) {
            Serial.print("Received: ");
            Serial.println(message);
            
            StaticJsonDocument<200> doc;
            DeserializationError error = deserializeJson(doc, message);
            
            if (!error) {
                const char* type = doc["type"];
                
                if (strcmp(type, "control") == 0) {
                    int throttle = doc["throttle"] | 0;
                    int steering = doc["steering"] | 0;
                    
                    Serial.print("Control - Throttle: ");
                    Serial.print(throttle);
                    Serial.print(" Steering: ");
                    Serial.println(steering);
                }
                else if (strcmp(type, "emergency") == 0) {
                    Serial.println("EMERGENCY STOP!");
                }
            }
        }
    }
}