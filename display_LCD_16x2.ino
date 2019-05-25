// here's all the functions to display on the 16x2 LCD

#ifdef LCD_ON
void clearDisplay() {
  lcd.clear();
  delay(10);
}

void setDisplayPosition(int theCol, int theRow) {
  lcd.setCursor(theCol,theRow);
  delay(10);
}

void displayText(char* theText, int theCol, int theRow) {
  if ((theCol == 0) && (theRow == 0)) lcd.clear();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) lcd.setCursor(theCol,theRow);
  lcd.print(theText);
}

void displayNumber(int theNumber, int theCol, int theRow) {
  if ((theCol == 0) && (theRow == 0)) lcd.clear();

  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) lcd.setCursor(theCol,theRow);
  lcd.print(theNumber);
}

void displaySetUp(){
  // initialize & set up LCD display
  lcd.init();
  lcd.begin(16,2);
  lcd.backlight();   // flash the backlight, then leave ON
  delay(theDelay/2);
  lcd.noBacklight(); 
  delay(theDelay);
  lcd.backlight(); 
}

void displayAmbientData() {
  displayText("Temp Humid DewPt", 0, 0);
  
  if (errorAmbientSensor) {
    // error reading ambient sensor
    displayText("am2320 error", 0, 1);
  }
  else {
    // if ambient read okay display AM2320 data
    displayNumber(temperatureAmbient, 0, 1);
    displayText("\337C", -1, -1);
    displayNumber(humidityAmbient, 5, 1);
    displayText("%", -1, -1);
    displayNumber(dewPointAmbient, 11, 1);
    displayText("\337C", -1, -1);
  }
}

void promptModeSelect() {
  displayText("Release button", 0, 0);
  displayText("to select mode:", 0, 1);
  delay(theDelay*2);
  clearDisplay();
}

void displayModeSelect() {
  displayText("Mode:", 0, 0);
  if (heaterMode == 0) displayText("Off   ", 0, 1);
  if (heaterMode == 1) displayText("Man   ", 0, 1);
  if (heaterMode == 2) displayText("Auto  ", 0, 1);
  if (heaterMode == 3) displayText("Reset ", 0, 1);
}

void displayModeSelected() {
  displayText("Selected", -1, -1);
  delay(theDelay);
}

void promptManPwrLevel() {
  displayText("Press to select", 0, 0);
  displayText("Man pwr level", 0, 1);
  delay(theDelay);
  displayText("Manual level:", 0, 0);     
}

void displayManPwrLevel() {
  displayNumber(int(heaterManualPower*10), 0, 1);
  displayText("%   ", -1, -1);
}

void displaySelectedManPwrLevel() {
  displayText("selected", 5, 1);
}

void displayDS18B20data( int theChannel ) {
  int aboveDewPoint;  // convert data to integer for display

  // display the heater number and, if sensor connected, its temperature
  displayText("Heater", 0, 0);
  displayNumber((theChannel+1), -1, -1);

  // the heater temp - if not a temp read error
  if(errorDS18B20[theChannel]) {
    displayText("noSensor", 8, 0);
  }
  else {
    displayNumber(tempSensor[theChannel], 8, 0);
    aboveDewPoint = tempSensor[theChannel] - dewPointAmbient;
    if (aboveDewPoint > 0) displayText("\337C +", -1, -1);
    if (aboveDewPoint == 0) displayText("\337C @", -1, -1);
    if (aboveDewPoint < 0) displayText("\337C -", -1, -1);
    displayNumber(abs(aboveDewPoint), -1, -1);
  }  
}  

void displayHeaterOutput( int theChannel ) {
  float thePerCentDutyCycle;
  // display the % power (duty cycle)
  thePerCentDutyCycle = 100 * heaterDutyCycle[ theChannel ] / 255;
  displayText("Pwr= ", 0, 1);
  displayNumber(int(thePerCentDutyCycle), -1, -1);
  displayText("%", -1, -1);

  // MANUAL mode or heater sensor error
  if (heaterMode == 1) { 
    displayText("MAN", 10, 1);
  }

  // AUTO mode
  if(heaterMode == 2) {
    if(errorDS18B20[theChannel]) {
      displayText("MAN", 10, 1);
    }
    else {
      displayText("AUTO", 10, 1);
    }
  }

  if (tempSensor[theChannel] >= tempCutOff) {
    // if above cut-off
    displayText("CUTOFF", 10, 1);
  }
}
#endif
