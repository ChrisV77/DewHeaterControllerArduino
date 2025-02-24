void resetDS18B20() {
  sensor0.begin();
  sensor1.begin();
  sensor0.setResolution(DS18B20res);
  sensor1.setResolution(DS18B20res);
}

// heater temp data from DS18B20 sensors
void getDS18B20data() {
  // restart the library if last pass had a read error (value == -127C/DEVICE_DISCONNECTED_C)
  // request temperature, calc delay needed for read/conversion
  sensor0.requestTemperatures();
  sensor1.requestTemperatures();
  delay(750/ (1 << (12-DS18B20res)));

  // get the temperature degC and check for error
  tempHeater[0] = sensor0.getTempCByIndex(0);
  tempHeater[1] = sensor1.getTempCByIndex(0);
}
