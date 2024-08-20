/*
 - All definitions, variables, constants etc for
 - pins, sensors, displays, feedback PID controller
*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// THESE ARE THE THINGS TO SETUP

// NUMBER OF HEATER CHANNELS. choose from 1 - 4 channels/heaters
const int   numChannels = 2;

// AMBIENT TEMP/HUMIDITY SENSOR: AM2320 or BME280
// options: AM2320_ON, BME280_ON (not sure if working yet)
#define AM2320_ON

// MODE CONTROL: either - PC_CONTROL: remote via PC (DewHeaterController = DewHeaterPCinterface_v2). Or 
//                      - MODEBUTTON: LOCAL with button
// options: PC_CONTROL, MODEBUTTON, (use default PC_CONTROL if no MODEBUTTON, as no PC comms if no connection from PC to arduino until established)
#define PC_CONTROL

// OPTIONAL. DISPLAYS: OLED (OLED1306=SSD1306 display), LCD (LCD1602=16*2 LCD1604=16*4 LCD2004=20*4). LCD not working yet in current version
// options: OLED1306, LCD1602, LCD1604, LCD2004, OR DISPLAY_OFF. Need one if using MODEBUTTON control. NB:OLED using ssd1306ascii driver
#define DISPLAY_OFF

// SOME OTHER OPTIONAL STUFF ** can be set in PC app; * stored in eeprom
int         autoAmbientThreshold = 5;           // **,* threshold to turn on heater for auto-ambient mode (diff between ambient temp and dew point)
int         tempCutOff = 32;                    // **,*  heater cut-off if too hot (& can be set in PC app (extras). Only works if have a DS18B20 temp sensor on heater
int         autoHeaterTargetTemp = 5;           // ** Only for Auto-heater mode: target temperature above dew point (& can be set in PC app)
const bool  blankHeaterDuringRead = true;       // options: true, false: blank heater during heater temperature read
                                                // - Use if getting strange values. Heater PWM output can interfere with DS18B20 on I2C if wires are long and close
const bool  dewPointComplexCalc = true;         // complex (true) or simple (false) dew point calculation
const bool  outputFlashMode = true;             // flash mode number on output (good if have LEDs there). options: true/false
int         manualPower = 10;                   // ** power level if in Manual mode OR Auto & no sensor connected: 0-100%
int         autoAmbientMaxPower = 30;           // ** max power level if in auto-ambient mode: 0-100%

// THAT'S ALL
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#define  DH_start   "DewHeater v"
#define  DH_modes   "Modes: Man-Auto "
#define  DH_control "Cntrl: PC-Btn   "
#define  DH_display "Disp:  OLED-LCD "

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DIGITAL PINS & SENSORS

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Sensors for ambient temperature & humidity sensing on I2C
// ambient temp/humidity sensor: AM2320, BME280 (DHT21/22 not used anymore)
#include <Wire.h>   // needed for I2C comms

// the ambient sensor
 #ifdef AM2320_ON
  #include "Adafruit_Sensor.h"
  #include "Adafruit_AM2320.h"
  Adafruit_AM2320 am2320 = Adafruit_AM2320();
#endif
#ifdef BME280_ON
  #include <BME280I2C.h>
  BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                    // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
#endif

// and variables for ambient sensor
float     temperatureAmbient;           // things to read
float     humidityAmbient;
float     dewPointAmbient;
float     pressureAmbient;
bool      errorAmbientSensor;           // error, eg if not connected

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// the displays if being used
// NB: if using this you'll need to run this and the ambint sensor on the I2C
// - LCD 16*2 or 16*4 20*4

#ifdef LCD1602
  #define LCD_ON
  #define numberCol 16
  #define numberRow 2
#endif
#ifdef LCD1604
  #define LCD_ON
  #define numberCol 16
  #define numberRow 4
#endif
#ifdef LCD2004
  #define LCD_ON
  #define numberCol 20
  #define numberRow 4
#endif

#ifdef LCD_ON
  // LCD display 16x2, address usually 0x3F or 0x27
  #include <hd44780.h>                        // main hd44780 header, seems to be best library for LCD I2C
  #define DISPLAY_ON
  #include <hd44780ioClass/hd44780_I2Cexp.h>  // i2c via expander backpack
  #ifdef LCD1602
    hd44780_I2Cexp lcd(0x27);
  #endif
  #ifdef LCD1604
    hd44780_I2Cexp lcd(0x3F);                 // I think this is correct??
  #endif
  #ifdef LCD2004
    hd44780_I2Cexp lcd(0x3F);
  #endif
  #define uCol 1                              // 1 per column/character
  #define uRow 1                              // 1 per row/character
#endif

// - OLED 128*32 (SSD1306). Haven't sorted out SH1106

#ifdef OLED1306
  // OLED SSD1306 0.9"/1.3" display 128x32 displays usually on I2C 0x3C (sometimes 0x3D)
  // simpler library, uses much less memory
  #define DISPLAY_ON
  #include "SSD1306Ascii.h"
  #include "SSD1306AsciiAvrI2c.h"
  #define display_address 0x3c
  SSD1306AsciiAvrI2c display;
  #define uCol 6                   // 8 units per column character (pixels)
  #define uRow 1                   // 1 unit per row character
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Sensors for heater temperature: DS18B20s: temp sensors, uisng OneWire & DallasTemperature
// the DIGITAL PINS for sensor input, heater output & control.

#include <OneWire.h>
#include <DallasTemperature.h>

const int heaterPin[]  = {9, 10, 5, 6};      // MOSFET heater output channels on PWM pins. 4 possible channels = pins 9, 10, 5, 6
OneWire DS18B20pin[] = {7, 8, 4, 11};        // heater temp sensor pins. 2 channels = 7, 8, 4, 11

// include definitions for up to 4 possible channels
DallasTemperature sensor[numChannels];      //  there are numChannels heater sensors

// DS18B20 variables
double      tempHeater[numChannels];        // DS18B20 sensor temperatures from sensors1/2.
boolean     errorDS18B20[numChannels];      // error if tempSensor == -90 or -127C (DEVICE_DISCONNECTED_C)
const int   resDS18B20 = 11;                // resolution of DS18B20 read. I think 9/10/11/12 = 0.5/0.25/0.125/0.0625 degC
                                            // I have been using 10 or 11 (11 a bit OTT but so what)
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// digital PIN for MODE CONTROL: only if local MODEBUTTON control
#ifdef MODEBUTTON
  const int controlSwitchPin = 3;   // Mode control switch PIN, uses internal pull-up resistor  
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PID controller - manually tuned PID for 50mm guidescope, gain = 
#include "PID_v1_bc.h"
const int maxChannels = 4;
double setPoint[maxChannels] = {22, 22, 22, 22};  // PID set point, dew point
double PIDout[maxChannels];                       // PID Output, NB: input is tempSensor
double Kp = 100;                                  // K-values for P,I,D
double Ki = 150;
double Kd = 10;

PID PID_control[] = {
  PID(&tempHeater[0], &PIDout[0], &setPoint[0], Kp, Ki, Kd ,DIRECT),
  PID(&tempHeater[1], &PIDout[1], &setPoint[1], Kp, Ki, Kd ,DIRECT),
  PID(&tempHeater[2], &PIDout[2], &setPoint[2], Kp, Ki, Kd ,DIRECT),
  PID(&tempHeater[3], &PIDout[3], &setPoint[3], Kp, Ki, Kd ,DIRECT)
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// global variables mode
int         globalMode;                   // the SET mode at each heater:
                                          //                    0=Off 1=Manual 2=Auto-Amb 3=Auto-HeatSensor 4=Auto-HeaterSensorPID
                                          // for LED flashing:  1=Off 2=Manual 3=Auto-Amb 4=Auto-HeatSensor 5=Auto-HeaterSensorPID
const int   numberModes = 4;
#ifdef OLED1306
const char  *heaterModeString[numberModes] = { "Off", "Manual", "AutoAmb", "AutoHtr"};
#endif
#ifdef LCD_ON                             // shorter chars for LCD display
const char  *heaterModeString[numberModes] = { "Off", "Man", "A-Amb", "A-Htr"};
#endif

// channel cycle variables
int       outputPower[numChannels];       // % power to heater: 0 - 100 %
int       chanCycleMode[numChannels];     // the mode at each heater during current cycle. 
                                          //    - could be different to set mode (chanMode) if sensor read error OR in cut-off
                                          //    - this is just to tell the PC

// communication data variables
bool        PCconnected = false;                // is a PC connected
char        startCMD = ':';                     // start of command delimiter, ASCOMlike
char        endCMD = '#';                       // end of command delimiter, ASCOMlike 
const byte  numCOMChars = 16;                   // max number of characters in a command
char        receivedCOMChars[numCOMChars];      // an array to store the received data (command)
int         inCMD;                              // the array number of command received from computer
int         inCMDval;                           // the number associated with this command - NB only those starting with 'n' or 'c'
const int   numOfCMDs = 18;
int         activeChannel;                      // the current channel: used for channelParam variable
bool        cycleDone;                          // the current cycle is completed

const char  *controllerCMDs[numOfCMDs] =  {   "rd",                                       
                      //  PC comms:           0=connect(0), disconnect(1), end of startup sending arduino settings(2)
                                              "at",       "ah",         "ad",
                      //  ambient:            1=temp      2=humidity    3=dewpoint (values x10 to remove decimal point & make an int)
                                              "md",       "mn",         "mt",                 "mx",                 "mc",               "mh",               "ms",
                      //  mode:               4=mode      5=man pwr     6=target temp auto-H  7= max Pwr auto-A     8=temp cutoff       9=thresh auto-A     10=save on arduino     
                                              "cn",       "cm",         "ct",                 "cp",                 "cs",               "cd",
                      //  chans (curr cycle)  11=number   12=mode       13=heater temp        14=cycle pwr          15=setpoint auto-H  16=cycle done
                                              "tc"};
                      //                      17= number channels

const int   errorValue = -127;                   // value to send in error reading sensor (ambient / heaters)
                                                // use -90 or -127 (this is dallas temp error) ??

// timing stuff
bool        ambientChecked;                     // ambient sensor checked & read. This is the start of each cycle in the main loop (after checking PC)
bool        channelSet[numChannels];            // heater channel set & sent to PC
const int   displayDelay = 500;                 // delays between displaying items
const unsigned long   cycleDuration = 20000;    // 20 sec delay between cycles
                                                // A cycle = 1 PC check, 2 ambient sensor, 3 heater channels set
const int   comDelay = 50;                      // delay for PC coms - had 200ms, trying shorter - 150, 100, 50 OK
const int   btnDelay = 25;                      // delay for button press
                                                // these are for loop timing
unsigned long         currentMillis;            // current time in millisecs
unsigned long         lastAmbientCheck;         // time of last ambient sensor check

// some compile error messages
#ifdef BME280_ON
#ifdef AM2320_ON
#error "Can't define both AM2320 & BME280 ambient sensors. Change the unused one to xxxxxx_OFF"
#endif
#endif

#ifdef MODEBUTTON
#ifdef PC_CONTROL
#error "Can't have PC_CONTROL && MODEBUTTON"
#endif
#endif

#ifdef MODEBUTTON
#ifndef DISPLAY_ON
#error "If using MODEBUTTON, must have a display define"
#endif
#endif

