// BOX PICKUP - ROBOTIC ARM
// Use this at the START to pick up the blue box
// Arm goes down, scoops/grabs box, lifts up

#include <Servo.h>

Servo elbowServo;
const int servoPin = 13;

// Servo positions - ADJUST THESE based on your arm!
const int ARM_UP = 0;         // Arm lifted (holding box)
const int ARM_DOWN = 180;     // Arm down (reach box on ground)
const int ARM_MIDDLE = 90;    // Neutral position

// Timing
const int LOWER_TIME = 800;   // Time to lower arm to box
const int GRAB_TIME = 500;    // Time to grip/scoop box
const int LIFT_TIME = 800;    // Time to lift box up

void setup() {
  elbowServo.attach(servoPin);
  
  // Start with arm up/neutral
  elbowServo.write(ARM_MIDDLE);
  delay(1000);
  
  // Pick up the box
  pickUpBox();
  
  // Hold box in lifted position
  // (Don't return to neutral - keep holding!)
  
  // Done - robot can now drive with box
  while(1);
}

void loop() {
  // Not used
}

void pickUpBox() {
  // Step 1: Lower arm to reach box on ground
  elbowServo.write(ARM_DOWN);
  delay(LOWER_TIME);
  
  // Step 2: Pause to ensure arm is down
  delay(GRAB_TIME);
  
  // Step 3: Lift arm up with box
  // (Box should be scooped/grabbed by arm/gripper)
  elbowServo.write(ARM_UP);
  delay(LIFT_TIME);
  
  // Step 4: Hold position (keep box lifted)
  delay(500);
  
  // Box is now picked up and held!
}


// ==========================================
// ALTERNATIVE: Smooth pickup motion
// ==========================================
/*
void pickUpBoxSmooth() {
  // Slowly lower arm
  for(int angle = ARM_MIDDLE; angle <= ARM_DOWN; angle++) {
    elbowServo.write(angle);
    delay(15);
  }
  
  delay(GRAB_TIME);
  
  // Quickly lift up
  elbowServo.write(ARM_UP);
  delay(LIFT_TIME);
}
*/


// ==========================================
// ALTERNATIVE: If you need to "scoop" the box
// ==========================================
/*
void scoopBox() {
  // Lower arm behind box
  elbowServo.write(ARM_DOWN);
  delay(600);
  
  // Robot drives forward slightly here (add motor code)
  // This pushes box into the scoop
  
  // Lift with box
  elbowServo.write(ARM_UP);
  delay(800);
}
*/
