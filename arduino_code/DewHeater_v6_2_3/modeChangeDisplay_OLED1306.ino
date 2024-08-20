// ---------------------------------------------------------------------------------------------------------------------------------------------------
// display stuff for mode changing - using OLED
// not currently being used

#ifdef MODEBUTTON
#ifdef OLED1306

void promptModeSelect() {
  delay(displayDelay);
  clearDisplay();
  displayText("Press button",0,0,0);
  displayText("to select mode:",0,1,0);
  delay(displayDelay*2);
}

void displayModeSelect() {
//  display.setTextColor(1,0);      // back to normal colour
  displayText("Mode = ",0,2,0);
  if (globalMode == 0) displayText("Off     ",7,2,0);
  if (globalMode == 1) displayText("Manual  ",7,2,0);
  if (globalMode == 2) displayText("Auto-Amb",7,2,0);
  if (globalMode == 3) displayText("Auto-Htr",7,2,0);
}

void displayModeSelected() {
  displayText("Selected",0,3,0);
  delay(displayDelay);
}

void promptManPwrLevel() {
  clearDisplay();
  displayText("Press to select",0,0,0);
  displayText("Man pwr level",0,1,0);
  delay(displayDelay);
}

void displayManPwrLevel() {
  displayNumber(manualPower,0,2,0);
  displayText("%   ",-1,-1,0);
}

void displaySelectedManPwrLevel() {
  displayText("selected",6,2,0);
}
#endif
#endif
