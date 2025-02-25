void resetAmbientSensor() {
  am2320.begin();
}

void getAmbientData() {
  float logEx;

  // read ambient temp and humidity - takes 100s of msec?.
  humidityAmbient = am2320.readHumidity();
  temperatureAmbient = am2320.readTemperature();
  logEx = 0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
  dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);    

  delay(100);
}
