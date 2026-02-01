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

// --- TUNING VARIABLES ---
// RAMP_SPEED: Needs to be higher (180-255) to fight gravity
const int RAMP_SPEED = 200; 

// TIME_FOR_50CM: How long to drive blindly at the top
// Test this: if it goes too far/short, change this number.
const int TIME_FOR_50CM = 2000; // 2000ms = 2 seconds

void runRampAscent() {
  Serial.println("Ramp Section Started: Climbing...");

  // STEP 1: THE CLIMB
  // We keep climbing as long as the CENTER sensor sees the Black Line.
  // The moment it sees "Cardboard" (which will read as NOT BLACK), we stop.
  
  while (detectColor() == "BLACK") {
    
    // Read Side Sensors for steering
    int leftVal = digitalRead(LEFT_IR);
    int rightVal = digitalRead(RIGHT_IR);

    // --- LINE FOLLOWING LOGIC (HIGH POWER) ---
    if (leftVal == 0 && rightVal == 0) {
      // On Line: Full power forward
      moveRaw(RAMP_SPEED, RAMP_SPEED);
    } 
    else if (leftVal == 1) {
      // Drifted Right: Turn Left (Right motor pushes hard)
      moveRaw(0, RAMP_SPEED); 
    } 
    else if (rightVal == 1) {
      // Drifted Left: Turn Right (Left motor pushes hard)
      moveRaw(RAMP_SPEED, 0); 
    }
    else {
      // Both sensors see Cardboard? 
      // Just keep going straight, the Color Sensor will handle the stop.
      moveRaw(RAMP_SPEED, RAMP_SPEED);
    }
  }

  // STEP 2: THE ARRIVAL
  // The loop broke because detectColor() != "BLACK"
  // This means the center sensor is looking at the blank platform.
  
  stopMotors();
  delay(500); // Catch breath
  Serial.println("Top Reached! Extending 50cm...");

  // STEP 3: THE 50CM EXTENSION
  // Drive straight blindly to get fully onto the platform
  moveRaw(200, 200); 
  delay(TIME_FOR_50CM); 
  
  stopMotors();
  Serial.println("Ramp Complete. Resting on Platform.");
}

// --- HELPER FUNCTIONS ---
// (These should already be in your main file, but included here just in case)

void moveRaw(int left, int right) {
  // Left Motor
  if (left >= 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    analogWrite(ENA, left);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    analogWrite(ENA, -left);
  }
  
  // Right Motor
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
