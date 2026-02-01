// TCS3200 Color Sensor Pins
const int S0 = 11;
const int S1 = 12;
const int S2 = 2;
const int S3 = 3;
const int signal = 4;

/ L298N Motor Driver Pins
const int ENA = 9;
const int IN1 = 7;
const int IN2 = 8;
const int ENB = 10;
const int IN3 = 11;
const int IN4 = 12;

// Motor speeds
const int SPEED_NORMAL = 150;
const int SPEED_SLOW = 100;

// Color readings
unsigned long red, green, blue, clear;

// Navigation state
int ringProgress = 0; // 0=not started, 1=past blue, 2=past red, 3=at center

void setup() {
  // Color sensor pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(signal, INPUT);
  
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  delay(1000);
}

void loop() {
  readColor();
  
  // DIRECT RGB COMPARISON - Track progress through rings
  if (red < 100 && green < 100 && blue > 150 && ringProgress == 0) {
    // BLUE ring detected
    ringProgress = 1;
  }
  else if (red > 150 && green < 100 && blue < 100 && ringProgress >= 1) {
    // RED ring detected
    ringProgress = 2;
  }
  else if (red < 50 && green < 50 && blue < 50 && ringProgress >= 2) {
    // BLACK center detected
    ringProgress = 3;
    reachedCenter();
  }
  
  // DIRECT RGB COMPARISON - Navigation logic
  if (red < 50 && green < 50 && blue < 50) {
    // BLACK - at center
    reachedCenter();
  }
  else if (red < 100 && green < 100 && blue > 150) {
    // BLUE ring - navigate inward
    navigateInward();
  }
  else if (red > 150 && green < 100 && blue < 100) {
    // RED ring - navigate inward
    navigateInward();
  }
  else if (red > 180 && green > 180 && blue > 180) {
    // WHITE - between rings, continue inward
    handleWhiteSpace();
  }
  else {
    // Unknown color - search
    slowSearch();
  }
  
  delay(50);
}

void readColor() {
  // Read clear
  digitalWrite(S2, HIGH);
  digitalWrite(S3, LOW);
  clear = pulseIn(signal, HIGH);
  
  // Read red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  red = pulseIn(signal, HIGH);
  
  // Read green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green = pulseIn(signal, HIGH);
  
  // Read blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue = pulseIn(signal, HIGH);
  
  // Map values - CALIBRATE THESE!
  red = map(red, 80, 30, 0, 255);
  green = map(green, 80, 30, 0, 255);
  blue = map(blue, 80, 30, 0, 255);
  
  // Constrain to valid range
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);
}

// ========================================
// MOTOR CONTROL FUNCTIONS
// ========================================

void navigateInward() {
  // Move forward toward center
  moveForward(SPEED_NORMAL);
}

void handleWhiteSpace() {
  // Between rings - continue moving forward
  moveForward(SPEED_NORMAL);
}

void slowSearch() {
  // Lost color - slowly turn to find it
  turnRight(SPEED_SLOW);
}

void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
  
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speed);
}

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void reachedCenter() {
  stopRobot();
  
  // Stop program - ready for code reupload for shooting
  while(1);
}
