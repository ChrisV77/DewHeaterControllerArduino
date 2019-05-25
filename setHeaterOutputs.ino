/* 
 *  These 2x functions:
 * - setHeaterData    = sets the power levels
 * - outputHeaterData = writes to output pins
 */

void setHeaterData(int theChannel) {
  // set output duty cycle for heaters
  float aboveDewPoint;
  float aboveDewPointConstrain;
  int theDutyCycle;

  // OFF mode: set heater duty cycle to 0
  if (heaterMode == 0)
    theDutyCycle = 0;

  // MANUAL mode: set heater duty cycle to manual level
  if (heaterMode == 1) {
    theDutyCycle = 25.5 * heaterManualPower; // output range 0 - 255
  }
  
  // AUTO mode: set heater duty cycle to auto set level if no read errors
  if (heaterMode == 2) {
    if (errorDS18B20[theChannel] || errorAmbientSensor) { 
      // if error reading either AM2320 or DS18B20, set heater duty cycle to manual level
      theDutyCycle = 25.5 * heaterManualPower; // as above for manual
    }
    else { 
      // if no read errors for AM2320 or DS18B20
      aboveDewPoint = tempSensor[theChannel] - dewPointAmbient;
      // restrict between 0 & threshold
      aboveDewPointConstrain = constrain( aboveDewPoint , 0 , dewPointThreshold );
      // PWM 0 - 100% duty cycle EQUIV TO analog 0 - 255
      theDutyCycle = 255 * (( dewPointThreshold - aboveDewPointConstrain ) / dewPointThreshold );
    }
  }
  // set the heater's duty cycle
  heaterDutyCycle[theChannel] = theDutyCycle;
}

void outputHeaterData(int theChannel) {
  int thePWMlevel;

  thePWMlevel = heaterDutyCycle[theChannel];
  // write the heater's duty cycle to the heater
  analogWrite(heaterPin[theChannel] , thePWMlevel);
}
