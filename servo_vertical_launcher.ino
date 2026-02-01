// SERVO BALL LAUNCHER
// Moves vertically up, then swings forward to launch ball
// Upload after reaching black center

#include <Servo.h>

Servo shooter;
const int servoPin = 13;  // Change this if using different pin

// Servo positions (adjust these based on your setup!)
const int START_POSITION = 90;    // Horizontal/resting position
const int UP_POSITION = 0;        // Vertical/up position (90° up from start)
const int FORWARD_POSITION = 180; // Forward swing position (90° forward from start)

// Timing (adjust for more/less force)
const int LIFT_TIME = 500;        // ms to move up
const int PAUSE_TIME = 200;       // ms to pause at top
const int SWING_TIME = 100;       // ms to swing forward (faster = more force)

void setup() {
  shooter.attach(servoPin);
  
  // Start at resting position
  shooter.write(START_POSITION);
  delay(1000);
  
  // Launch sequence
  launchBall();
  
  // Return to start
  shooter.write(START_POSITION);
  
  // Done
  while(1);
}

void loop() {
  // Not used
}

void launchBall() {
  // Step 1: Move UP (vertical)
  shooter.write(UP_POSITION);
  delay(LIFT_TIME);
  
  // Step 2: Brief pause at top
  delay(PAUSE_TIME);
  
  // Step 3: Swing FORWARD fast to hit ball
  shooter.write(FORWARD_POSITION);
  delay(SWING_TIME);
  
  // Step 4: Hold forward position briefly
  delay(300);
}
