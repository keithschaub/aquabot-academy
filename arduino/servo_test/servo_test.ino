#include <Servo.h>

// Create servo object
Servo rudderServo;

// Pin definition
const int SERVO_PIN = 10;  // The pin where you connected the orange (signal) wire

// Constants
const int CENTER_POS = 90;   // Center position
const int LEFT_POS = 30;     // Left position
const int RIGHT_POS = 150;   // Right position
const int NUM_CYCLES = 5;    // Number of times to perform the movement

// Counter for cycles
int cycleCount = 0;
bool testComplete = false;

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  Serial.println("Servo Test Starting...");
  
  // Attach servo to pin
  rudderServo.attach(SERVO_PIN);
  
  // Move to center position
  Serial.println("Moving to center position (90 degrees)");
  rudderServo.write(CENTER_POS);
  
  // Wait 5 seconds before starting the test
  Serial.println("Waiting 5 seconds...");
  delay(5000);
}

void loop() {
  // Only perform the test if we haven't completed all cycles
  if (!testComplete) {
    if (cycleCount < NUM_CYCLES) {
      // Move to left position
      Serial.print("Cycle ");
      Serial.print(cycleCount + 1);
      Serial.println(" - Moving to 30 degrees");
      rudderServo.write(LEFT_POS);
      delay(1000);
      
      // Move to right position
      Serial.print("Cycle ");
      Serial.print(cycleCount + 1);
      Serial.println(" - Moving to 150 degrees");
      rudderServo.write(RIGHT_POS);
      delay(1000);
      
      // Increment cycle counter
      cycleCount++;
    } else {
      // All cycles complete, move to center and stay there
      Serial.println("Test complete! Moving to center position (90 degrees)");
      rudderServo.write(CENTER_POS);
      testComplete = true;
    }
  }
  // Once test is complete, do nothing (stay at center position)
} 