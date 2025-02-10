/*
  Save to & write params from EEPROM to store between power cycles
  The format of saved EEPROM data is:
  Byte 0:   mode
  Byte 2:   manual power
  Byte 4:   temp target - auto-heater mode
  Byte 6:   temp Cut-Off
  Byte 8:   max power - auto-ambient mode
  Byte 10:  temp threshold - autoambient mode
*/

// read dew heater params from EEPROM
void getEEPROMparams() {
  unsigned int eepromAddress;   // counter for EEPROM address
  int numBytes = 2;             // 2 bytes per unsigned integer value stored on EEPROM for Arduino UNO

  eepromAddress = 0;
  EEPROM.get( eepromAddress , globalMode );           // global mode of operation 1=off 2=manual 3=auto-ambient 4=auto-ambient+heater sensor. NB: channels can default when running
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , manualPower );          // manual power level
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , aAmbMaxPwr );           // max power (for auto-ambient mode)
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , aAmbThresh );           // temp threshold (for ambient mode)
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , aHtrTargetTemp );       // target heater temp - currently temp above ambient
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , tempCutOff );           // heater temp at which heater powers cuts off - to prevent over heating

  msgEEPROM = "EEPROM data OK";
  if (checkErrorEEPROMdata()) {
    saveEEPROMparams();
    msgEEPROM = "Err: rst EEPROM";
  }
}

// save dew heater params to EEPROM
void saveEEPROMparams() {
  unsigned int eepromAddress;  // counter for EEPROM address
  int numBytes = 2;            // 2 bytes per unsigned integer value stored on EEPROM for Arduino UNO

  eepromAddress = 0;
  EEPROM.put( eepromAddress , globalMode );           // global mode of operation 1=off 2=manual 3=auto-ambient 4=auto-ambient+heater sensor. NB: channels can default when running
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , manualPower );          // manual power level
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , aAmbMaxPwr );           // max power (for auto-ambient mode)
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , aAmbThresh );           // temp threshold (for ambient mode)
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , aHtrTargetTemp );       // target heater temp - currently temp above ambient
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , tempCutOff );           // heater temp at which heater powers cuts off - to prevent over heating
}

bool checkInRange(int val, int minval, int maxval) {
  return ((val >= minval) && (val <= maxval));
}

bool checkErrorEEPROMdata() {
  bool errorEEPROMdata = false;

  // check EEPROM value within allowed/reasonable range
  if (!checkInRange( globalMode, globalModeRange[0], globalModeRange[1])) {     
    globalMode = globalModeRange[2];
    errorEEPROMdata = true;
  }
  if (!checkInRange( manualPower, manualPowerRange[0], manualPowerRange[1])) {
    manualPower = manualPowerRange[2];
    errorEEPROMdata = true;
  }
  if (!checkInRange( aAmbMaxPwr, aAmbMaxPwrRange[0], aAmbMaxPwrRange[1])) {     
    aAmbMaxPwr = aAmbMaxPwrRange[2];
    errorEEPROMdata = true;
  }
  if (!checkInRange( aAmbThresh, aAmbThreshRange[0], aAmbThreshRange[1])) {     
    aAmbThresh = aAmbThreshRange[2];
    errorEEPROMdata = true;
  }
  if (!checkInRange( aHtrTargetTemp, aHtrTargetTempRange[0], aHtrTargetTempRange[1])) {
    aHtrTargetTemp = aHtrTargetTempRange[2];
    errorEEPROMdata = true;
  }
  if (!checkInRange( tempCutOff, tempCutOffRange[0], tempCutOffRange[1])) {     
    tempCutOff = tempCutOffRange[2];
    errorEEPROMdata = true;
  }
  return errorEEPROMdata;
}