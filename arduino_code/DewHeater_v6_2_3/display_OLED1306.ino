// functions to display on the 128 x 32 OLED display - SSD1306 
// set as (col,row) = 6,8 per char
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Basic display stuff

#ifdef OLED1306

void displaySetUp(){
  // initialize display
  display.begin(&Adafruit128x32,display_address);
  display.setFont(Adafruit5x7);
}

void clearDisplay() {
  display.clear();  
}

void displayVersion(){
  clearDisplay();
  displayText(DH_start,0,0,0);
  displayText(DH_version,11,0,0);
  delay(displayDelay);
  displayText(DH_modes,0,1,0);
  delay(displayDelay);
  displayText(DH_control,0,2,0);
  delay(displayDelay);
  displayText(DH_display,0,3,0);
  delay(displayDelay*2);
}

void setDisplayPosition(int theCol, int theRow) {
  display.setCursor(theCol*uCol,theRow*uRow);
}

void displayText(String theText, int theCol, int theRow, int invertText) {
  // text to display , @column, @row, normal/invert text = 0/1
  if ((theCol == 0) && (theRow == 0)) clearDisplay();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) display.setCursor(theCol*uCol,theRow*uRow);
  display.setInvertMode(invertText);                // invert if needed
  display.print(theText);
  display.setInvertMode(0);                         // return to text colour if inverted
}

void displayNumber(int theNumber, int theCol, int theRow, int invertText) {
  // number to display , @column, @row, normal/inverted colour (1/0)
  if ((theCol == 0) && (theRow == 0)) clearDisplay();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) display.setCursor(theCol*uCol,theRow*uRow);
  display.setInvertMode(invertText);                // invert if needed
  display.print(theNumber);
  display.setInvertMode(0);                         // return to text colour if inverted
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Display ambient, sensor and heater data

void displayAmbientData() {
  displayText("Ambient",0,0,0);
  displayText("Temp",0,1,0);
  displayText("Humid",6,1,0);
  displayText("DewPt",13,1,0);
  
  if (errorAmbientSensor) {
    // error reading ambient sensor
    displayText("--",0,2,0);
    displayText("--",6,2,0);
    displayText("--",13,2,0);
  }
  else {
    // if ambient read okay display AM2320 data
    displayNumber(temperatureAmbient,0,2,0);
    displayText("C",-1,-1,0);
    displayNumber(humidityAmbient,6,2,0);
    displayText("%",-1,-1,0);
    displayNumber(dewPointAmbient,13,2,0);
    displayText("C",-1,-1,0);
  }
}

void displayDS18B20data( int currChan ) {
  int aboveDewPoint;  // convert data to integer for display

  // display the heater number and,if sensor connected,its temperature
  displayText("H",0,currChan,0);
  displayNumber((currChan+1),-1,-1,0);

  // the heater temp - if not a temp read error
  if(!errorDS18B20[currChan]) {
    displayNumber(tempHeater[currChan],3,currChan,0);
    displayText("C",-1,-1,0);
  }
  else { //error temp read
    displayText("----",3,currChan,0);
  }  
}  

void displayHeaterOutput( int currChan ) {
  float thePerCentDutyCycle;
  // display the % power (duty cycle)
  displayNumber(outputPower[ currChan ],8,currChan,0);
  displayText("%",-1,-1,0);
  // and the mode - reverse the colour if has defaulted from auto to manual or OFF
  if (globalMode == chanCycleMode[currChan]) {
    // current channel mode same as global
    displayText(heaterModeString[chanCycleMode[currChan]],13,currChan,0);
  }
  else {
    // current channel mode NOT same as global: defaulted due to sensor read error
    displayText(heaterModeString[chanCycleMode[currChan]],13,currChan,1);
  }
}

#endif