/*
 * ESC Test Sketch
 * For testing A2212/10T 1400KV brushless motor with 30A ESC
 * Using 1000µs as stop position, 2000µs as full throttle
 */

#include <Servo.h>

// Pin definitions
const int ESC_PIN = 9;
const int LED_PIN = 13;

// Standard RC pulse widths in microseconds
const int STOP_PULSE = 1000;    // Stop position
const int MAX_PULSE = 2000;     // Maximum throttle
const int MIN_THROTTLE = 1100;  // Minimum throttle to start moving
const int REV_MAX_PULSE = 1000; // Maximum reverse (if ESC supports it)
const int REV_MIN_THROTTLE = 900; // Minimum reverse throttle

Servo ESC;
bool isArmed = false;
bool isForward = true;  // Direction flag

void setup() {
  Serial.begin(115200);
  Serial.println(F("ESC Test Program - Bidirectional Mode"));
  Serial.println(F("Commands:"));
  Serial.println(F("'a' - Begin initialization"));
  Serial.println(F("'d' - Disarm ESC"));
  Serial.println(F("'0' to '9' - Set speed (0% to 90%)"));
  Serial.println(F("'m' - Maximum speed (100%)"));
  Serial.println(F("'r' - Toggle direction (forward/reverse)"));
  Serial.println(F("'s' - Stop motor"));
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize ESC with extended pulse width limits for reverse
  ESC.attach(ESC_PIN, REV_MIN_THROTTLE, MAX_PULSE);
  
  // Ensure we start at stop
  setEscPulse(STOP_PULSE);
  delay(1000);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    processCommand(command);
  }
}

// Safely set ESC speed ensuring we stay within bounds
void setEscPulse(int pulse) {
  // Constrain pulse width to safe values
  pulse = constrain(pulse, REV_MIN_THROTTLE, MAX_PULSE);
  ESC.writeMicroseconds(pulse);
}

void processCommand(char command) {
  switch (command) {
    case 'a':
      armESC();
      break;
      
    case 'd':
      disarmESC();
      break;
      
    case 'r':
      if (isArmed) {
        toggleDirection();
      }
      break;
      
    case 's':
    case '0':
      if (isArmed) {
        setEscPulse(STOP_PULSE);
        Serial.println(F("Motor stopped (1000µs)"));
      }
      break;
      
    case 'm':  // 100% power
      if (isArmed) {
        if (isForward) {
          setEscPulse(MAX_PULSE);
          Serial.println(F("Speed set to 100% (2000µs)"));
        } else {
          setEscPulse(REV_MAX_PULSE);
          Serial.println(F("Reverse speed set to 100% (1000µs)"));
        }
      }
      break;
      
    case '1' ... '9':
      if (isArmed) {
        int speedPercent = (command - '0') * 10;
        int pulseWidth;
        
        if (isForward) {
          // Forward: Map 0-100% to MIN_THROTTLE-MAX_PULSE
          pulseWidth = map(speedPercent, 0, 100, MIN_THROTTLE, MAX_PULSE);
          setEscPulse(pulseWidth);
          Serial.print(F("Forward speed set to "));
        } else {
          // Reverse: Map 0-100% to STOP_PULSE-REV_MIN_THROTTLE
          pulseWidth = map(speedPercent, 0, 100, STOP_PULSE, REV_MIN_THROTTLE);
          setEscPulse(pulseWidth);
          Serial.print(F("Reverse speed set to "));
        }
        
        Serial.print(speedPercent);
        Serial.print(F("% ("));
        Serial.print(pulseWidth);
        Serial.println(F("µs)"));
      } else {
        Serial.println(F("Error: Not initialized. Send 'a' first."));
      }
      break;
  }
}

void toggleDirection() {
  setEscPulse(STOP_PULSE);  // Always stop before changing direction
  delay(1000);  // Wait for motor to stop
  isForward = !isForward;
  Serial.print(F("Direction changed to: "));
  Serial.println(isForward ? F("FORWARD") : F("REVERSE"));
  Serial.println(F("Motor stopped - Send speed command"));
}

void armESC() {
  Serial.println(F("Starting ESC initialization..."));
  
  // Hold at stop position for initialization
  Serial.println(F("Setting stop position"));
  setEscPulse(STOP_PULSE);
  delay(3000);  // Give ESC time to recognize stop signal
  
  isArmed = true;
  isForward = true;  // Start in forward direction
  digitalWrite(LED_PIN, HIGH);
  Serial.println(F("ESC Ready - Starting at stop position"));
  Serial.println(F("Send '1'-'9' for speed, 'm' for 100%, 'r' to toggle direction"));
}

void disarmESC() {
  setEscPulse(STOP_PULSE);
  isArmed = false;
  digitalWrite(LED_PIN, LOW);
  Serial.println(F("ESC Disarmed"));
} 