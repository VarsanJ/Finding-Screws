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
const int TURN_SPEED = 150;     
const int REVERSE_SPEED = -100; 
const int OBSTACLE_DIST = 15;   

// DODGE TUNING
const int DODGE_TURN_TIME = 600;  
const int DODGE_OUT_TIME = 600;   
const int DODGE_PASS_TIME = 1200; 

const int FORK_TIMEOUT = 1500;  
const int TIME_FOR_15CM = 500;  
const int TIME_TURN_90 = 600;   
const int ARM_DOWN = 100;       
const int ARM_UP = 0;

Servo arm;

void runRedObstacleCourse() {
  Serial.println("Red Course: Active...");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  arm.attach(SERVO_PIN);
  arm.write(ARM_UP); 

  long startTime = millis();
  bool forkCleared = false;
  bool bluePickupDone = false; 

  while (true) {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    String detectedColor = detectColor(); 
    int distance = readDistance();

    // 1. OBSTACLE DODGE
    if (distance > 0 && distance < OBSTACLE_DIST) {
       Serial.println("Obstacle! Dodging...");
       executeObstacleDodge();
    }

    // 2. BLUE PICKUP
    else if (detectedColor == "BLUE" && !bluePickupDone) {
      Serial.println("Blue Tape! Picking up...");
      executeRedPickup();
      bluePickupDone = true; 
    }

    // 3. FORK CLEARING
    else if (millis() - startTime < FORK_TIMEOUT) {
      if (rightVal == 1) { moveRaw(RED_SPEED, 0); } 
      else { moveRaw(RED_SPEED, RED_SPEED); }
    } 
    
    // 4. NORMAL LINE FOLLOW
    else {
      if (!forkCleared) { forkCleared = true; }
      if (leftVal == 0 && rightVal == 0) { moveRaw(RED_SPEED, RED_SPEED); } 
      else if (leftVal == 1) { moveRaw(REVERSE_SPEED, TURN_SPEED); } 
      else if (rightVal == 1) { moveRaw(TURN_SPEED, REVERSE_SPEED); }
    }
    
    // 5. END
    if (detectedColor == "BLACK") {
      stopMotors();
      break;
    }
  }
}

// --- UPDATED DODGE LOGIC ---
void executeObstacleDodge() {
  stopMotors();
  delay(200);

  // 1. (Reverse Removed as requested)

  // 2. Turn Left (Face Away)
  Serial.println("Dodging: Turning Left...");
  moveRaw(-150, 150); 
  delay(DODGE_TURN_TIME);
  
  // 3. Drive Out 
  Serial.println("Dodging: Driving Out...");
  moveRaw(150, 150);
  delay(DODGE_OUT_TIME);

  // 4. Turn Right (Face Parallel)
  Serial.println("Dodging: Turning Right...");
  moveRaw(150, -150); 
  delay(DODGE_TURN_TIME);

  // 5. Drive Past 
  Serial.println("Dodging: Driving Past...");
  moveRaw(150, 150);
  delay(DODGE_PASS_TIME); 

  // 6. Turn Right (Face the Line)
  Serial.println("Dodging: Turning Right (To Find Line)...");
  moveRaw(150, -150); 
  delay(DODGE_TURN_TIME);

  // 7. Drive Until Center Hits Red
  Serial.println("Dodging: Hunting for Red Line...");
  moveRaw(150, 150);
  
  long huntStart = millis();
  while (detectColor() != "RED" && (millis() - huntStart < 3000)) {
     delay(10);
  }
  stopMotors();
  Serial.println("Line Found!");

  // 8. ALIGN TO TRACK (The "Smart Turn")
  // We are currently facing the line sideways.
  // We Spin LEFT until the RIGHT IR Sensor hits the line.
  // This confirms we have rotated 90 degrees and are straddling it.
  
  Serial.println("Aligning: Spinning Left until Right Sensor hits...");
  moveRaw(-150, 150); // Spin Left

  // Wait until Right IR sees Dark (1)
  // Safety timeout included
  long alignStart = millis();
  while (digitalRead(RIGHT_IR) == 0 && (millis() - alignStart < 2000)) {
    delay(10);
  }
  
  stopMotors();
  Serial.println("Aligned! Resuming...");
  delay(200);
}

// --- PICKUP SUB-ROUTINE ---
void executeRedPickup() {
  stopMotors();
  delay(500);
  moveRaw(150, 150); delay(TIME_FOR_15CM); stopMotors(); 
  moveRaw(150, -150); delay(TIME_TURN_90); stopMotors(); 
  arm.write(ARM_DOWN); delay(500); 
  moveRaw(100, 100); delay(600); stopMotors(); 
  arm.write(ARM_UP); delay(1000); 
  moveRaw(-150, 150); 
  while (detectColor() != "RED") { delay(10); }
  stopMotors();
}

// --- HELPERS ---
int readDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

// (Include standard helpers: moveRaw, stopMotors, detectColor)
