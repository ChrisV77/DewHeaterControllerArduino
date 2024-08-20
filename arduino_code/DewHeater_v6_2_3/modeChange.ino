// change control mode = OFF - MANUAL - AUTO
// change the manual level - for channels in AUTO mode without sensors, and for MANUAL mode
// here's all the functions to display on the 16x2 LCD

#ifdef MODEBUTTON

boolean checkModeButtonPress() {
  boolean modeButtonPressed;
  int priorMode;

  priorMode = globalMode;
  modeButtonPressed = false;
  // If controlSwitchPin is pressed = LOW, cycle through modes (& display them)
  // release button to select
  if (digitalRead(controlSwitchPin) == LOW) {
    // if mode button pressed, display prompt
    modeButtonPressed = true;
    delay(btnDelay);
  }
  // if button pressed
  if ( modeButtonPressed == true) {
    // wait for button release
    while (digitalRead(controlSwitchPin) == LOW) {
      delay(btnDelay);
    }
    clearDisplay();
    promptModeSelect();
    delay(displayDelay);
    globalMode -= 1; // go to mode-1 (as doing mode+1 in next loop)
    // button released, now wait for button press
    do {
      // cycle between modes until mode button pressed
      globalMode += 1;
      if (globalMode > 3) globalMode = 0;
      displayModeSelect();
      delay(displayDelay*2);
    } while (digitalRead(controlSwitchPin) == HIGH);
    delay(btnDelay);
  //  if (globalMode != priorMode) modeChanged = true;
  }
  return modeButtonPressed;
}

void changeTheMode() {
  displayModeSelected();          // display that new mode selected
  // if OFF mode = 0
  if (globalMode == 0) {
    manualPower = 0;        // set heater to 0, will stay 0 if mode=OFF
  }

  // if not OFF - need to set manual power: for MAN mode and AUTO modes if fail read sensor
  if ( globalMode != 0 ) {
    // display prompt
    promptManPwrLevel();
    manualPower = -10; // start as -1, as DO..LOOP starts with incr +1
    do {
      // cycle power levels
      manualPower += 10;
      if (manualPower > 100) manualPower = 0;
      // display the manual heater power
      displayManPwrLevel();
      delay(displayDelay*2);
    } while (digitalRead(controlSwitchPin) == HIGH);
    displaySelectedManPwrLevel();
  }
  while (digitalRead(controlSwitchPin) == LOW) {
    delay(btnDelay);      //debounce
  }
  // save the new parameters
  saveEEPROMparams();
}
#endif
