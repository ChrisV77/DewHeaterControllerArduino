// Dew Heater Controller - test DS18B20
// for v4.0
// output display = serial output to computer
// Select Tools/Serial Monitor

// global constants 
const int numHeaters = 3;           // max number of heaters
const float tempCutOff = 25;        // heater cut-off if too hot
const float dewPointThreshold = 5;  // threshold above dew point to start heater

// DS18B20 = digital 10, 11, 12. Sensor 1-3 temperature
// Heater Outputs = digital 3, 5, 6. Heater 1-3 temperature.

// MOSFET Driver modules = Heater Outputs 1-3 on PWM pins
int heaterDutyCycle[] = {0, 0, 0};       // drive to heater
int heaterPin[] = {3, 5, 6};             // heater pins

// DS18B20 for sensing temperature on heated elements, using Onewire library
// Have separate pin for each DS18B20. Easier to wire up and distinguish
#include <OneWire.h>
#include <DallasTemperature.h>
int DS18B20[] = {10, 11, 12};  // only pin12 at this stage
OneWire oneWire1(DS18B20[0]);
OneWire oneWire2(DS18B20[1]);
OneWire oneWire3(DS18B20[2]);
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
DallasTemperature sensor3(&oneWire3);
boolean errorDS18B20[3];
float tempHeater[3];          // 3x heaters

// general - delay for sensor and "i" for counting
int theDelay = 1000;          // need ~1sec delay after DS18B20 read in parasitic mode
int i;

void setup() {
  // set up the serial monitor: 
  Serial.begin(9600);
  Serial.println("Dew Heater Controller");
  Serial.println("DS18B20 test - serial output to computer");
  Serial.println("Heater number \tDigital Pin \tStatus \t\tTemp (C)");
  delay(theDelay*2);  
  
  // setup heater settings and set to zero
  for (i = 0; i < numHeaters; i++) {
    pinMode(heaterPin[i], OUTPUT);  // sets the pin as output
    heaterDutyCycle[i] = 0;
    analogWrite(heaterPin[i] , heaterDutyCycle[i]);
  } 
}

void loop() {
  // Read temperatures from DS18B20s and set duty cycle. NB-need long delay after read
  for (i = 0; i < numHeaters; i++) {
    // read the heater, return if an error and value to global variable tempHeater
    errorDS18B20[i] = getDS18B20data(i);
    // display heater data 
    displayHeaterData(i);
  }
}

boolean getDS18B20data(int theHeater) {
  boolean anError = false;

  // for each heater
  // Start the sensor, read temperatures & save to global array tempHeater

  if (theHeater == 0) {
    // Start up the DallasTemperature library for lens temp sensors 1-3
    sensor1.begin();
//    delay(theDelay);
    sensor1.requestTemperatures();
    tempHeater[theHeater] = sensor1.getTempCByIndex(0);
  }
  if (theHeater == 1) {
    sensor2.begin();
//    delay(theDelay);
    sensor2.requestTemperatures();
    tempHeater[theHeater] = sensor2.getTempCByIndex(0);
  }
  if (theHeater == 2) {
    sensor3.begin();
//    delay(theDelay);
    sensor3.requestTemperatures();
    tempHeater[theHeater] = sensor3.getTempCByIndex(0);
  }
  if (tempHeater[theHeater] == -127) {
    anError = true;
  }
  return anError;
}

void displayHeaterData( int theHeater ) {
  Serial.print(theHeater+1);
  Serial.print("\t\t");
  Serial.print(heaterPin[ theHeater ]);    // heater pins
  Serial.print("\t\t");

  if ( errorDS18B20[ theHeater ] ) {
    // error reading DS18B20
    Serial.println("error");
  }
  else {
    // DS18B20 read OK
    Serial.print("OK \t\t");
    Serial.println(tempHeater[ theHeater ] , 1);
  }
  delay(theDelay);
}

