/*
 Dew Heater Controller
 
 PLEASE READ NOTES IN GLOBALS.H (lines 7-33) TO SET UP THESE DEVICES:
  - Ambient temp/humidity sensor: AM2320 on I2C; soon BME280 on I2C; DHT21/22 no more (not very good)
  - Heater temp sensor: DS18B20
  - PC comms to work with app DewHeaterPCinterface - displays everything & allows control of settings. Kind of OPTIONAL
  - OPTIONAL Displays: OLED (SSD1306), LCD (16x2 16x4) but not working with new version yet
  - OPTIONAL Button control on unit to control settings. Can't be used if PC control activae

 v6.1: New since v6.0
  - improved #defs etc
  - changed button control: push button to set up, push again to select mode
  - can have up to 4 chans
  - have added SSD1306ascii driver for OLED (uses less memory). This is seems to fix problem with running both OLED & PC_CONTROL
 v6.2: New since v6.1
  - Using only ssd1306ascii library for OLED (removed old adafruit ssd1306 library)
  - Fixed error: modebutton - the changed settings are now saved eePROM
 v6.2.1: New since v6.2
  - Using only ssd1306ascii library for OLED (removed old adafruit ssd1306 library)
  - Fixed error: modebutton - the changed settings are now saved eePROM
  - Fixed: timing to display ambient for half cycle then chans next half (used daggy delay before)
  - add output flash to show mode number, has bool in  Globals to  turn on/off (good if have LED across output)
  - BME280. I have put in the code and it compiles OK. Just have to check it works when I get one
 v6.2.2
  - added 16*2, 20*4 LCD display
  - improved heater sensor setup using multi example in dalla temperature library
  - changed auto-ambient back to old way. Heater o/p inversly proportional to ambient temp above dew point
  - flash LED outputs on startup numChannels times
v6.2.3
  - BME280 working
  - set up PID better
  - fixed display labels & data for channels
  - fixed number heater o/p #flashes for mode
  - added rd2 - tell PC all start settings sent (so PC wont highlight btnsetmode)
  - APP added <panels> for channel display

  To do
  - APP - use panels for mode settings (with grp boxes) to just blank out labels and values within each
  - APP - add the auto-amb/htr setting to each channel when saving to file
  - should I make auto-heater rel to DP or ambient. Check that Celestron heater & maybe Kendrick
  - MAYBE? have savetodisk as an eeprom var on arduino?
  - redundancy flashHeaterOutputs() & flashModeHeater() ?
*/

#define DH_version "6.2.3"
#include "Globals.h"    // include the global variables, constants etc
#include "EEPROM.h"     // set up to use the EEPROM for parameter storage between power cycles

void setup(){
  // start serial port to communicate with PC
  Serial.begin(9600);

  // read stored params from EEPROM
  getEEPROMparams();

  // set up display - if any display enabled
  #ifdef DISPLAY_ON
    displaySetUp();
    displayVersion();
  #endif 

  // setup mode control pin - only if MODEBUTTON enabled
  #ifdef MODEBUTTON
    pinMode(controlSwitchPin, INPUT_PULLUP);            // internal pullup resistor, so LOW = active
  #endif

  // set up 
  setupHeaterOutputPins();    // heater pins
  resetAmbientSensor();       // sensors - ambient: AM2320,DHT21/22,BME289 & heater: DS18B20s
  setUpHeaterSensors();
  setUpPIDcontrol();          // PID controller params

  // set up timing, ambient sensor state & channel set state
  currentMillis = millis();
  lastAmbientCheck = currentMillis; 
  ambientChecked = false;                                   // ambient not checked yet
  for (int currChan=0; currChan < numChannels; currChan++) {
    channelSet[currChan] = false;                           // reset channel set state to false
  }

  // flash heater output
  delay(displayDelay*4);
  flashAllHeaterOutputs(numChannels);      //  numChannels xTimes
  delay(displayDelay*4);
  flashAllHeaterOutputs(globalMode+1);      //  mode xTimes
}

void loop() { 
  // Check for commands from PC or button depending on which is ON
  #ifdef PC_CONTROL
   if (checkPC()) doCMDReceived();
  #endif
  #ifdef MODEBUTTON
    if (checkModeButtonPress()) changeTheMode();
  #endif

 currentMillis = millis();                                      // get current time
 if (!ambientChecked) {
      // do ambient check at end of cycle
    if ((currentMillis - lastAmbientCheck) > cycleDuration) {   
      // reset time & send mode data to PC
      ambientChecked = true;                                    // set ambientChecked to true
      lastAmbientCheck = currentMillis;                         // set time of last ambient sensor check
      // read ambient sensor & display or send to PC
      getAmbientData();
      #ifdef DISPLAY_ON
        displayAmbientData();                                   // display on LCD/OLED if connected
      #endif
      for (int currChan=0; currChan < numChannels; currChan++) {
        channelSet[currChan] = false;                           // reset channel set/read state to false
      }
    }
  }
  else {
    // ambient sensor has been checked. Get heater info and display 1/2 way through cycle
    if ((currentMillis - lastAmbientCheck) > (cycleDuration/2)) {
      ambientChecked = false;
      getDS18B20data();                                         // read all the DS18B20 heater sensors
      // go through channels and set the heaters
      for (int currChan=0; currChan < numChannels; currChan++) {
        if (!channelSet[currChan]) {
          setHeaterModePower(currChan);                         // set heater output (depends on mode)
          outputToHeater(currChan, outputPower[currChan]);      // set heater output pins
          channelSet[currChan] = true;                          // the channel has been set
        }
      }
      if (outputFlashMode) flashModeHeater();                   // flash output mode number of times 
      // finished checking/setting all channels, reset cycle & tell PC if connected
      // channel heater info: if these device enabled, display & send to PC (if connected)
      #ifdef PC_CONTROL
        if (PCconnected) {                          // if PC connected
          sendAmbientDataToPC();                    // send ambient data
          for (int currChan=0; currChan < numChannels; currChan++) {
            sendChannelDataToPC(currChan);          // send channel data
          }
          tellPCcycleDone();                        // tell the PC that the cycle is done: PC will display in app and save cycle data
        }
      #endif
      #ifdef DISPLAY_ON
        for (int currChan=0; currChan < numChannels; currChan++) {
          displayDS18B20data(currChan);
          displayHeaterOutput(currChan);
        }
      #endif
    }
  }
}
