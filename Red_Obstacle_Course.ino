#include <Servo.h>

// --- PINS (Match your main file) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int TRIG_PIN = 4; // Ultrasonic Trig
const int ECHO_PIN = 3; // Ultrasonic Echo
const int SERVO_PIN = 9; 

// Motor Pins
const int ENA = 5; const int IN1 = 6; const int IN2 = 7;
const int IN3 = 8; const int IN4 = 9; const int ENB = 10;

// TUNING VARIABLES
const int RED_SPEED = 150;      
const int FORK_TIMEOUT = 1500;  // Time to ignore the left fork
const int TIME_FOR_15CM = 500;  // Align with box
const int TIME_TURN_90 = 600;   // Spin Right
const int ARM_DOWN = 100;       
const int ARM_UP = 0;

Servo arm;

void runRedObstacleCourse() {
  Serial.println("Red Course Started...");
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); // Ensure arm starts UP

  long startTime = millis();
  bool forkCleared = false;
  bool bluePickupDone = false; // Flag to ensure we only do it once

  while (true) {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    String detectedColor = detectColor(); // Get color from center sensor

    // --- 1. BLUE TAPE TRIGGER (The Pickup) ---
    if (detectedColor == "BLUE" && !bluePickupDone) {
      Serial.println("Blue Tape Detected! Initiating Pickup...");
      executeRedPickup();
      bluePickupDone = true; 
      // After pickup, we are back on Red. loop continues.
    }

    // --- 2. FORK CLEARING LOGIC (First 1.5 Seconds) ---
    else if (millis() - startTime < FORK_TIMEOUT) {
      // Blind mode to ignore left fork
      if (rightVal == 1) {
        moveRaw(RED_SPEED, 0); // Correct Right Drift
      } else {
        moveRaw(RED_SPEED, RED_SPEED); // Drive Straight (Ignore Left)
      }
    } 
    
    // --- 3. STANDARD RED LINE FOLLOWING ---
    else {
      if (!forkCleared) {
        Serial.println("Fork Cleared. Normal Logic Resumed.");
        forkCleared = true;
      }

      if (leftVal == 0 && rightVal == 0) {
        moveRaw(RED_SPEED, RED_SPEED);
      } 
      else if (leftVal == 1) {
        moveRaw(0, RED_SPEED); // Turn Left
      } 
      else if (rightVal == 1) {
        moveRaw(RED_SPEED, 0); // Turn Right
      }
    }
    
    // --- 4. END CONDITION ---
    if (detectedColor == "BLACK") {
      stopMotors();
      Serial.println("Red Course Complete.");
      break;
    }
  }
}

// --- SUB-ROUTINE: The Pickup Sequence ---
void executeRedPickup() {
  stopMotors();
  delay(500);

  // STEP 1: Align (Drive 15cm past Blue)
  Serial.println("Aligning (15cm)...");
  moveRaw(150, 150);
  delay(TIME_FOR_15CM);
  stopMotors();

  // STEP 2: Rotate 90 RIGHT
  Serial.println("Turning 90 Right...");
  moveRaw(150, -150); 
  delay(TIME_TURN_90);
  stopMotors();
  delay(500);

  // STEP 3: Approach & Insert Arm
  Serial.println("Approaching Box...");
  arm.write(ARM_DOWN); // Lower Arm First
  delay(500);
  
  // Drive forward blindly/timed to insert arm
  // (Adjust time to match distance to box)
  moveRaw(100, 100); 
  delay(600); 
  stopMotors();

  // STEP 4: Lift Arm
  Serial.println("Lifting Box...");
  arm.write(ARM_UP);
  delay(1000);

  // STEP 5: Return to Path (Counter-Clockwise Spin)
  // We spin LEFT until we see the Red Line again
  Serial.println("Scanning Left for Red Path...");
  moveRaw(-150, 150); // Spin Left (CCW)
  
  while (detectColor() != "RED") {
    delay(10); // Wait for sensor to hit Red
  }
  
  stopMotors();
  Serial.println("Red Path Regained! Resuming...");
  delay(500);
  
  // Nudge forward slightly to ensure IR sensors align
  moveRaw(150, 150);
  delay(100);
}

// --- HELPERS (Standard) ---
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
