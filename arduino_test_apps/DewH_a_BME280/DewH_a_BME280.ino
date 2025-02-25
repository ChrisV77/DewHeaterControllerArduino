// test the sensors

// include the global variables & constants for PINs etc
#include "Globals.h"

int       loopNumber = 1;

void setup() {
  // start serial port
  Serial.begin(9600);

  Wire.begin();
  // Start up the sensor libraries - BME280 address is 0x76
  bme.begin();

  // not needed, just using to check function
  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }


}

void loop() { 
  Serial.print("Loop");
  Serial.println(loopNumber);

  // The AM2320 on I2C
  getBME280data();
  printBME280data();

  
  delay(4000);
  loopNumber +=1;
}
