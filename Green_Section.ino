#include <Servo.h>

// --- PINS (Match your main file) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int ECHO_PIN = 3; // Ultrasonic Echo
const int TRIG_PIN = 4; // Ultrasonic Trig
const int SERVO_PIN = 9;  

// Motor Pins
const int ENA = 5; const int IN1 = 6; const int IN2 = 7;
const int IN3 = 8; const int IN4 = 9; const int ENB = 10;

// --- TUNING VARIABLES (YOU MUST CALIBRATE THESE) ---
// Times are in milliseconds
const int TIME_FOR_10CM = 300;     // Time to drive 10cm
const int TIME_TURN_90 = 600;      // Time to spin 90 degrees
const int TIME_TURN_180 = 1200;    // Time to spin 180 degrees
const int BOX_DISTANCE = 5;        // Stop when box is 5cm away
const int ARM_UP = 0;              // Servo angle for up
const int ARM_DOWN = 100;          // Servo angle for down (hooking)

Servo arm;

void runGreenSection() {
  Serial.println("Green Section Started");
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); // Ensure arm is up to start
  
  // Setup Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // STEP 1: Follow Green Tape until Blue
  // We reuse the line follow logic, assuming Green looks "Dark" to IR sensors
  // If Green looks "Light", you might need to invert the IR logic (0 vs 1)
  while (detectColor() != "BLUE") {
    lineFollow();
  }
  
  // STEP 2: Align (Go forward 10cm)
  stopMotors();
  delay(200);
  Serial.println("Blue Found. Aligning...");
  moveRaw(150, 150);
  delay(TIME_FOR_10CM);
  stopMotors();
  
  // STEP 3: Rotate 90 Right to face box
  Serial.println("Turning 90 Right...");
  moveRaw(150, -150); // Spin Right
  delay(TIME_TURN_90);
  stopMotors();
  delay(500); // Stabilize
  
  // STEP 4: Detect Box & Approach
  // Drive forward slowly until Ultrasonic sees the box close up
  Serial.println("Approaching Box...");
  while (readDistance() > BOX_DISTANCE) {
    moveRaw(100, 100); // Slow approach
  }
  stopMotors();
  
  // STEP 5: Pickup Sequence
  Serial.println("Picking up...");
  arm.write(ARM_DOWN); // Lower Hook
  delay(1000);
  
  moveRaw(100, 100);   // Nudge Forward to insert hook
  delay(400);          // Small nudge duration
  stopMotors();
  
  arm.write(ARM_UP);   // Lift Cube
  delay(1000);
  
  // STEP 6: Rotate 180 (Turn Around)
  Serial.println("Turning 180...");
  moveRaw(150, -150); // Spin Right (or Left)
  delay(TIME_TURN_180);
  stopMotors();
  
  // STEP 7: Drop Off Sequence
  Serial.println("Dropping off...");
  arm.write(ARM_DOWN); // Lower Arm
  delay(1000);
  
  moveRaw(-150, -150); // Back up 10cm (leave box)
  delay(TIME_FOR_10CM);
  stopMotors();
  
  arm.write(ARM_UP);   // Raise Arm (Empty)
  delay(1000);
  
  moveRaw(150, 150);   // Forward 10cm (return to center)
  delay(TIME_FOR_10CM);
  stopMotors();
  
  // STEP 8: Find Green Path Again
  Serial.println("Searching for Green...");
  // Spin Clockwise until we see Green
  while (detectColor() != "GREEN") {
    moveRaw(150, -150);
  }
  
  stopMotors();
  Serial.println("Green Path Regained! Resuming...");
}

// --- HELPER FUNCTIONS ---

int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  return distance; // Returns cm
}

// (Includes your standard moveRaw, stopMotors, lineFollow, detectColor here)
// Make sure to copy those from your previous file or ensure they are linked!  
