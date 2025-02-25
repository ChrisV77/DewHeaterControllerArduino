void printAM2320data() {
  Serial.print("AM2320 I2C ");
  if (errorAM2320) {
    Serial.println("error. ");
  }
  else {
    Serial.print("good."); 
    Serial.print(" Temp: ");
    Serial.print(temperatureAmbient,1);
    Serial.print(" C. ");
    Serial.print("Humidity: ");
    Serial.print(humidityAmbient,1);
    Serial.print("%. ");
    Serial.print("DewPoint: ");
    Serial.print(dewPointAmbient,1);
    Serial.println(" C. ");
  }
}

void printDS18B20data(int theChannel) {
  float theTempDiff;
  Serial.print("DS18B20 pin");
  Serial.print(DS18B20pin[theChannel]);
  Serial.print(". ");

  // check if error reading
  if (errorDS18B20[theChannel]) {
    Serial.print("Read error."); 
  }
  else {
    Serial.print("Read good. Parasitic power: "); 
    if (isItParasitic) Serial.print("ON. ");
      else Serial.print("OFF. ");
    Serial.print("Res: "); 
    Serial.print(deviceResolution[theChannel]);
    Serial.print("bits. "); 
    Serial.print("Temp: ");
    Serial.print(tempSensor[theChannel], 3);
    Serial.print("C. ");

    // and also if AM2320 read OK
    if (!errorAM2320) {
      theTempDiff = tempSensor[theChannel] - dewPointAmbient;
      if (theTempDiff > 0) Serial.print("Above dew point: ");
      if (theTempDiff < 0) Serial.print("Below dew point: ");
      if (theTempDiff == 0) Serial.print("At dew point: ");
      Serial.print(abs(theTempDiff), 1);
      Serial.print("C. ");
    }
  }
  Serial.println();
}
