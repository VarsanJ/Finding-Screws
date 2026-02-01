#include <Servo.h>

// Component Pins - Inputted From Hardware - Might Require Changes Based on Config
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int TRIG_PIN = 4; 
const int ECHO_PIN = 3; 
const int SERVO_PIN = 13; 

// Motor Pins - Inputted From Hardware - Might Require Changes Based on Config
const int ENA = 5; 
const int IN1 = 6; 
const int IN2 = 7;
const int IN3 = 8; 
const int IN4 = 9; 
const int ENB = 10;

// Tuning Variables - Inputted From Testing -- Might Require Changes live from test data
// Times are in milliseconds
const int TIME_FOR_10CM = 300;     // Time to drive 10cm
const int TIME_TURN_90 = 600;      // Time to spin 90 degrees
const int TIME_TURN_180 = 1200;    // Time to spin 180 degrees
const int BOX_DISTANCE = 5;        // Stop when box is 5cm away
const int ARM_UP = 0;              // Servo angle for up
const int ARM_DOWN = 100;          // Servo angle for down (hooking)

// Declares the servo arm
Servo arm;

// TCS3200 Color Sensor Pins
const int S0 = 11;
const int S1 = 12;
const int S2 = 2;
const int S3 = 1;
const int signal = 4;

// Variables for color readings
unsigned long red, green, blue, clearX;

// Detected color and navigation state
String currentColor = "UNKNOWN";
String previousColor = "UNKNOWN";

void runGreenSection() {
  // Function Purpose: Operate the Green Section of the Route
  // Last Modified: 2026-01-31
  
  arm.attach(SERVO_PIN); // Attaches the arm
  arm.write(ARM_UP); // Ensure arm is up to start
  
  // Setup Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // STEP 1: Follow Green Tape until Blue
  while (detectColor() != "BLUE") {
    lineFollow();
  }
  
  // STEP 2: Align (Go forward 10cm)
  stopMotors();
  delay(200);
  moveRaw(150, 150);
  delay(TIME_FOR_10CM);
  stopMotors();
  
  // STEP 3: Rotate 90 Right to face box
  moveRaw(150, -150); // Spin Right
  delay(TIME_TURN_90);
  stopMotors();
  delay(500); // Stabilize
  
  // STEP 4: Detect Box & Approach
  // Drive forward slowly until Ultrasonic sees the box close up
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
  moveRaw(150, -150); // Spin Right (or Left)
  delay(TIME_TURN_180);
  stopMotors();
  
  // STEP 7: Drop Off Sequence
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
  
  // STEP 8: Find Green Path Again & Return to Start
  // Spin Clockwise until see Green
  while (detectColor() != "GREEN") {
    moveRaw(150, -150);
  }
  stopMotors();
  
  // STEP 9: RETURN TO START
  Serial.println("Returning to start...");
  returnToStart();
  
  // STEP 10: Final Stop at Start
  stopMotors();
  Serial.println("Green Section Complete - At Start Position");
}

void returnToStart() {
  // Function Purpose: Navigate back to the starting position
  // Last Modified: 2026-01-31
  
  // Follow green line backwards to start
  // Robot should reverse along the path OR turn around and drive forward
  
  // Option A: Turn around and follow line back
  moveRaw(150, -150); // Spin 180
  delay(TIME_TURN_180);
  stopMotors();
  
  // Follow green line until we reach the start (black line/marker)
  while (detectColor() != "BLACK") {
    lineFollow();
  }
  
  // Reached black start line
  stopMotors();
  delay(200);
  
  // Move forward slightly to position at exact start
  moveRaw(150, 150);
  delay(TIME_FOR_10CM);
  stopMotors();
  
  Serial.println("Reached start position");
}

int readDistance() {
  // Function Purpose: Determine the distance away for an object detected by the ultrasonic range finder
  // Last Modified: 2026-01-31
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  return distance; // Returns cm
}

void moveRaw(int left, int right) {
  // Function Purpose: Enable Rotational Movement when Necessary
  // Last Edited: 2026-01-31

  
  if (left >= 0) {
    // This means that the motor must move left (based on the left sensor)
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, LOW);
    analogWrite(ENA, left);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -left);
  }
  
  if (right >= 0) {
    // This means that the motor must move right (based on the right sensor)
    digitalWrite(IN3, HIGH); 
    digitalWrite(IN4, LOW);
    analogWrite(ENB, right);
  } else {
    digitalWrite(IN3, LOW); 
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -right);
  }
}

void stopMotors() {
  // Function Purpose: Disable the motor controlling the wheels to prevent movement
  // Last Edited: 2026-01-31 
  
  // Set motor speed controls to zero
  analogWrite(ENA, 0); 
  analogWrite(ENB, 0);
  // Disable Motor Pins to stop wheel
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, LOW);
}

String detectColor() {
  // Function Purpose: Detect Colour on Surface
  // Last Modified: 2026-01-31
  
  // Read clear
  digitalWrite(S2, HIGH);
  digitalWrite(S3, LOW);
  clearX = pulseIn(signal, HIGH);
  
  // Read red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  red = pulseIn(signal, HIGH);
  
  // Read green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green = pulseIn(signal, HIGH);
  
  // Read blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue = pulseIn(signal, HIGH);
  
  // Map values - CALIBRATE THESE!
  red = map(red, 80, 30, 0, 255);
  green = map(green, 80, 30, 0, 255);
  blue = map(blue, 80, 30, 0, 255);
  
  // Constrain to valid range
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);
  
  // Store previous color
  previousColor = currentColor;
  
  // Identify current color
  currentColor = identifyColor(red, green, blue);

  // Returns the colour
  return currentColor;
}

String identifyColor(int r, int g, int b) {
  // Function Purpose: Identify Colour on Surface
  // Last Modified: 2026-01-31
  
  // WHITE (between rings or background) - all values high
  if (r > 180 && g > 180 && b > 180) {
    return "WHITE";
  }
  
  // RED ring - high red, low green and blue
  if (r > 140 && g < 100 && b < 100 && r > g && r > b) {
    return "RED";
  }
  
  // BLUE ring - high blue, low red and low green
  if (r < 100 && g < 100 && b > 140 && b > r && b > g) {
    return "BLUE";
  }

  // GREEN ring - low blue, low red and high green
  if (r < 100 && b < 100 && g > 140 && g > r && g > b) {
    return "GREEN";
  }

  // Black ring
  if (r < 50 && b < 50 && g < 50){
    return "BLACK";
  }
    
  return "UNKNOWN";
}

void lineFollow() {
  // Function Purpose: Follow Line
  // Last Modified: 2026-01-31
  
  int leftVal = digitalRead(LEFT_IR);
  int rightVal = digitalRead(RIGHT_IR);
    
  // Adjust these based on your specific sensor logic (0 vs 1 for line)
  if (leftVal == 0 && rightVal == 0) {
    moveRaw(150, 150); // Straight
  } else if (leftVal == 1) {
    moveRaw(0, 150);   // Turn Left
  } else if (rightVal == 1) {
    moveRaw(150, 0);   // Turn Right
  }
}
