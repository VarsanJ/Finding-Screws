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

// TUNING VARIABLES - Inputted From Testing -- Might Require Changes live from test data
const int RED_SPEED = 150;      
const int TURN_SPEED = 150;     
const int REVERSE_SPEED = -100; // Negative speed for snappy turns
const int OBSTACLE_DIST = 15;   // Dodge if wall is closer than 15cm

// TIME VARIABLES - Inputted From Testing -- Might Require Changes live from test data
const int TIME_FOR_15CM = 500;  
const int TIME_FOR_30CM = 1000; 
const int TIME_TURN_90 = 600;   
const int FORK_TIMEOUT = 1500;  // Ignore left fork for first 1.5s

// SERVO POSITIONS - Program Constants -- No modifications from config/testing
const int ARM_DOWN = 100;       
const int ARM_UP = 0;

// DODGE VARIABLES - Inputted From Testing -- Might Require Changes live from test data
const int DODGE_TURN_TIME = 600;  
const int DODGE_OUT_TIME = 600;   
const int DODGE_PASS_TIME = 1200; 

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

void runRedObstacleCourse() {
  // Purpose: General program to run the obstacle course
  // Last Modified: 2026-01-331
  
  // Assign IO to the pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize servo motor used for the robot
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); // Start with arm up

  // Variables used specific to the design of the obstacle course
  long startTime = millis(); // How many ms after starting
  bool forkCleared = false;
  int blueTapeCount = 0; // 0 = First Task, 1 = Second Task

  // Main Program Loop
  while (true) {
    
    // Inititializes the variables needed and gets data
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    String detectedColor = detectColor(); 
    int distance = readDistance();

    // Top Priority: Obstacle Avoidance
    if (distance > 0 && distance < OBSTACLE_DIST) {
       executeObstacleDodge();
    }
    // Program Objective: Tasks necessary to complete obstacle
    else if (detectedColor == "BLUE") { // These tasks are only to occur when the robot is on blue tape
      
      // TASK 1: Cube Pickup on First Blue Tape
      if (blueTapeCount == 0) {
        executeFirstTask();
        blueTapeCount++; 
      }
      
      // TASK 2: Back-and-Forth Maneuver On Second Blue Tape
      else if (blueTapeCount == 1) {
        executeSecondTask();
        blueTapeCount++; 
      }
    }

    // The fork in the path must be navigatable correctly (right order)
      
    // For the first 1.5 seconds, we IGNORE the Left Sensor to pass the fork.
    else if (millis() - startTime < FORK_TIMEOUT) {
      if (rightVal == 1) { 
        moveRaw(RED_SPEED, 0); // Allow Right correction
      } else { 
        moveRaw(RED_SPEED, RED_SPEED); // Force Straight (Ignore Left)
      }
    } 
    
    // General Program: Following the line
    else {
      if (!forkCleared) {  // The Fork would now have been cleared
        forkCleared = true; 
      }
      
      if (leftVal == 0 && rightVal == 0) { 
        // Both White -> Drive Straight
        moveRaw(RED_SPEED, RED_SPEED); 
      } 
      else if (leftVal == 1) { 
        // Left sees Red -> Snappy Turn Left
        moveRaw(REVERSE_SPEED, TURN_SPEED); 
      } 
      else if (rightVal == 1) { 
        // Right sees Red -> Snappy Turn Right
        moveRaw(TURN_SPEED, REVERSE_SPEED); 
      }
    }
    
    // Finishing End
    if (detectedColor == "BLACK") {
      stopMotors();
      break; // The black line reached indicates that the program is complete
    }
  }
}

void executeFirstTask() {
  // Function Purpose: Execute the first task related to the cube
  // Last Modified: 2026-01-31

  // First ensure that the vehicle is stopped
  stopMotors();
  delay(500);

  // 1. Align (15cm, obtained measurement)
  moveRaw(150, 150); 
  delay(TIME_FOR_15CM); 
  stopMotors();
  
  // 2. Turn Right 90 degrees
  moveRaw(150, -150); 
  delay(TIME_TURN_90); 
  stopMotors();
  
  // 3. Grab Cube
  arm.write(ARM_DOWN); 
  delay(500); 
  moveRaw(100, 100); 
  delay(600); 
  stopMotors(); // Drive to cube
  arm.write(ARM_UP); 
  delay(1000); // Lift
  
  // 4. Return (Spin Left until Red)
  moveRaw(-150, 150); 
  while (detectColor() != "RED") { 
    delay(10); 
  }
  stopMotors();
  
  // Nudge forward slightly
  moveRaw(150, 150); 
  delay(200); 
}

void executeSecondTask() {
  stopMotors();
  delay(500);

  // 1. Forward 15cm (Clear tape)
  moveRaw(150, 150); 
  delay(TIME_FOR_15CM); 
  stopMotors();

  // 2. Turn Left 90 (Counter-Clockwise)
  moveRaw(-150, 150); 
  delay(TIME_TURN_90); 
  stopMotors();
  delay(500);

  // 3. Arm Down & Reverse 30cm
  arm.write(ARM_DOWN); 
  delay(500);
  moveRaw(-150, -150); 
  delay(TIME_FOR_30CM); 
  stopMotors();

  // 4. Arm Up & Forward 30cm
  arm.write(ARM_UP); 
  delay(500);
  moveRaw(150, 150); 
  delay(TIME_FOR_30CM); 
  stopMotors();

  // 5. Turn Right 90 (Clockwise) to Resume until red line found
  moveRaw(150, -150); 
  long searchStart = millis();
  while (detectColor() != "RED" && (millis() - searchStart < 3000)) { // 3s used as safety threshold
    delay(10);
  }
  
  stopMotors();
  moveRaw(150, 150); 
  delay(200); 
}

void executeObstacleDodge() {
  // Function Purpose: Design sequence of steps to dodge a detected obstacle
  // Last Modified: 2026-01-31
  
  stopMotors();
  delay(200);

  // 1. Turn Left since can not proceed straight
  moveRaw(-150, 150); 
  delay(DODGE_TURN_TIME);
  
  // 2. Drive Out
  moveRaw(150, 150); 
  delay(DODGE_OUT_TIME);
  
  // 3. Turn Right to ensure that the movement is parallel to the red tape
  moveRaw(150, -150); 
  delay(DODGE_TURN_TIME);
  
  // 4. Drive Past the obstacle while remaining parallel to the red tape
  moveRaw(150, 150); 
  delay(DODGE_PASS_TIME);
  
  // 5. Turn Right to return to the red tape once a sufficient distance has been passed
  moveRaw(150, -150); 
  delay(DODGE_TURN_TIME);
  
  // 6. Return to the red line until after it has been detected
  moveRaw(150, 150);
  long huntStart = millis();
  while (detectColor() != "RED" && (millis() - huntStart < 3000)) { // Ensures a safety threshold of 3s
    delay(10); 
  }
  stopMotors();

  // 7. Align (Spin Left until Right Sensor hits Red) with the direction of the path
  moveRaw(-150, 150);
  long alignStart = millis();
  while (digitalRead(RIGHT_IR) == 0 && (millis() - alignStart < 2000)) { // Ensures a safety threshold of 2s
    delay(10); 
  }
  stopMotors();
  
  delay(200);
}

int readDistance() {
  // Function Purpose: Determine the distance away for an object detected by the ultrasonic range finder
  // Last Modified: 2026-01-31

  // Initializs the range finder, used to ensure accuracy
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); 
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Collects duration and converts unit
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
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
  
  // BLUE ring - high blue, low red and green
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
