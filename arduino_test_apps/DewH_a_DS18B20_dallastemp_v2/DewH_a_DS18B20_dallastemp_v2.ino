#include <OneWire.h>
#include <DallasTemperature.h>

long int startMillis;
long int endMillis;
const int numChannels   = 2;
OneWire ds18x20[] = { 7, 8 };
const int oneWireCount = sizeof(ds18x20) / sizeof(OneWire);
DallasTemperature sensor[oneWireCount];

void setup(void) {
  // start serial port
  Serial.begin(9600);

  // Start up the library on all defined bus-wires
  DeviceAddress deviceAddress;
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].setOneWire(&ds18x20[i]);
    sensor[i].begin();
    if (sensor[i].getAddress(deviceAddress, 0)) sensor[i].setResolution(deviceAddress, 12);
  }
}

void loop(void) {
  Serial.print("There are ");
  Serial.print(oneWireCount);
  Serial.println(" Sensors");

  Serial.print("Requesting temperatures...");
  startMillis = millis();
  for (int i = 0; i < oneWireCount; i++) {
    sensor[i].requestTemperatures();
  }
  endMillis = millis();
  Serial.print("DONE in ");
  Serial.print((endMillis - startMillis));
  Serial.println("msec");

  delay(1000);
  for (int i = 0; i < oneWireCount; i++) {
    float temperature = sensor[i].getTempCByIndex(0);
    Serial.print("Temperature for the sensor ");
    Serial.print(i);
    Serial.print(" is ");
    Serial.println(temperature);
  }
  Serial.println();
  delay(4000);
}