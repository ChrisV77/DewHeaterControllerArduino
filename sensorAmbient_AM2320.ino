// get ambient temp/hum data from AM2320 sensor
// get heater temp data from DS18B20 sensors

#ifdef AM2320_ON
void resetAmbientSensor() {
  am2320.begin();
}

void getAmbientData() {
  float logEx;
  // read ambient temp and humidity - takes 100s of msec?
  humidityAmbient = am2320.readHumidity();
  temperatureAmbient = am2320.readTemperature();
  errorAmbientSensor = false;
  if (isnan(temperatureAmbient) || isnan(humidityAmbient)) {
    errorAmbientSensor = true;
  }
  else {
    errorAmbientSensor = false;
    // Calc dew point if no error reading AM2320 sensor
    // simple calc = dewPointAmbient = temperatureAmbient - ((100 - humidityAmbient)/5.0);
    // this is the more complex calc
    logEx = 0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
    dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);    
  }
}
#endif
