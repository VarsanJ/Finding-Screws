#include <Servo.h>

// --- PINS ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int TRIG_PIN = 4; 
const int ECHO_PIN = 3; 
const int SERVO_PIN = 9; 

// Motor Pins
const int ENA = 5; const int IN1 = 6; const int IN2 = 7;
const int IN3 = 8; const int IN4 = 9; const int ENB = 10;

// TUNING VARIABLES
const int RED_SPEED = 150;      
const int TURN_SPEED = 150;     // Speed of the outer wheel
const int REVERSE_SPEED = -100; // Speed of the inner wheel (Negative = Tighter Turn)

const int FORK_TIMEOUT = 1500;  
const int TIME_FOR_15CM = 500;  
const int TIME_TURN_90 = 600;   
const int ARM_DOWN = 100;       
const int ARM_UP = 0;

Servo arm;

void runRedObstacleCourse() {
  Serial.println("Red Course: Automatic Steering Active...");
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); 

  long startTime = millis();
  bool forkCleared = false;
  bool bluePickupDone = false; 

  while (true) {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    String detectedColor = detectColor(); 

    // --- 1. BLUE TAPE TRIGGER ---
    if (detectedColor == "BLUE" && !bluePickupDone) {
      Serial.println("Blue Tape! Picking up...");
      executeRedPickup();
      bluePickupDone = true; 
    }

    // --- 2. FORK CLEARING (First 1.5s) ---
    else if (millis() - startTime < FORK_TIMEOUT) {
      if (rightVal == 1) {
        moveRaw(RED_SPEED, 0); // Keep old gentle correction for the fork
      } else {
        moveRaw(RED_SPEED, RED_SPEED); 
      }
    } 
    
    // --- 3. AUTOMATIC "SNAPPY" TURNING ---
    else {
      if (!forkCleared) {
        Serial.println("Fork Cleared. Engaging Tight Steering.");
        forkCleared = true;
      }

      if (leftVal == 0 && rightVal == 0) {
        // Both White: Perfect! Drive Straight.
        moveRaw(RED_SPEED, RED_SPEED);
      } 
      else if (leftVal == 1) {
        // Left Sensor sees Red (Drifting Right or Turn Left)
        // ACTION: Reverse Left Wheel to snap back quickly
        moveRaw(REVERSE_SPEED, TURN_SPEED); 
      } 
      else if (rightVal == 1) {
        // Right Sensor sees Red (Drifting Left or Turn Right)
        // ACTION: Reverse Right Wheel to snap back quickly
        moveRaw(TURN_SPEED, REVERSE_SPEED); 
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
  arm.write(ARM_DOWN); 
  delay(500);
  
  moveRaw(100, 100); 
  delay(600); 
  stopMotors();

  // STEP 4: Lift Arm
  Serial.println("Lifting Box...");
  arm.write(ARM_UP);
  delay(1000);

  // STEP 5: Return to Path (Counter-Clockwise Spin)
  Serial.println("Scanning Left for Red Path...");
  moveRaw(-150, 150); // Spin Left (CCW)
  
  while (detectColor() != "RED") {
    delay(10); 
  }
  
  stopMotors();
  Serial.println("Red Path Regained! Resuming...");
  delay(500);
  
  moveRaw(150, 150);
  delay(100);
}

// --- HELPERS ---
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
