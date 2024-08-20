// ---------------------------------------------------------------------------------------------------------------------------------------------------
// display stuff for mode changing - using OLED
// not currently being used

#ifdef MODEBUTTON   // compile only if using MODEBUTTON and have an LCD display
#ifdef LCD_ON

void promptModeSelect() {
  delay(displayDelay);
  displayText("Press button    ",0,0,0);
  displayText("to select mode: ",0,1,0);
  delay(displayDelay*2);
}

void displayModeSelect() {
  int displayRow;

  if (numberRow == 2) displayRow = 0;     // only 2 lines available for 1602/4 displays
  else displayRow = 2;                    // allow 4 lines if 2004 display
  displayText("Mode:",0,displayRow,0);
  if (globalMode == 0) displayText("Off   ",6,displayRow,0);
  if (globalMode == 1) displayText("Man   ",6,displayRow,0);
  if (globalMode == 2) displayText("A-Amb ",6,displayRow,0);
  if (globalMode == 3) displayText("A-Htr ",6,displayRow,0);
}

void displayModeSelected() {
  int displayRow;

  if (numberRow == 2) displayRow = 1;     // only 2 lines available for 1602/4 displays
  else displayRow = 3;                    // allow 4 lines if 2004 display
  displayText("Selected",0,displayRow,0);
  delay(displayDelay);
}

void promptManPwrLevel() {
  int displayRow;

  displayText("Press to select ",0,0,0);
  delay(displayDelay);
  if (numberRow == 2) displayRow = 0;     // only 2 lines available for 1602/4 displays
  else displayRow = 1;                    // allow 4 lines if 2004 display
  displayText("Manual pwr level",0,displayRow,0);
  delay(displayDelay);
}

void displayManPwrLevel() {
  int displayRow;

  if (numberRow == 2) displayRow = 1;     // only 2 lines available for 1602/4 displays
  else displayRow = 2;                    // allow 4 lines if 2004 display
  displayNumber(manualPower,0,displayRow,0);
  displayText("%   ",-1,-1,0);
}

void displaySelectedManPwrLevel() {
  int displayRow;

  if (numberRow == 2) displayRow = 1;     // only 2 lines available for 1602/4 displays
  else displayRow = 2;                    // allow 4 lines if 2004 display
  displayText("selected",6,displayRow,0);
}
#endif
#endif
