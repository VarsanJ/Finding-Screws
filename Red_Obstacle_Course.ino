void runRedStartSequence() {
  // STEP 1: Move forward from black start line
  moveRaw(150, 150);
  delay(TIME_FOR_10CM);
  
  // STEP 2: Turn LEFT 90° to face red path
  moveRaw(-150, 150);
  delay(TIME_TURN_90);
  
  // STEP 3: Drive forward until RED detected
  moveRaw(150, 150);
  while (detectColor() != "RED") {
    delay(10);
  }
  
  // STEP 4: Align with red line
  moveRaw(-150, 150);
  while (digitalRead(RIGHT_IR) == 0) {
    delay(10);
  }
  stopMotors();
}
