// --- PINS (Match with your team's main file) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
// Motor pins
const int ENA = 5; 
const int IN1 = 6; 
const int IN2 = 7;
const int IN3 = 8; 
const int IN4 = 9; 
const int ENB = 10;

void runStartSequence() {
  Serial.println("Start: Following Black Line...");

  // STEP 1: Follow Black Line until the intersection
  // We assume the intersection starts when the center sensor sees ANYTHING other than Black
  while (detectColor() == "BLACK") {
    lineFollow();
  }

  // STEP 2: We hit the intersection! 
  stopMotors();
  delay(200); // Stabilize
  Serial.println("Intersection Reached. Spinning Clockwise for Green...");

  // STEP 3: Spin Clockwise until Green
  while (detectColor() != "GREEN") {
    // Clockwise Spin = Left Motor Forward, Right Motor Back
    // Adjust speed (150) if it spins too fast/slow
    moveRaw(150, -150); 
  }
  
  // STEP 4: Found Green!
  stopMotors();
  Serial.println("Green Found! Sequence Complete.");
}

// --- HELPER FUNCTIONS ---

// Direct motor control
void moveRaw(int leftSpeed, int rightSpeed) {
  // Left Motor Logic
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    analogWrite(ENA, leftSpeed);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    analogWrite(ENA, -leftSpeed); // Convert negative to positive PWM
  }
  
  // Right Motor Logic
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    analogWrite(ENB, rightSpeed);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    analogWrite(ENB, -rightSpeed);
  }
}

void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// Basic Line Following for Step 1
void lineFollow() {
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
