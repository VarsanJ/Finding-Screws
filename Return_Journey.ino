// --- PINS (Match your main file) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int ENA = 5; const int IN1 = 6; const int IN2 = 7;
const int IN3 = 8; const int IN4 = 9; const int ENB = 10;

// TUNING VARIABLES
const int DESCENT_SPEED = 150;  // Slower speed for gravity
const int RETURN_SPEED = 150;   

void runReturnJourney() {
  Serial.println("Return Journey Started: Descending Immediately...");

  // STEP 1: Descend the Ramp (Black on Cardboard)
  // We assume we are already facing downhill.
  
  // Keep following the Black line until the Center Sensor sees GREEN
  // (This means we hit the bottom where the Green tape starts)
  while (detectColor() != "GREEN") {
    
    // Use IR Sensors to stay on Black Line
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);

    if (leftVal == 0 && rightVal == 0) {
      moveRaw(DESCENT_SPEED, DESCENT_SPEED);
    } 
    else if (leftVal == 1) {
      moveRaw(0, DESCENT_SPEED); // Correct Left
    } 
    else if (rightVal == 1) {
      moveRaw(DESCENT_SPEED, 0); // Correct Right
    }
  }
  
  // We saw Green! We are off the ramp.
  Serial.println("Hit Green Tape! Switching to Flat Mode.");
  stopMotors();
  delay(200); // Stabilize at the bottom
  
  // Nudge forward slightly to ensure we are fully on Green tape
  moveRaw(150, 150);
  delay(300);

  // STEP 2: The Green Return (Ignoring Blue)
  // Drive on Green. Ignore Blue. Stop at Black/Red.
  
  Serial.println("Driving Home on Green...");
  while (detectColor() != "BLACK" && detectColor() != "RED") {
    
    // Standard Line Following
    lineFollow(); 
  }

  // STEP 3: The Finish Line
  stopMotors();
  Serial.println("Intersection Reached! Mission Complete.");
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

void lineFollow() {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    
    if (leftVal == 0 && rightVal == 0) {
      moveRaw(RETURN_SPEED, RETURN_SPEED);
    } else if (leftVal == 1) {
      moveRaw(0, RETURN_SPEED);
    } else if (rightVal == 1) {
      moveRaw(RETURN_SPEED, 0);
    }
}
