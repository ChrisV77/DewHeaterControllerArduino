void getAM2320data() {
  // read ambient temp and humidity - takes 100s of msec?
  temperatureAmbient = am2320.readTemperature();
  humidityAmbient = am2320.readHumidity();
//  delay(200);
  calcDewPoint();
}

void calcDewPoint() {
  float logEx;

  if (isnan(temperatureAmbient) || isnan(humidityAmbient)) {
    // read error
    errorAM2320 = true;
  }
  else {
    // Calc dew point if no error reading AM2320 sensor
    // simple calc = dewPointAmbient = temperatureAmbient - ((100 - humidityAmbient)/5.0);
    // this is the more complex calc
    logEx=0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
    dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);    
    errorAM2320 = false;
  }
}

void getDS18B20data(int theChannel) {
  // restart the library if last pass had a read error (value == -127C/DEVICE_DISCONNECTED_C)
  if (errorDS18B20[theChannel] == true) {
    if (theChannel == 0) sensor0.begin();
    if (theChannel == 1) sensor1.begin();
    delay(750/ (1 << (12-resDS18B20)));
    delay(750/ (1 << (12-resDS18B20)));
  }

  // The DS18B20s on one-wire digital pins
  // if (theChannel == 0) numberOfDevices[theChannel] = sensor0.getDeviceCount();
  // if (theChannel == 1) numberOfDevices[theChannel] = sensor1.getDeviceCount();

  // request temperature, calc delay needed for read/conversion
  if (theChannel == 0) sensor0.requestTemperatures();
  if (theChannel == 1) sensor1.requestTemperatures();
  delay(750/ (1 << (12-resDS18B20)));

  // check parasitic mode
  if (theChannel == 0) isItParasitic[theChannel] = sensor0.isParasitePowerMode();
  if (theChannel == 1) isItParasitic[theChannel] = sensor1.isParasitePowerMode();
   
  // get the resolution
  if (theChannel == 0) deviceResolution[theChannel] = sensor0.getResolution();
  if (theChannel == 1) deviceResolution[theChannel] = sensor1.getResolution();

  // get the temperature degC and check for error
  if (theChannel == 0) tempSensor[theChannel] = sensor0.getTempCByIndex(0);
  if (theChannel == 1) tempSensor[theChannel] = sensor1.getTempCByIndex(0);
  
  // save error status
  if (tempSensor[theChannel] == DEVICE_DISCONNECTED_C) {
    errorDS18B20[theChannel] = true;
  }
  else {
    errorDS18B20[theChannel] = false;
  }
}
