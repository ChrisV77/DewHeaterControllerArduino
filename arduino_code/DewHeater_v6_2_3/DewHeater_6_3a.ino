/*
 Dew Heater Controller
 
 PLEASE READ NOTES IN GLOBALS.H (lines 7-33) TO SET UP THESE DEVICES:
  - Ambient temp/humidity sensor: AM2320 on I2C; soon BME280 on I2C; DHT21/22 no more (not very good)
  - Heater temp sensor: DS18B20
  - PC comms to work with app DewHeaterPCinterface - displays everything & allows control of settings. Kind of OPTIONAL
  - OPTIONAL Displays: OLED (SSD1306), LCD (16x2 16x4) but not working with new version yet
  - OPTIONAL Button control on unit to control settings. Can't be used if PC control activae

v6.2.3
  - BME280 now working
  - set up PID better
  - fixed display labels & data for channels
  - fixed number heater o/p #flashes for mode
  - added rd2 - tell PC all start settings sent (so PC wont highlight btnsetmode)
  - WINDOWS APP added <panels> for channel display
v6.2.4
  - fixed poor AM2320 reads (used modified adafruit AM2320 library, but might also try Tilaart library)
  - fixed zero output with auto modes
  - started amalgamating/streanlining LCD & OLED display code
v6.3a
  - improved LCD & OLED displaying. 
  - added menus if using a display: change mode, change params, dipslay params. These all done separately. NB: changing mode doesn't set any params
  - changed auto modes
    - auto-ambient: auto-ambient threshold  = temperature below ambient to turn on heater (& max power when amivbent temp = dew point). I'm using between 5-8C
    - auto-heater: set target temperature relative to ambient. I'm using 2-4C
  - change: if in PC_CONTROL mode then can also have a display (but not mode button)
To do later (maybe):
  - add Rob Tillart AM2320 sensor library. But check if better.
  - WINDOWS APP - MAYBE use panels for mode settings (with grp boxes) to just blank out labels and values within each
*/

#define DH_version "6.3a"
#include "Globals.h"    // include the global variables, constants etc
#include "EEPROM.h"     // set up to use the EEPROM for parameter storage between power cycles

void setup(){
  // start serial port to communicate with PC
  Serial.begin(9600);

  // read stored params from EEPROM
  getEEPROMparams();

  // setup mode control pin - only if MODEBUTTON enabled
  #ifdef MODEBUTTON
    pinMode(controlSwitchPin, INPUT_PULLUP);            // internal pullup resistor, so LOW = active
  #endif

  // set up 
  setupHeaterOutputPins();    // heater pins
  resetAmbientSensor();       // sensors  - ambient: BME280,AM2320
  setUpHeaterSensors();       //          - & heaters DS18B20
  setUpPIDcontrol();          // PID controller params

  // set up timing, ambient sensor state & channel set state
  currentMillis = millis();
  lastAmbientCheck = currentMillis; 
  ambientChecked = false;                                   // start with ambient not checked yet
  for (int currChan=0; currChan < numChannels; currChan++) {
    channelSet[currChan] = false;                           // reset channel set state to false
  }

  // set up display - if any display enabled
  #ifdef DISPLAY_ON
    displaySetUp();
    displayVersion();
    menuL2ParamDisplay();
    displayClear();
  #endif 
  if (outputFlashMode) flashNumHeaterChannels(numChannels);    // flash output mode number of times, then power level
}

void loop() { 
  // Check for commands from PC or button depending on which is ON
  #ifdef PC_CONTROL
   if (checkPC()) doCMDReceived();
  #endif
  #ifdef MODEBUTTON
    if (checkButtonPress()) menuL1Main_Select();
  #endif

 currentMillis = millis();                                        // get current time
 if (!ambientChecked) {
    // do ambient check (if not already done & at start/end cycle)
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
    // Get heater info and display (if ambient check done & 1/2 way through cycle)
    if ((currentMillis - lastAmbientCheck) > (cycleDuration/2)) {
      ambientChecked = false;
      getDS18B20data();                                         // read all the DS18B20 heater sensors
      // go through channels and set the heaters
      for (int currChan=0; currChan < numChannels; currChan++) {
        if (!channelSet[currChan]) {
          setHeaterMode(currChan);                         // set heater output (depends on mode)
          setHeaterOutput(currChan, outputPower[currChan]);      // set heater output pins
          channelSet[currChan] = true;                          // the channel has been set
        }
      }
      // finished checking/setting all channels, reset cycle & tell PC if connected
      // channel heater info: if these device enabled, display & send to PC (if connected)
      #ifdef PC_CONTROL
        if (PCconnected) {                                      // if PC connected
          sendAmbientDataToPC();                                // send ambient data
          for (int currChan=0; currChan < numChannels; currChan++) {
            sendChannelDataToPC(currChan);                      // send channel data
          }
          tellPCcycleDone();                                    // tell PC cycle completed: PC will display in app and save cycle data
        }
      #endif
      #ifdef DISPLAY_ON
        for (int currChan=0; currChan < numChannels; currChan++) {
          displayChannelData(currChan);
        }
      #endif
      if (outputFlashMode) flashHeaterModePower();              // flash output mode number of times, then power level
    }
  }
}
