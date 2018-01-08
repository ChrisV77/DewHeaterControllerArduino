// Dew Heater Controller - test DHT
// for v4.0
// output display = serial output to computer
// Select Tools/Serial Monitor

// global constants 
const int numHeaters = 3;           // max number of heaters
const float tempCutOff = 25;        // heater cut-off if too hot
const float dewPointThreshold = 5;  // threshold above dew point to start heater

// DHT22 = digital 2. Ambient temperature & humidity

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

// general - delay for sensor and "i" for counting
int theDelay = 1000;          // need ~1sec delay after DS18B20 read in parasitic mode
int i;

void setup() {
  // set up the serial monitor: 
  Serial.begin(9600);
  Serial.println("Dew Heater Controller");
  Serial.println("DHT22 test - serial output to computer");
  Serial.println("Digital pin \tStatus\t\tTemp (C)\tHumidity (%)\tDew Point");
  delay(theDelay*2);  
}

void loop() {
  // Read ambient temperature & humidity from DHT22 
  // & calculate dew point if no error reading
  errorDHT22 = getDHT22data();
  // display ambient data
  displayAmbientData();
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

void displayAmbientData() {
  Serial.print(DHTPIN);
  Serial.print("\t\t");
  if (errorDHT22) {
    // error reading DHT22
    Serial.println("error reading DHT22");
  }
  else {
    // if ambient read okay display DHT22 data
    Serial.print("OK");
    Serial.print("\t\t");
    Serial.print(temperatureAmbient, 1);
    Serial.print("\t\t");
    Serial.print(humidityAmbient, 1);
    Serial.print("\t\t");
    Serial.println(dewPointAmbient, 1);
  }
  delay(theDelay);
}


