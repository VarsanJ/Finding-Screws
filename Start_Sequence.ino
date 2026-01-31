#include <Servo.h>

// --- PINS (CHECK THESE WITH YOUR TEAM) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int SERVO_PIN = 9;  

// --- MOTORS (Need these to nudge forward) ---
// You might need to change these pin numbers to match 'car_movement_code.ino'
const int ENA = 5; 
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

// --- SERVO ANGLES (CRITICAL: You must test these!) ---
// Since it's a linkage arm, "0" might be up or down depending on how you screwed it on.
// TEST: Write 90. If it's half way, try 0 or 180 to find the limits.
const int ARM_UP = 0;    // Position to hold the cube high
const int ARM_DOWN = 100; // Position to line up the hook with the cube hole

Servo arm;

void setupStartSequence() {
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); // Start with arm raised
  
  // Setup Motor Pins if not done elsewhere
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
}

void runStartSequence() {
  
  // 1. Move to Blue Zone
  Serial.println("Moving to Pickup Zone...");
  while (detectColor() != "BLUE") {
    lineFollow(); // Uses your team's line follow logic
  }

  // 2. Stop at Blue
  stopMotors();
  delay(500); 

  // 3. THE "SPEARING" MANEUVER
  Serial.println("Aligning Hook...");
  
  // Lower the arm to "Attack Position"
  arm.write(ARM_DOWN); 
  delay(1000); // Give servo time to move
  
  // NUDGE FORWARD to insert hook into cube
  // We go slow (speed 100) for a short time (400ms)
  // You might need to increase/decrease '400' depending on distance
  moveRaw(100, 100); 
  delay(400); 
  stopMotors();
  
  // 4. LIFT
  Serial.println("Lifting...");
  arm.write(ARM_UP);
  delay(1000); // Wait for lift
  
  // 5. Exit Zone
  Serial.println("Got it! Moving to Split.");
  moveRaw(150, 150); // Drive normally
  delay(600); // Clear the blue patch
  
  // Resume Line Following to the split
  while (detectColor() == "BLACK" || detectColor() == "WHITE") {
     lineFollow();
  }
  
  stopMotors();
}

// --- HELPER FUNCTIONS ---

void moveRaw(int leftSpeed, int rightSpeed) {
  // Direct motor control for the "Nudge"
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void lineFollow() {
    // Simple placeholder for line following
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    
    if (leftVal == 0 && rightVal == 0) moveRaw(150, 150);
    else if (leftVal == 1) moveRaw(0, 150); // Turn Left
    else if (rightVal == 1) moveRaw(150, 0); // Turn Right
}

// Reuse your existing detectColor function here
// String detectColor() { ... }
