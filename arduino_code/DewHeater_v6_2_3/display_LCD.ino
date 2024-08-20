// functions to display on the 16 x 2 LCD display - using HD44780 library 
// set as (col,row) = 1,1 per char
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Basic display stuff

#ifdef LCD_ON

void displaySetUp(){
  // initialize display
  lcd.begin(numberCol,numberRow);
  lcd.begin(20,4);
  lcd.backlight();
}

void clearDisplay() {
  lcd.clear();  
}

void displayVersion(){
  int displayRow;

  displayText(DH_start,0,0,0);
  displayText(DH_version,11,0,0);
  delay(displayDelay);
  displayText(DH_modes,0,1,0);
  delay(displayDelay*2);
  if (numberRow == 2) displayRow = 0;     // only 2 lines available for 1602/4 displays
  else displayRow = 2;                    // allow 4 lines if 2004 display
  displayText(DH_control,0,displayRow,0);
  delay(displayDelay*2);
  if (numberRow == 2) displayRow = 1;     // only 2 lines available for 1602/4 displays
  else displayRow = 3;                    // allow 4 lines if 2004 display
  displayText(DH_display,0,displayRow,0);
  if (numberRow == 2) {
    delay(displayDelay*2);
    displayText(DH_start,0,0,0);
    displayText(DH_version,11,0,0);
  }
}

void setDisplayPosition(int theCol, int theRow) {
  lcd.setCursor(theCol*uCol,theRow*uRow);
}

void displayText(String theText, int theCol, int theRow, int invertText) {
  // text to display , @column, @row, normal/invert text = 0/1
  if ((theCol == 0) && (theRow == 0)) clearDisplay();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) lcd.setCursor(theCol*uCol,theRow*uRow);
  // lcd.setInvertMode(invertText);                // invert if needed
  lcd.print(theText);
  // lcd.setInvertMode(0);                         // return to text colour if inverted
}

void displayNumber(int theNumber, int theCol, int theRow, int invertText) {
  // number to display , @column, @row, normal/inverted colour (1/0)
  if ((theCol == 0) && (theRow == 0)) clearDisplay();
  // if theRow && theCol == -1, then don't change the cursor position
  if ((theCol >= 0) && (theRow >= 0)) lcd.setCursor(theCol*uCol,theRow*uRow);
  // lcd.setInvertMode(invertText);                // invert if needed
  lcd.print(theNumber);
  // lcd.setInvertMode(0);                         // return to text colour if inverted
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Display ambient, sensor and heater data


void displayAmbientData() {
  // displayText("Ambient",0,0,0);
  displayText("Temp",0,0,0);
  displayText("Humid",5,0,0);
  displayText("DewPt",11,0,0);
  
  if (errorAmbientSensor) {
    // error reading ambient sensor
    displayText("--",0,1,0);
    displayText("--",5,1,0);
    displayText("--",11,1,0);
  }
  else {
    // if ambient read okay display AM2320 data
    displayNumber(temperatureAmbient,0,1,0);
    displayText("C",-1,-1,0);
    displayNumber(humidityAmbient,5,1,0);
    displayText("%",-1,-1,0);
    displayNumber(dewPointAmbient,11,1,0);
    displayText("C",-1,-1,0);
  }
}

void displayDS18B20data(int currChan) {
  int aboveDewPoint;  // convert data to integer for display
  int displayRow;

  if (numberRow == 2) displayRow = currChan % 2;      // only 2 lines available for 1602/4 displays
  else displayRow = currChan;                         // allow 4 lines if 2004 display

  // display the heater number and,if sensor connected,its temperature
  displayNumber((currChan+1),0,displayRow,0);

  // the heater temp - if not a temp read error
  if(!errorDS18B20[currChan]) {
    displayNumber(tempHeater[currChan],2,displayRow,0);
    displayText("C",-1,-1,0);
  }
  else { //error temp read
    displayText("--",2,displayRow,0);
  }  
}  

void displayHeaterOutput(int currChan) {
  float thePerCentDutyCycle;
  int displayRow;

  if (numberRow == 2) displayRow = currChan % 2;      // only 2 lines available for 1602/4 displays
  else displayRow = currChan;                         // allow 4 lines if 2004 display

  // display the % power (duty cycle)
  displayNumber(outputPower[ currChan ],7,displayRow,0);
  displayText("%",-1,-1,0);
  // and the mode - reverse the colour if has defaulted from auto to manual or OFF
  if (globalMode == chanCycleMode[currChan]) {
    // current channel mode same as global
    displayText(heaterModeString[chanCycleMode[currChan]],11,displayRow,0);
  }
  else {
    // current channel mode NOT same as global: defaulted due to sensor read error
    displayText(heaterModeString[chanCycleMode[currChan]],11,displayRow,1);
  }
  if ((numberRow == 2) && (displayRow == 1)) delay(displayDelay*2);        // add a delay - need if more than 2x chans for 1602/4 displays
}

#endif