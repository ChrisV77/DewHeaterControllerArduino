// set the heater output pin value
void outputToHeater(int theChan, int thePowerPercentLevel) {
  int thePWMlevel;

  // map the 0-100% power to 0-255 PWM level for digital pins
  thePWMlevel = map(thePowerPercentLevel, 0, 100, 0, 255);
  // write the heater's duty cycle to the heater
  analogWrite(heaterPin[theChan] , thePWMlevel);
}

