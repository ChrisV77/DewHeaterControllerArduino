// change control mode = OFF - MANUAL - AUTO
// change the manual level - for channels in AUTO mode without sensors, and for MANUAL mode

boolean checkForModeChange() {
  boolean changeMode;
  int priorMode;

  priorMode = heaterMode;
  changeMode = false;
  // If controlSwitchPin is pressed = LOW
  // Cycle through modes = OFF-MANUAL-AUTOMATIC-RESET
  // release button to select
  if (digitalRead(controlSwitchPin) == LOW) {
    // if mode button pressed, display prompt && changeMode=true
    promptModeSelect();
    changeMode = true;
  }
  while (digitalRead(controlSwitchPin) == LOW) {
    // and cycle between modes while mode button pressed
    heaterMode += 1;
    if (heaterMode > 3) heaterMode = 0;
    displayModeSelect();
    delay(theDelay*2);
  }
  return changeMode;
}

void changeTheMode() {
  displayModeSelected();          // display that new mode selected
  // if OFF mode = 0
  if (heaterMode == 0) {
    heaterManualPower = 0;        // set heater to 0, will stay 0 if mode=OFF
  }

  // if AUTO or MAN mode = 1,2
  if ((heaterMode == 1) || (heaterMode == 2)) {
    // display prompt
    promptManPwrLevel();
    heaterManualPower = -1; // start as -1, as DO..LOOP starts with incr +1
    do {
      // cycle power levels
      heaterManualPower += 1;
      if (heaterManualPower > 10) heaterManualPower = 0;
      // display the manual heater power
      displayManPwrLevel();
      delay(theDelay);
    } while (digitalRead(controlSwitchPin) == HIGH);
    displaySelectedManPwrLevel();
  }
  while (digitalRead(controlSwitchPin) == LOW) {
    delay(20);      //debounce
  }

  // if RESET mode = 3
  if (heaterMode == 3) {
    resetAmbientSensor();
    resetDS18B20();
    heaterMode = defaultHeaterMode;
    heaterManualPower = defaultHeaterManualPower;
  }

  delay(theDelay*2);
}
