#include "PID_v1_bc.h"

// ambient sensor
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
Adafruit_AM2320 am2320 = Adafruit_AM2320();
// and variables for ambient sensor
double        temperatureAmbient;
double        humidityAmbient;
double        dewPointAmbient;
const double  cutOff = 30.0;
boolean       errorAmbientSensor;
unsigned long startMillis;
unsigned long lastMillis;
unsigned long currentMillis;
int           blockNum;
//int           i;
int           theStep;
bool          tempCutOff[2];

// heater pin
const int numChannels=2;
const int heaterPin[numChannels]   = {9, 10};      // MOSFET Driver modules = Heater Outputs 1-2 on PWM pins 9 & 10.

// DS18B20s: temp sensors. Setup oneWire instances and pass references to Dallas Temperature
const int DS18B20pin[numChannels]  = {7, 8};      // on pins 7 & 8
double     tempHeater[numChannels];     // DS18B20 sensor temperatures from sensors1/2
boolean   errorDS18B20[numChannels];   // true if error reading channel temp

#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire0(DS18B20pin[0]);
OneWire oneWire1(DS18B20pin[1]);
DallasTemperature sensor0(&oneWire0);
DallasTemperature sensor1(&oneWire1);
const int DS18B20res = 11;

double currentKvalues[numChannels][3];
double setPoint = 22;  // PID set point, dew point
double setPointDiff;
double PIDout[numChannels];                       // PID Output, NB: input is tempSensor
double Kp;
double Ki;
double Kd;
PID PID_control[] = {
  PID(&tempHeater[0], &PIDout[0], &setPoint, Kp,Ki,Kd,DIRECT),
  PID(&tempHeater[1], &PIDout[1], &setPoint, Kp,Ki,Kd,DIRECT)
};

// PID parameters
const int blockLoops = 3;      // 3 loops * 20sec = 1 min
const int theDelay = 20000;     // the loop delay = 20sec
bool cycleDone;                 // only do one cycle, use this to stop at end 
