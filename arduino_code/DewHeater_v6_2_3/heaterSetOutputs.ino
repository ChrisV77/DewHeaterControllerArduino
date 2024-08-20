/* 
 *  These functions:
 * - set up PID etc
 * - setHeaterData    = sets the power levels
 * - outputHeaterData = writes to output pins
 */

// set up heater output pins
void setupHeaterOutputPins() {
  // setup heater output pins
  for (int currChan=0; currChan < numChannels; currChan++) {
    pinMode(heaterPin[currChan], OUTPUT);                   // set the heater output pin as output
  }
}

// flash heater output LEDS
void flashAllHeaterOutputs(int number_times) {
  int displayPowerLevel = 50;                                 // % power level for display

  // flash all outputs at startup - number times = number channels
  for (int i=0; i < number_times; i++) {
    for (int currChan=0; currChan < number_times; currChan++) {
      outputPower[currChan] = displayPowerLevel;              // flash power LED
      outputToHeater(currChan, outputPower[currChan]);        // write power to outputs
    }
    delay(displayDelay/2);
    for (int currChan=0; currChan < number_times; currChan++) {
      outputPower[currChan] = 0;                              // initially 0%
      outputToHeater(currChan, outputPower[currChan]);        // write 0% power to outputs
    }
    delay(displayDelay/2);
  }
}

void setUpPIDcontrol() {
  for (int currChan=0; currChan < numChannels; currChan++) {
    PID_control[currChan].SetMode(AUTOMATIC);
    PID_control[currChan].SetOutputLimits(0, 100);                     // output limit 0-100%
    PID_control[currChan].SetTunings(Kp, Ki, Kd);             // update K-params for PID
  }
}

// calc heater value - for mode 2 = auto-ambient
int calcOutputAutoAmbientMode(int theChan) {
  float belowAmbientTemp;
  float belowAmbientTempConstrain;
  int thePowerPercent;

  // calculate how far ambient temp is above dew point & constrain the maximum to the threshold value (this is where heater will turn on)
  belowAmbientTemp = temperatureAmbient - dewPointAmbient;
  belowAmbientTempConstrain = constrain( belowAmbientTemp , 0 , autoAmbientThreshold );
  // map (0,aboveDewPointConstrain) to (max power,0)- range = 0% @ aboveDewPoint>=threshold & maxPowerAutoAmbient% @ aboveDewPoint=0
  thePowerPercent = map((belowAmbientTempConstrain*10), 0, (autoAmbientThreshold*10), autoAmbientMaxPower, 0);      // NB temp values *10 for an integer function (so 15.6C=156)
  return thePowerPercent;
}

// calc heater value - for mode 3 = auto-heater (PID control)
int calcOutputAutoHeaterModePID(int theChan) {
  int theSetPoint;

  // basic setPoint calculation. Sets it to
  // - dewpoint + the target value above dew point. OR ambient temperature. Whichever larger
  setPoint[theChan] = max( (dewPointAmbient + autoHeaterTargetTemp), temperatureAmbient );
  PID_control[theChan].Compute();
  return PIDout[theChan];
}

// set the heater output pin value
void outputToHeater(int theChan, int thePowerPercentLevel) {
  int thePWMlevel;

  // map the 0-100% power to 0-255 PWM level for digital pins
  thePWMlevel = map(thePowerPercentLevel, 0, 100, 0, 255);
  // write the heater's duty cycle to the heater
  analogWrite(heaterPin[theChan] , thePWMlevel);
}

// OPTIONAL. flash each heater output mode xtimes
void flashModeHeater() {
  int theMode;
  int theChan;
  int displayPowerLevel = 50;                                 // % power level for display

  // Output the mode - flash mode number of times: 1=off 2=manual 3=auto-amb 4=auto-htr
  for (theChan=0; theChan < numChannels; theChan++) outputToHeater(theChan,0);
  delay(displayDelay/2);
  for (theMode=0; theMode < numberModes; theMode++) {
    for (theChan=0; theChan < numChannels; theChan++) {
      if (chanCycleMode[theChan] >= theMode) outputToHeater(theChan,displayPowerLevel);
    }
    delay(displayDelay/2);
    for (theChan=0; theChan < numChannels; theChan++) outputToHeater(theChan,0);
    delay(displayDelay/2);
  }
  // turn back on to current power
  for (int theChan=0; theChan < numChannels; theChan++) outputToHeater(theChan,outputPower[theChan]);
}
\

void setHeaterModePower(int theChan) {
  int theMode;

  // (1) change the channel mode from global mode if errors or above cut-off
  // - check for read errors in AUTO modes 2/3, and change 'theMode' if needed. Error due to some malfunction or no sensor connected
  theMode = globalMode;
  switch (theMode) {
    case 3: // MODE 3: AUTO-heater
      if (errorDS18B20[theChan] ) theMode = 2;      // if error heater sensor read, change to mode 2 (auto-ambient)
      if (errorAmbientSensor) theMode = 1;          // if error ambient read change mode to 1 (manual)
      break;
    case 2: // MODE 2 : Auto-ambient
      if (errorAmbientSensor) theMode = 1;          // if error ambient read change mode to 1 (manual)
      break;
  }
  // - check if heater temperature above cutoff & set 'theMode' to OFF (in anymode). 
  //    - can work in any mode. BUT only works if there's a DS18B20 heater sensor
  if (!errorDS18B20[theChan]) {
    if (tempHeater[theChan] > tempCutOff) theMode = 0;
  }

  // (2) set the cycle modes and output power (0-100%)
  switch (theMode) {
    case 3: // MODE 3: AUTO Dewpoint+HeaterTemp PID mode: set heater duty cycle to diff between heater element temp and dew point - using PID
      chanCycleMode[theChan] = 3;
      outputPower[theChan] = calcOutputAutoHeaterModePID(theChan);
      break;

    case 2: // MODE 2 : Auto DEWPOINT-ambient: set heater duty cycle to to diff between ambient temp and dew point
      chanCycleMode[theChan] = 2;
      outputPower[theChan] = calcOutputAutoAmbientMode(theChan);
      break;

    case 1: // MODE 1: MANUAL mode: set heater duty cycle to manual level
      // convert heater input range 0-10 TO output duty cycle range 0-255
      chanCycleMode[theChan] = 1;
      outputPower[theChan] = manualPower;
      break;

    case 0: // MODE 0: Off mode: set heater duty cycle to 0
      chanCycleMode[theChan] = 0;
      outputPower[theChan] = 0;
      break;
  }
}
