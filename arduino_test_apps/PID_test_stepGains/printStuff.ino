
void printHeader() {
  Serial.println("");
  Serial.print("Temp stepping (C): from ambient to ambient+");
  Serial.print(tempStep,1);
  Serial.println("C: multiple temp steps with a Kpid = gain * base Kpid. Where: ");
  // print current set of PID parameters
  Serial.print("PID K-values: Kp=");
  Serial.print(Kvalues[0],3);    // print the actual K-values being used
  Serial.print(" Ki=");
  Serial.print(Kvalues[1],3);
  Serial.print(" Kd=");
  Serial.print(Kvalues[2],3);
  Serial.println(". Steps with different gains");
  Serial.println("time (min),ambTemp (C),setPoint (C),htr1 (C),htr2 (C),ambHum (%),dewpoint (C),PIDout 1,PIDout 2,gain1,gain2");
} 

void printData() {
  int theChan;

  // print stuff
  Serial.print( float(currentMillis - startMillis)/60000 , 2 );
  Serial.print(",");
  Serial.print(temperatureAmbient,3);
  Serial.print(",");
  Serial.print(setPoint,3);
  for (theChan = 0; theChan < numChannels; theChan++) {
    Serial.print(",");
    Serial.print(tempHeater[theChan],3);
  } 
  Serial.print(",");
  Serial.print(humidityAmbient,1);
  Serial.print(",");
  Serial.print(dewPointAmbient,3);
  for (theChan = 0; theChan < numChannels; theChan++) {
    Serial.print(",");
    Serial.print(PIDout[theChan],0);
  } 
  for (theChan = 0; theChan < numChannels; theChan++) {
    Serial.print(",");
    Serial.print(gainStepsPID[theChan][theStep]);
  } 
  Serial.println();
}
