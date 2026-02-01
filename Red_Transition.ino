// IR Sensors
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;

// Motor Pins
const int ENA = 5; 
const int IN1 = 6; 
const int IN2 = 7;
const int IN3 = 8; 
const int IN4 = 9; 
const int ENB = 10;

// TUNING VARIABLES
const int TIME_FOR_15CM = 500;   // Time to drive 15cm (Needs calibration!)
const int SPIN_SPEED = 150;      // Speed for turning in place
const int RED_SPEED = 150;       // Speed for following Red

void runRedTransition() {
  Serial.println("Transition: Green -> Red (Overshoot Method)");

  // STEP 1: Clear the Intersection
  // Drive forward 15cm blindly to get away from the Green/Black junction
  Serial.println("Driving forward 15cm...");
  moveRaw(150, 150); 
  delay(TIME_FOR_15CM);
  stopMotors();
  delay(300); // Stabilize

  // STEP 2: The Search Spin
  // Spin Clockwise (Right) until we hit the Red line
  Serial.println("Spinning Clockwise to find Red...");
  moveRaw(SPIN_SPEED, -SPIN_SPEED); 

  // Keep spinning as long as we DO NOT see Red
  while (detectColor() != "RED") {
    // Just keep spinning
    delay(10);
  }
  
  // STEP 3: Lock and Load
  stopMotors();
  Serial.println("Red Tape Detected! Starting Obstacle Course.");
  delay(500);

  // STEP 4: Follow Red Logic
  // Now we just stick to the Red line.
  while (true) {
    lineFollowRed();
    
    // Stop if we see Black (End of Red path?) or another condition
    if (detectColor() == "BLACK") {
      Serial.println("End of Red Path.");
      break; 
    }
  }
  
  stopMotors();
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

// Specific Line Follower for Red (Checks for "Dark" line)
void lineFollowRed() {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    
    if (leftVal == 0 && rightVal == 0) {
      moveRaw(RED_SPEED, RED_SPEED);
    } else if (leftVal == 1) {
      moveRaw(0, RED_SPEED); // Turn Left
    } else if (rightVal == 1) {
      moveRaw(RED_SPEED, 0); // Turn Right
    }
}
