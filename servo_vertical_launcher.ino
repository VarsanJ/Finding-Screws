// ROBOTIC ARM BALL PUSHER
// Servo at elbow joint - lifts arm up, then pushes forward
// Upload after reaching black center

#include <Servo.h>

Servo elbowServo;  // Servo at the elbow joint
const int servoPin = 13;  // Change if using different pin

// Servo positions for elbow joint
const int ARM_DOWN = 90;      // Arm resting (horizontal/down)
const int ARM_UP = 0;         // Arm lifted up (elbow bent up)
const int ARM_PUSH = 180;     // Arm extended forward (pushing motion)

// You might need to reverse these if your servo is mounted differently:
// const int ARM_DOWN = 90;
// const int ARM_UP = 180;
// const int ARM_PUSH = 0;

// Timing
const int LIFT_TIME = 600;    // Time to lift arm up
const int PAUSE_TIME = 200;   // Pause at top before pushing
const int PUSH_TIME = 300;    // Time for push motion

void setup() {
  elbowServo.attach(servoPin);
  
  // Start with arm down/resting
  elbowServo.write(ARM_DOWN);
  delay(1000);
  
  // Execute push sequence
  pushBall();
  
  // Return to resting position
  elbowServo.write(ARM_DOWN);
  delay(500);
  
  // Done
  while(1);
}

void loop() {
  // Not used
}

void pushBall() {
  // Step 1: Lift arm UP (bend elbow)
  elbowServo.write(ARM_UP);
  delay(LIFT_TIME);
  
  // Step 2: Brief pause at top
  delay(PAUSE_TIME);
  
  // Step 3: PUSH forward (extend arm)
  elbowServo.write(ARM_PUSH);
  delay(PUSH_TIME);
  
  // Step 4: Hold push position
  delay(500);
}


// ==========================================
// ALTERNATIVE: If you need SLOWER movement
// ==========================================
/*
void pushBallSlow() {
  // Gradually lift arm up
  for(int angle = ARM_DOWN; angle >= ARM_UP; angle--) {
    elbowServo.write(angle);
    delay(10);  // Smooth motion
  }
  
  delay(PAUSE_TIME);
  
  // Push forward fast
  elbowServo.write(ARM_PUSH);
  delay(PUSH_TIME);
}
*/


// ==========================================
// ALTERNATIVE: If you need MORE FORCE
// ==========================================
/*
void pushBallHard() {
  // Quick lift
  elbowServo.write(ARM_UP);
  delay(300);
  
  // FAST push (more force)
  elbowServo.write(ARM_PUSH);
  delay(100);  // Very fast = more impact
}
*/



