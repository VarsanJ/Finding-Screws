/*
  This is the code to move the car forward and backward
 */
int leftWheel = 13;
int rightWheel = 12;

void setup() {
  pinMode(leftWheel, OUTPUT);
  pinMode(rightWheel, OUTPUT);
}

void loop() {
  digitalWrite(leftWheel, HIGH);
  digitalWrite(rightWheel, HIGH);

  delay(1000);

  digitalWrite(leftWheel, LOW);
  digitalWrite(rightWheel, LOW);
    
}
