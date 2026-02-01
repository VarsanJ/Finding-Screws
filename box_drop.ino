// BOX DROP - ROBOTIC ARM
// Use this to DROP the box in the blue zone
// Lowers arm to release box

#include <Servo.h>

Servo elbowServo;
const int servoPin = 13;

// Servo positions
const int ARM_UP = 0;         // Holding box (lifted)
const int ARM_DOWN = 180;     // Drop position (lowered)
const int ARM_RELEASE = 135;  // Partial lower to release

// Timing
const int LOWER_TIME = 600;   // Time to lower and drop box

void setup() {
  elbowServo.attach(servoPin);
  
  // Start with arm up (holding box from pickup)
  elbowServo.write(ARM_UP);
  delay(500);
  
  // Drop the box
  dropBox();
  
  // Return to neutral
  elbowServo.write(90);
  delay(500);
  
  // Done
  while(1);
}

void loop() {
  // Not used
}

void dropBox() {
  // Lower arm to release box
  elbowServo.write(ARM_DOWN);
  delay(LOWER_TIME);
  
  // Brief pause with box on ground
  delay(300);
  
  // Lift arm away from box
  elbowServo.write(ARM_UP);
  delay(400);
}


// ==========================================
// ALTERNATIVE: Gentle drop
// ==========================================
/*
void dropBoxGentle() {
  // Slowly lower to ground
  for(int angle = ARM_UP; angle <= ARM_DOWN; angle++) {
    elbowServo.write(angle);
    delay(10);
  }
  
  delay(500);
  
  // Quickly retract
  elbowServo.write(ARM_UP);
  delay(300);
}
*/
