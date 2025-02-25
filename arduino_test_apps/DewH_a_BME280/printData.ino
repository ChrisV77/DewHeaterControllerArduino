void printBME280data() {
  Serial.print("BME280 I2C ");
  if (errorAmbientSensor) {
    Serial.println("error. ");
  }
  else {
    Serial.print("good."); 
    Serial.print(" Pressure:: ");
    Serial.print(pressureAmbient,1);
    Serial.print(" Pa. ");
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
