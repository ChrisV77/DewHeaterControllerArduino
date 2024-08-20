/*
  Save to & write params from EEPROM to store between power cycles
  The format of saved EEPROM data is:
  Byte 0:   mode
  Byte 2:   manual power
  Byte 4:   temp target - auto-heater mode
  Byte 6:   temp Cut-Off
  Byte 8:   max power - auto-ambient mode
*/

// read dew heater params from EEPROM
void getEEPROMparams() {
  unsigned int eepromAddress;  // counter for EEPROM address
  int numBytes = 2;            // 2 bytes per unsigned integer value stored on EEPROM for Arduino UNO

  eepromAddress = 0;
  EEPROM.get( eepromAddress , globalMode );           // the mode of operation 1=off 2=manual 3=auto-ambient 4=auto-ambient+heater sensor
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , manualPower );          // the manual power level
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , autoHeaterTargetTemp ); // the target heater temp - currently temp above ambient at 100% RH
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , tempCutOff );           // the heater temp at which heater powers cuts off - to prevent over heating
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , autoAmbientMaxPower );  // the max power (for auto-ambient mode)
  eepromAddress += numBytes;
  EEPROM.get( eepromAddress , autoAmbientThreshold ); // the temp threshold (for ambient mode)
}

// save dew heater params to EEPROM
void saveEEPROMparams() {
  unsigned int eepromAddress;  // counter for EEPROM address
  int numBytes = 2;            // 2 bytes per unsigned integer value stored on EEPROM for Arduino UNO

  eepromAddress = 0;
  EEPROM.put( eepromAddress , globalMode );           // the mode of operation 1=off 2=manual 3=auto-ambient 4=auto-ambient+heater sensor
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , manualPower );          // the manual power level
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , autoHeaterTargetTemp ); // the target heater temp - currently temp above ambient at 100% RH
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , tempCutOff );           // the heater temp at which heater powers cuts off - to prevent over heating
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , autoAmbientMaxPower );  // the max power (for auto-ambient mode)
  eepromAddress += numBytes;
  EEPROM.put( eepromAddress , autoAmbientThreshold ); // the temp threshold (for ambient mode)
}

