// get ambient temp & humidity data from AM2320 or BME280 sensor

#ifdef AM2320_ON
void resetAmbientSensor() {
  am2320.begin();
}

void getAmbientData() {
  // read ambient temp and humidity - takes 100s of msec?.
  humidityAmbient = am2320.readHumidity();
  temperatureAmbient = am2320.readTemperature();
  calcDewPoint();
}
#endif

#ifdef BME280_ON
void resetAmbientSensor() {
  // Start up the bme280. address is 0x76
  bme.begin();
}

void getAmbientData() {
  bool bmeStatus;

  // read ambient temp and humidity - takes 100s of msec?.
  bmeStatus = bme.begin();
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);    // units for read
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);
  delay(comDelay);      // short delay for read - not sure if needed
  if (!bmeStatus) {
    // error reading sensor
    errorAmbientSensor = true;
  }
  else {
    // sensor read OK
    bme.read(pressureAmbient, temperatureAmbient, humidityAmbient, tempUnit, presUnit);
    calcDewPoint();
  }
}
#endif

// calculation doesn't do any reading, so is common to all sensors
void calcDewPoint() {
  float logEx;

  if (isnan(temperatureAmbient) || isnan(humidityAmbient)) {
    errorAmbientSensor = true;
    humidityAmbient = errorValue;     // set both to read error value
    temperatureAmbient = errorValue;
  }
  else {
    // Calc dew point if no error reading sensor
    errorAmbientSensor = false;
    if (dewPointComplexCalc) {
      // this is the more complex calc
      logEx = 0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
      dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);    
    }
    else {
      // simple calc:
      dewPointAmbient = temperatureAmbient - ((100 - humidityAmbient)/5.0);
    }
  }
}
