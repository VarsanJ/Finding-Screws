// --- PINS (Match your main file) ---
const int LEFT_IR = A0;   
const int RIGHT_IR = A1;
const int ENA = 5; const int IN1 = 6; const int IN2 = 7;
const int IN3 = 8; const int IN4 = 9; const int ENB = 10;

// TUNING VARIABLES
const int RED_SPEED = 150;      
const int FORK_TIMEOUT = 1500; // 1.5 Seconds to clear the fork (Adjust this!)

void runRedObstacleCourse() {
  Serial.println("Red Course: Fork Clearing Mode Active...");

  long startTime = millis();
  bool forkCleared = false;

  while (true) {
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);
    
    // Check if we are still in the "Danger Zone" (First 1.5 seconds)
    if (millis() - startTime < FORK_TIMEOUT) {
      
      // --- MODE 1: BLIND FORK CLEARING ---
      // We ignore the Left Sensor logic here to prevent turning into the fork.
      
      if (rightVal == 1) {
        // If Right sensor triggers, we really ARE drifting left. Fix it.
        moveRaw(RED_SPEED, 0); 
      }
      else {
        // Default: Drive Straight.
        // Even if leftVal == 1 (The Fork), we ignore it and go Straight.
        moveRaw(RED_SPEED, RED_SPEED);
      }
      
    } else {
      
      // --- MODE 2: STANDARD LINE FOLLOWING ---
      // The fork is behind us. Returns to normal behavior.
      
      if (!forkCleared) {
        Serial.println("Fork Cleared. Normal Logic Resumed.");
        forkCleared = true;
      }

      if (leftVal == 0 && rightVal == 0) {
        moveRaw(RED_SPEED, RED_SPEED);
      } 
      else if (leftVal == 1) {
        moveRaw(0, RED_SPEED); // Normal correction allowed now
      } 
      else if (rightVal == 1) {
        moveRaw(RED_SPEED, 0); 
      }
    }
    
    // --- END CONDITION ---
    if (detectColor() == "BLACK") {
      stopMotors();
      Serial.println("Red Course Complete.");
      break;
    }
  }
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
