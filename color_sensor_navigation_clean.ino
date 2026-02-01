// TCS3200 Color Sensor Pins
const int S0 = 11;
const int S1 = 12;
const int S2 = 2;
const int S3 = 3;
const int signal = 4;


// Variables for color readings
unsigned long red, green, blue, clear;

// Detected color and navigation state
String currentColor = "UNKNOWN";
String previousColor = "UNKNOWN";
int ringProgress = 0; // 0=not started, 1=past blue, 2=past red, 3=at center

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(signal, INPUT);
  
  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void loop() {
  readColor();
  
  // Track progress through rings
  if (currentColor == "BLUE" && ringProgress == 0) {
    ringProgress = 1;
  }
  else if (currentColor == "RED" && ringProgress >= 1) {
    ringProgress = 2;
  }
  else if (currentColor == "BLACK" && ringProgress >= 2) {
    ringProgress = 3;
    reachedCenter();
  }
  
  // Navigation logic
  if (currentColor == "BLACK") {
    reachedCenter();
  }
  else if (currentColor == "BLUE") {
    navigateInward();
  }
  else if (currentColor == "RED") {
    navigateInward();
  }
  else if (currentColor == "WHITE") {
    handleWhiteSpace();
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
  
  // Store previous color
  previousColor = currentColor;
  
  // Identify current color
  currentColor = identifyColor(red, green, blue);
}

String identifyColor(int r, int g, int b) {
  // BLACK (center target) - all values very low
  if (r < 50 && g < 50 && b < 50) {
    return "BLACK";
  }
  
  // WHITE (between rings or background) - all values high
  if (r > 180 && g > 180 && b > 180) {
    return "WHITE";
  }
  
  // RED ring - high red, low green and blue
  if (r > 150 && g < 100 && b < 100 && r > g && r > b) {
    return "RED";
  }
  
  // BLUE ring - high blue, low red and green
  if (r < 100 && g < 100 && b > 150 && b > r && b > g) {
    return "BLUE";
  }
  
  return "UNKNOWN";
}

void navigateInward() {
  // TODO: Add motor control
  // Move toward center of target
  // Example:
  // moveForward();
}

void handleWhiteSpace() {
  // TODO: Add motor control
  // Continue moving inward or adjust course
  // Example:
  // moveForward();
}

void reachedCenter() {
  stopRobot();
  
  // Stop program - ready for code reupload for shooting
  while(1);
}

void stopRobot() {
  // TODO: Add motor stop code
  // Example:
  // digitalWrite(motorLeft, LOW);
  // digitalWrite(motorRight, LOW);
  // analogWrite(motorSpeed, 0);
}
