#include <WiFiS3.h>

const char* ssid = "thisistheway";
const char* password = "eatpie69";

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  Serial.println("\nWiFi Test with Reset");
  
  // Reset WiFi module
  WiFi.disconnect();
  delay(1000);
  WiFi.end();
  delay(1000);
  
  Serial.println("Starting WiFi connection...");
  int status = WiFi.begin(ssid, password);
  
  Serial.print("Initial status: ");
  Serial.println(status);
  
  Serial.print("Connecting");
  int attempts = 0;
  while (status != WL_CONNECTED && attempts < 20) {
    status = WiFi.status();
    Serial.print(".");
    Serial.print("(");
    Serial.print(status);
    Serial.print(")");
    delay(1000);
    attempts++;
  }
  
  Serial.println();
  
  if (status == WL_CONNECTED) {
    Serial.println("Connected successfully!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("Connection failed!");
    Serial.print("WiFi status = ");
    Serial.println(status);
  }
}

void loop() {
  // Print status every 5 seconds
  Serial.print("Current status: ");
  Serial.print(WiFi.status());
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  delay(5000);
} 