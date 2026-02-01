#include <Servo.h>

// --- PINS (Must match your Main File) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int TRIG_PIN = 4; 
const int ECHO_PIN = 3; 
const int SERVO_PIN = 9; 

// Motor Pins
const int ENA = 5; const int IN1 = 6; const int IN2 = 7;
const int IN3 = 8; const int IN4 = 9; const int ENB = 10;

// --- TUNING VARIABLES (Calibrate these on the floor!) ---
const int RED_SPEED = 150;      
const int TURN_SPEED = 150;     
const int REVERSE_SPEED = -100; // Negative speed for snappy turns
const int OBSTACLE_DIST = 15;   // Dodge if wall is closer than 15cm

// TIME VARIABLES
const int TIME_FOR_15CM = 500;  
const int TIME_FOR_30CM = 1000; 
const int TIME_TURN_90 = 600;   
const int FORK_TIMEOUT = 1500;  // Ignore left fork for first 1.5s

// SERVO POSITIONS
const int ARM_DOWN = 100;       
const int ARM_UP = 0;

// DODGE VARIABLES
const int DODGE_TURN_TIME = 600;  
const int DODGE_OUT_TIME = 600;   
const int DODGE_PASS_TIME = 1200; 

Servo arm;

void runRedObstacleCourse() {
  Serial.println("Red Course: Full System Active...");
  
  // Setup Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); // Start with arm up

  long startTime = millis();
  bool forkCleared = false;
  int blueTapeCount = 0; // 0 = First Task, 1 = Second Task

  // --- MAIN LOOP ---
  while (true) {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    String detectedColor = detectColor(); 
    int distance = readDistance();

    // --- PRIORITY 1: OBSTACLE AVOIDANCE (Safety) ---
    // Only dodge if we are NOT currently on a Blue Tape task
    // (distance > 0 filters out sensor glitches)
    if (distance > 0 && distance < OBSTACLE_DIST) {
       Serial.println("Obstacle Detected! Initiating Dodge...");
       executeObstacleDodge();
    }

    // --- PRIORITY 2: BLUE TAPE TASKS ---
    else if (detectedColor == "BLUE") {
      
      // TASK 1: Cube Pickup
      if (blueTapeCount == 0) {
        Serial.println("1st Blue Tape: Cube Pickup...");
        executeFirstTask();
        blueTapeCount++; 
      }
      
      // TASK 2: Back-and-Forth Maneuver
      else if (blueTapeCount == 1) {
        Serial.println("2nd Blue Tape: Back-and-Forth...");
        executeSecondTask();
        blueTapeCount++; 
      }
    }

    // --- PRIORITY 3: START FORK PROTECTION ---
    // For the first 1.5 seconds, we IGNORE the Left Sensor to pass the fork.
    else if (millis() - startTime < FORK_TIMEOUT) {
      if (rightVal == 1) { 
        moveRaw(RED_SPEED, 0); // Allow Right correction
      } else { 
        moveRaw(RED_SPEED, RED_SPEED); // Force Straight (Ignore Left)
      }
    } 
    
    // --- PRIORITY 4: STANDARD LINE FOLLOWING ---
    else {
      if (!forkCleared) { forkCleared = true; }
      
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
    
    // --- PRIORITY 5: FINISH LINE ---
    if (detectedColor == "BLACK") {
      stopMotors();
      Serial.println("Red Course Complete. Mission Accomplished.");
      break; // EXIT THE LOOP
    }
  }
}

// ==========================================
//           TASK 1: CUBE PICKUP
// ==========================================
void executeFirstTask() {
  stopMotors();
  delay(500);

  // 1. Align (15cm)
  moveRaw(150, 150); delay(TIME_FOR_15CM); stopMotors();
  
  // 2. Turn Right 90
  moveRaw(150, -150); delay(TIME_TURN_90); stopMotors();
  
  // 3. Grab Cube
  arm.write(ARM_DOWN); delay(500); 
  moveRaw(100, 100); delay(600); stopMotors(); // Drive to cube
  arm.write(ARM_UP); delay(1000); // Lift
  
  // 4. Return (Spin Left until Red)
  Serial.println("Returning to path...");
  moveRaw(-150, 150); 
  while (detectColor() != "RED") { delay(10); }
  stopMotors();
  
  // Nudge forward slightly
  moveRaw(150, 150); delay(200); 
}

// ==========================================
//           TASK 2: BACK-AND-FORTH
// ==========================================
void executeSecondTask() {
  stopMotors();
  delay(500);

  // 1. Forward 15cm (Clear tape)
  moveRaw(150, 150); delay(TIME_FOR_15CM); stopMotors();

  // 2. Turn Left 90 (Counter-Clockwise)
  moveRaw(-150, 150); delay(TIME_TURN_90); stopMotors();
  delay(500);

  // 3. Arm Down & Reverse 30cm
  arm.write(ARM_DOWN); delay(500);
  moveRaw(-150, -150); delay(TIME_FOR_30CM); stopMotors();

  // 4. Arm Up & Forward 30cm
  arm.write(ARM_UP); delay(500);
  moveRaw(150, 150); delay(TIME_FOR_30CM); stopMotors();

  // 5. Turn Right 90 (Clockwise) to Resume
  // We spin Right until we find the Red Line again
  Serial.println("Creating alignment...");
  moveRaw(150, -150); 
  long searchStart = millis();
  while (detectColor() != "RED" && (millis() - searchStart < 3000)) {
    delay(10);
  }
  
  stopMotors();
  moveRaw(150, 150); delay(200); 
}

// ==========================================
//           OBSTACLE DODGE
// ==========================================
void executeObstacleDodge() {
  stopMotors();
  delay(200);

  // 1. Turn Left (Face Away)
  moveRaw(-150, 150); delay(DODGE_TURN_TIME);
  
  // 2. Drive Out
  moveRaw(150, 150); delay(DODGE_OUT_TIME);
  
  // 3. Turn Right (Parallel)
  moveRaw(150, -150); delay(DODGE_TURN_TIME);
  
  // 4. Drive Past
  moveRaw(150, 150); delay(DODGE_PASS_TIME);
  
  // 5. Turn Right (Towards Line)
  moveRaw(150, -150); delay(DODGE_TURN_TIME);
  
  // 6. Hunt for Red Line (Center Sensor)
  moveRaw(150, 150);
  long huntStart = millis();
  while (detectColor() != "RED" && (millis() - huntStart < 3000)) { delay(10); }
  stopMotors();

  // 7. Align (Spin Left until Right Sensor hits Red)
  // This ensures we are straddling the line correctly
  moveRaw(-150, 150);
  long alignStart = millis();
  while (digitalRead(RIGHT_IR) == 0 && (millis() - alignStart < 2000)) { delay(10); }
  stopMotors();
  
  delay(200);
}

// --- HELPER FUNCTIONS ---
int readDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void moveRaw(int left, int right) {
  if (left >= 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    analogWrite(ENA, left);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    analogWrite(ENA, -left);
  }
  
  if (right >= 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    analogWrite(ENB, right);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    analogWrite(ENB, -right);
  }
}

void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
