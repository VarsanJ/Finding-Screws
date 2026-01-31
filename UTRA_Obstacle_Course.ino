// Constants for the array
char direction[] = {'L', 'R'}; // L is CW, R is CCW, this is designed based off the track route
const int rotation_v = 120, base_v = 200;
cnt = 0;


void rotateToNewRoad(){
  const int const_90_time = 100;
  if (direction[cnt]=='L') {
    setRotateCCW();
    delay(const_90_time);
    stopMotors();
    cnt++;
  }
  else if (direction[cnt]=='R') {
    setRotateCW();
    delay(const_90_time);
    stopMotors();
    cnt++;
  }
}

bool isRed()
{
  // Colour Code
  unsigned long colour;

  // Return
  if colour == "Red" return true;
  return false; 
}

void moveForward() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(leftMotorSpeed, base_v);
  analogWrite(rightMotorSpeed, base_v);
}

void stopMotors() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(leftMotorSpeed, 0);
  analogWrite(rightMotorSpeed, 0);
}

void setrotateCW(int duration) {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  analogWrite(leftMotorSpeed, rotation_v);
  analogWrite(rightMotorSpeed, rotation_v);
}

void setrotateCCW(int duration) {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(leftMotorSpeed, rotation_v);
  analogWrite(rightMotorSpeed, rotation_v);
}

void avoidObstacle(){
  // PURPOSE: Code to go around the obstacle
  
  const int delayForward = 1;  
  const int r_duration = 0; // How long a rotation of 90 degrees will be
  setrotateCCW();
  delay(r_duration);
  stopMotors();
  
  moveForward();
  delay(delayForward);
  stopMotors(); 
  
  setrotateCW();
  delay(delayForward); 
  stopMotors();
  
  setrotateCW();
  delay(delayForward); 
  stopMotors();

  while (isRed() == false){
    moveForward();
  }
  
  delay(500);

  setrotateCCW();
  delay(r_duration);
  stopMotors();
  
}


void setup(){
  Serial.begin(9600);
  
  // Initialize TCS230 color sensor pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Set TCS230 frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize motor pins
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorSpeed, OUTPUT);
  pinMode(rightMotorSpeed, OUTPUT);

  
}

void loop(){
  boolean obstacle = false;
  if (obstacle){
    stopMotors();
    avoidObstacle();
  } 
  else if (isRed() == false) {
    stopMotors();
    rotateToNewRoad();
  }
  else moveForward();
}



