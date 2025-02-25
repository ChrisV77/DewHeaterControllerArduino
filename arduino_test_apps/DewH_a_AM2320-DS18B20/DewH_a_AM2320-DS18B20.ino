// test the sensors

// include the global variables & constants for PINs etc
#include "Globals.h"

// AM2320 for ambient temperature & humidity sensing on I2C - ADAFRUIT libraries
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
Adafruit_AM2320 am2320 = Adafruit_AM2320();

// DS18B20s: setup oneWire instances and pass references to Dallas Temperature
// set up 2x channels with Heater/DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire0(DS18B20pin[0]);
OneWire oneWire1(DS18B20pin[1]);
DallasTemperature sensor0(&oneWire0);
DallasTemperature sensor1(&oneWire1);

// some other variables (in addition to Globals from main program
int       numberOfDevices[numChannels];
boolean   isItParasitic[numChannels];
int       deviceResolution[numChannels];
int       loopNumber = 1;

void setup() {
  // start serial port
  Serial.begin(9600);

  // Start up the sensor libraries - AM2320 & DS18B20s
  am2320.begin();
  sensor0.begin();
  sensor1.begin();
  sensor0.setResolution(resDS18B20);
  sensor1.setResolution(resDS18B20);
}

void loop() { 
  Serial.print("Loop");
  Serial.println(loopNumber);

  // The AM2320 on I2C
  getAM2320data();
  printAM2320data();

  // The DS18B20s on one-wire/dallastemperature
  getDS18B20data(0);
  printDS18B20data(0);
  getDS18B20data(1);
  printDS18B20data(1);

  delay(10000);
  loopNumber +=1;
}
