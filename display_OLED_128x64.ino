// here's all the functions to display on the 128 x 64 OLED display

#ifdef OLED_ON
void clearDisplay() {
  display.clearDisplay();
  display.display();
  delay(10);
}

void setDisplayPosition(int theCol, int theRow) {
  display.setCursor(theCol,theRow);
  display.display();
  delay(10);
}

void displayText(char* theText, int theCol, int theRow) {
  if ((theCol == 0) && (theRow == 0)) display.clearDisplay();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) display.setCursor(theCol,theRow);
  display.print(theText);
  display.display();
}

void displayNumber(int theNumber, int theCol, int theRow) {
  if ((theCol == 0) && (theRow == 0)) display.clearDisplay();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) display.setCursor(theCol,theRow);
  display.print(theNumber);
  display.display();
}

void displaySetUp(){
  // initialize display
  // with the 128*64 SSD1306 display - I2C addr 0x3C (not 3D)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();
  delay(theDelay);
}

void displayAmbientData() {
  displayText("Temp Humid DewPt", 0, 0);
  
  if (errorAmbientSensor) {
    // error reading ambient sensor
    displayText("am2320 error", 0, 1);
  }
  else {
    // if ambient read okay display AM2320 data
    displayNumber(temperatureAmbient, 0, 8);
    displayText("\337C", -1, -1);
    displayNumber(humidityAmbient, 32, 8);
    displayText("%", -1, -1);
    displayNumber(dewPointAmbient, 64, 8);
    displayText("\337C", -1, -1);
  }
}

void promptModeSelect() {
  displayText("Release button", 0, 0);
  displayText("to select mode:", 0, 8);
  delay(theDelay*2);
  clearDisplay();
}

void displayModeSelect() {
  displayText("Mode:", 0, 0);
  if (heaterMode == 0) displayText("Off   ", 0, 8);
  if (heaterMode == 1) displayText("Man   ", 0, 8);
  if (heaterMode == 2) displayText("Auto  ", 0, 8);
  if (heaterMode == 3) displayText("Reset ", 0, 8);
}

void displayModeSelected() {
  displayText("Selected", -1, -1);
  delay(theDelay);
}

void promptManPwrLevel() {
  displayText("Press to select", 0, 0);
  displayText("Man pwr level", 0, 8);
  delay(theDelay);
}

void displayManPwrLevel() {
  displayText("Manual level: ", 0, 0);     
  displayNumber(int(heaterManualPower*10), -1, -1);
  displayText("%   ", -1, -1);
}

void displaySelectedManPwrLevel() {
  displayText("selected", 0, 8);
  displayText("...", 0, 16);
}

void displayDS18B20data( int theChannel ) {
  int aboveDewPoint;  // convert data to integer for display

  // display the heater number and, if sensor connected, its temperature
  displayText("Heater", 0, 0);
  displayNumber((theChannel+1), -1, -1);

  // the heater temp - if not a temp read error
  if(errorDS18B20[theChannel]) {
    displayText("noSensor", 64, 0);
  }
  else {
    displayNumber(tempSensor[theChannel], 64, 0);
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
  displayText("Pwr= ", 0, 8);
  displayNumber(int(thePerCentDutyCycle), -1, -1);
  displayText("%", -1, -1);

  // MANUAL mode or heater sensor error
  if (heaterMode == 1) { 
    displayText("MAN", 64, 8);
  }

  // AUTO mode
  if(heaterMode == 2) {
    if(errorDS18B20[theChannel]) {
      displayText("MAN", 64, 8);
    }
    else {
      displayText("AUTO", 64, 8);
    }
  }

  if (tempSensor[theChannel] >= tempCutOff) {
    // if above cut-off
    displayText("CUTOFF", 64, 8);
  }
}
#endif
