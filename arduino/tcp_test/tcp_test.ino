#include <WiFiS3.h>

const char* ssid = "thisistheway";
const char* password = "eatpie69";
const char* server = "192.168.86.32";
const int port = 8080;

// Static IP configuration
IPAddress staticIP(192, 168, 86, 39);
IPAddress gateway(192, 168, 86, 1);    // Your router IP
IPAddress subnet(255, 255, 255, 0);    // Standard subnet mask

WiFiClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("\nTCP Test with IP Verification");
  
  // Disconnect from any previous connections
  WiFi.disconnect();
  delay(1000);
  
  // Set static IP before connecting
  Serial.println("Configuring static IP...");
  WiFi.config(staticIP, gateway, subnet);
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait for connection with IP verification
  int attempts = 0;
  while (attempts < 20) { // Try for about 20 seconds
    if (WiFi.status() == WL_CONNECTED) {
      IPAddress ip = WiFi.localIP();
      if (ip[0] != 0) { // Check if we got a valid IP
        Serial.println("\nWiFi Connected Successfully!");
        Serial.print("Arduino IP: ");
        Serial.println(ip);
        Serial.print("Signal Strength (RSSI): ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        break;
      }
    }
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (attempts >= 20) {
    Serial.println("\nFailed to get valid IP address!");
    Serial.println("Restarting...");
    delay(1000);
    // Reset the Arduino
    NVIC_SystemReset();
  }
  
  // Additional network information
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  
  delay(5000); // Wait for network to fully stabilize
  
  Serial.println("Starting TCP test...");
}

void loop() {
  if (!client.connected()) {
    Serial.print("\nConnecting to server ");
    Serial.print(server);
    Serial.print(":");
    Serial.println(port);
    
    // Verify our IP before attempting connection
    IPAddress currentIP = WiFi.localIP();
    Serial.print("Current Arduino IP: ");
    Serial.println(currentIP);
    
    if (currentIP[0] == 0) {
      Serial.println("Invalid IP detected! Restarting...");
      delay(1000);
      NVIC_SystemReset();
    }
    
    // Try to connect with timeout
    unsigned long startAttempt = millis();
    bool connectResult = client.connect(server, port);
    unsigned long connectTime = millis() - startAttempt;
    
    if (connectResult) {
      Serial.println("Connected to server!");
      Serial.print("Connection took ");
      Serial.print(connectTime);
      Serial.println(" ms");
      client.println("Hello from Arduino!");
    } else {
      Serial.println("Connection failed");
      Serial.print("Connection attempt took ");
      Serial.print(connectTime);
      Serial.println(" ms");
      Serial.println("Diagnostics:");
      Serial.print("WiFi status: ");
      Serial.println(WiFi.status());
      Serial.print("Current IP: ");
      Serial.println(WiFi.localIP());
      Serial.print("Signal Strength: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
      
      // Try to ping the server IP
      Serial.print("\nPinging server IP... ");
      if (WiFi.ping(server) > 0) {
        Serial.println("SUCCESS");
        Serial.println("Server is reachable but TCP connection failed");
      } else {
        Serial.println("FAILED");
        Serial.println("Cannot reach server IP at all");
      }
      
      delay(5000);
    }
  }
  
  // Check for incoming data
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  // Keep connection alive
  if (client.connected()) {
    client.println("ping");
    delay(10000);
  }
} 