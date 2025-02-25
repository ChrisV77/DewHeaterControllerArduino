void resetAmbientSensor() {
  // not needed
}

void getBME280data() {
  bool bmeStatus;

  // read ambient temp and humidity - takes 100s of msec?.
  bmeStatus = bme.begin();
  delay(500);      // short delay for read - not sure if needed

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pressureAmbient, temperatureAmbient, humidityAmbient, tempUnit, presUnit);
  calcDewPoint();
}

void calcDewPoint() {
  float logEx;

  if (isnan(temperatureAmbient) || isnan(humidityAmbient)) {
    errorAmbientSensor = true;
    humidityAmbient = errorValue;     // set both to read error value
    temperatureAmbient = errorValue;
  }
  else {
    errorAmbientSensor = false;
    // Calc dew point if no error reading sensor
    // this is the more complex calc
    // logEx = 0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
    // dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);    

    // simple calc:
    dewPointAmbient = temperatureAmbient - ((100 - humidityAmbient)/5.0);
  }
}
