// heater temp data from DS18B20 sensors

void resetDS18B20() {
  if (numChannels >= 1) {
    sensor0.begin();
    sensor0.setResolution(resDS18B20);
  }
  if (numChannels >= 2) {
    sensor1.begin();
    sensor1.setResolution(resDS18B20);
  }
  if (numChannels >= 3) {
    sensor1.begin();
    sensor1.setResolution(resDS18B20);
  }
  if (numChannels == 4) {
    sensor1.begin();
    sensor1.setResolution(resDS18B20);
  }
}

void getDS18B20data(int theChannel) {
  // zero the PWM output to heaters while talking to DS18B20s
  // as causing interefrences if DS18B20 & heater lines close
  analogWrite(heaterPin[theChannel] , 0);                 
  
  // restart the library if last pass of DS18B20 had a read error
  if (errorDS18B20[theChannel]) {
    if (theChannel == 0) sensor0.begin();
    if (theChannel == 1) sensor1.begin();
    if (theChannel == 2) sensor2.begin();
    if (theChannel == 3) sensor3.begin();
    delay(750/ (1 << (12-resDS18B20)));
  }

  // request temperature, calc delay needed for read/conversion
  if (theChannel == 0) sensor0.requestTemperatures();
  if (theChannel == 1) sensor1.requestTemperatures();
  if (theChannel == 2) sensor2.requestTemperatures();
  if (theChannel == 3) sensor3.requestTemperatures();
  delay(750/ (1 << (12-resDS18B20)));

  // get temperature in celsius
  if (theChannel == 0) tempSensor[0] = sensor0.getTempCByIndex(0);
  if (theChannel == 1) tempSensor[1] = sensor1.getTempCByIndex(0);
  if (theChannel == 2) tempSensor[2] = sensor2.getTempCByIndex(0);
  if (theChannel == 3) tempSensor[3] = sensor3.getTempCByIndex(0);
  delay(750/ (1 << (12-resDS18B20)));
  outputHeaterData(theChannel);                   // turn the PWMs back on now as turned off before reading DS18B20

  // save new error status
  if (tempSensor[theChannel] == DEVICE_DISCONNECTED_C)
    errorDS18B20[theChannel] = true;
  else
    errorDS18B20[theChannel] = false;
}
