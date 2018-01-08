// Dew Heater Controller - test DS18B20
// for v4.0
// output display = serial output to computer
// Select Tools/Serial Monitor

// global constants 
const int numHeaters = 3;           // max number of heaters
const float tempCutOff = 25;        // heater cut-off if too hot
const float dewPointThreshold = 5;  // threshold above dew point to start heater

// these are the pins being used for sensors and heaters
// DHT22 = digital 2. Ambient temperature & humidity
// DS18B20 = digital 10, 11, 12. Sensor 1-3 temperature
// Heater Outputs = digital 3, 5, 6. Heater 1-3 temperature.

// DHT22 for ambient temperature & humidity sensing
// on pin 2
#include <DHT.h>
#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
#define DHTPIN 2    // the DHT22 pin
DHT dht(DHTPIN, DHTTYPE);
float dewPointAmbient;
float temperatureAmbient;
float humidityAmbient;
boolean errorDHT22;

// MOSFET Driver modules = Heater Outputs 1-3 on PWM pins
int heaterDutyCycle[] = {0, 0, 0};       // drive to heater range=0-255
int heaterPin[] = {3, 5, 6};             // heater pins

// DS18B20 for sensing temperature on heated elements, using Onewire library
// Have separate pin for each DS18B20. Easier to wire up and distinguish
#include <OneWire.h>
#include <DallasTemperature.h>
int DS18B20Pin[] = {10, 11, 12};  // only pin12 at this stage
OneWire oneWire1(DS18B20Pin[0]);
OneWire oneWire2(DS18B20Pin[1]);
OneWire oneWire3(DS18B20Pin[2]);
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
  Serial.println("DHT22 & DS18B20 test - serial output to computer");
  Serial.println("DHT22 = sensor for ambient temp & humidity");
  Serial.println("DS18B20s = sensors for heater temps");
  Serial.println();

  delay(theDelay*2);
  
  // setup heater settings and set to zero
  for (i = 0; i < numHeaters; i++) {
    pinMode(heaterPin[i], OUTPUT);  // sets the pin as output
    heaterDutyCycle[i] = 0;
    analogWrite(heaterPin[i] , heaterDutyCycle[i]);
  } 
}

void loop() {
  // Read ambient temperature & humidity from DHT22 
  // & calculate dew point if no error reading
  errorDHT22 = getDHT22data();
  // display ambient data
  Serial.println("DHT22 pin \tStatus\tTemp(C)\tHumidity(%)\tDew Point \t");
  displayAmbientData();

  // Read temperatures from DS18B20s and set duty cycle. NB-need long delay after read
  Serial.println("DS18B20 Pins\tStatus \tTemp(C)\tHeater=\tNumber\tPin \tPwr (%)");
  for (i = 0; i < numHeaters; i++) {
    // read the heater, return if an error and value to global variable tempHeater
    errorDS18B20[i] = getDS18B20data(i);
    // send duty cycle to heater output, set to 0 if any read errors
    heaterDutyCycle[i] = setDutyCycle(i);
    analogWrite(heaterPin[i] , heaterDutyCycle[i]);   
    // display heater data
    displayHeaterData(i);
  }
  Serial.println();
  Serial.println();
  delay(theDelay*3);
}

boolean getDHT22data() {
  float logEx,dewPoint;
  boolean anError;
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidityAmbient = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperatureAmbient = dht.readTemperature();
  delay(theDelay);

  if (isnan(temperatureAmbient) || isnan(humidityAmbient)) {
    // if error reading DHT22 set all to 0
    temperatureAmbient = 0;
    humidityAmbient = 0;
    dewPointAmbient = 0 ;  
    anError = true;
  }
  else {
    // if no error reading DHT22 calc dew point
    // more complex dew point calculation
    logEx=0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
    dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
//    simpler dew point calculation
//    dewPointAmbient = temperatureAmbient - ((100 - humidityAmbient)/5.0);
    anError = false;
  }
  return anError;
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

int setDutyCycle(int theHeater) {
  // set output duty cycle on temp diff between heater and ambient dew point 
  float aboveDewPoint;
  float theDutyCycle = 0;

  if (errorDHT22 || errorDS18B20[ theHeater ]) {
    // Heater OFF if error reading DHT22 or DS18B20 (for this heater)
    theDutyCycle = 0;
  }
  else {
    if ( tempHeater[ theHeater ] > tempCutOff )  {
      // Heater OFF if above cut-off
      theDutyCycle = 0;
    }
    else {
      // Heater ON if Temp < Threshold(C) above dew point(C)
      aboveDewPoint = tempHeater[ theHeater ] - dewPointAmbient;
      // restrict between 0 & threshold
      aboveDewPoint = constrain( aboveDewPoint , 0.0 , dewPointThreshold );
      // PWM 0 - 100% duty cycle EQUIV TO analog 0 - 255
      theDutyCycle = 255 * (( dewPointThreshold - aboveDewPoint ) / dewPointThreshold );
    }
  }
  return theDutyCycle;  
}

void displayAmbientData() {
  Serial.print(DHTPIN);
  Serial.print("\t\t");
  if (errorDHT22) {
    // error reading DHT22
    Serial.println("\terror reading DHT22");
  }
  else {
    // if ambient read okay display DHT22 data
    Serial.print("OK\t");
    Serial.print(temperatureAmbient, 1);
    Serial.print("\t");
    Serial.print(humidityAmbient, 1);
    Serial.print("\t\t");
    Serial.println(dewPointAmbient, 1);
  }
  Serial.println();
  delay(theDelay);
}

void displayHeaterData( int theHeater ) {
  float thePerCentDutyCycle;
  thePerCentDutyCycle = 100 * heaterDutyCycle[ theHeater ] / 255;

  Serial.print(DS18B20Pin[ theHeater ]);    // heater pins
  Serial.print("\t\t");

  if ( errorDS18B20[ theHeater ] ) {
    // error reading DS18B20
    Serial.print("error\t\t\t");
  }
  else {
    // DS18B20 read OK
    Serial.print("OK \t");
    Serial.print(tempHeater[ theHeater ] , 1);
    Serial.print("\t\t");
  }
  Serial.print(theHeater+1);
  Serial.print("\t");
  Serial.print(heaterPin[ theHeater ]);
  Serial.print("\t");
  Serial.println(thePerCentDutyCycle , 0);
  delay(theDelay);
}

