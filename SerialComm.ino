// send data to serial output

#ifdef SERIALCOMM_ON
void printAmbientData() {
  Serial.println("");
  Serial.print("Ambient: ");
  if (errorAmbientSensor) {
    Serial.println("no sensor. ");
  }
  else {
    Serial.print("Temp = ");
    Serial.print(temperatureAmbient,1);
    Serial.print("C. ");
    Serial.print("Humidity = ");
    Serial.print(humidityAmbient,0);
    Serial.print("%. ");
    Serial.print("DewPoint = ");
    Serial.print(dewPointAmbient,1);
    Serial.println("C. ");
  }
}

void printHeaterDS18B20data(int theChannel) {
  Serial.print("Heater");
  Serial.print(theChannel+1); 
  Serial.print(": "); 
  // check if error reading
  if (errorDS18B20[theChannel]) {
    Serial.print("no DS18B20, "); 
  }
  else {
    Serial.print("DS18B20 "); 
    Serial.print("temp = ");
    Serial.print(tempSensor[theChannel], 1);
    Serial.print("C. ");
  }
  if (heaterMode==1) Serial.print("Manual ");
  if (heaterMode==2) Serial.print("Automatic ");
  Serial.print("heater power = ");
  Serial.print(int(100 * heaterDutyCycle[ theChannel ] / 255));
  Serial.print("%. ");
  Serial.println();
}

void getCommand() {
  // routine to allow serial control of mode and power
  // enter the single character command <enter> in the serial monitor
  // o/m/a = change to off/manual/automatic mode
  // r = reset the device and go back to automatic mode (with 0 manual power)
  // +/- = increase/decrease the manual power level
  while (Serial.available() > 0) {
    char incomingCharacter = Serial.read();
    switch (incomingCharacter) {
    // mode: 0=Off 1=Manual 2=Auto 3=Reset
      case 'o':
        heaterMode = 0;       // off mode
        break;
      case 'm':
        heaterMode = 1;       // manual mode
        break;
      case 'a':
        heaterMode = 2;       // automatic mode
        break;
      case 'r':       // reset - goes to auto and 0 manual power (defaults)
        resetAmbientSensor();
        resetDS18B20();
        heaterMode = defaultHeaterMode;
        heaterManualPower = defaultHeaterManualPower;
        break;
      case '+':
        if (heaterManualPower < 10) heaterManualPower += 1;
        break;
      case '-':
        if (heaterManualPower > 0) heaterManualPower -= 1;
        break;
    }
  }
}
#endif
