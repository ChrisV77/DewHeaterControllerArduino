/*
Test PID with multiple temp steps from ambient using
- single Kpid values, stepping with same gain on each channel: Kpid * gainStepsPID
- does
  - 15 min @ ambient (ambient temp), then repeats
  - repeats
    - 15 min @ ambient (ambient temp)
    - 15 min @ step up (ambient temp + 3C)
  - turn off heaters
- use PID_simple_testing app - this will monitor & write it to a csv file <PID_date.txt> in the public folder
*/

#include "globals.h"

// PID stepping/incrementin parameters
const int numSteps = 5;     // number of gain values in below array
const double gainStepsPID[numChannels][numSteps] = 
{ 
  {1, 2, 4, 6, 8},        // incrementing gain, currently same for both channels 1 & 2
  {1, 2, 4, 6, 8}
};
double Kvalues[3] = { 10, 10, 0 };  // baseline values of Kp, Ki, Kd
double tempStep = 3.0;    // step size from ambient

void setup()
{
  Serial.begin(9600);

  for(int i = 0; i < numChannels; i++) {
    pinMode(heaterPin[i], OUTPUT);                      // define PWM Output on pins
    PID_control[i].SetMode(AUTOMATIC);                  // PID mode
    PID_control[i].SetOutputLimits(0, 255);             // output limit 0-255
  }

  // reset sensors
  resetAmbientSensor();
  resetDS18B20();
  delay(theDelay);
  cycleDone = false;
  startMillis = millis();
  lastMillis = startMillis;
  
  // print header line
  printHeader();      
  // set tuning by gain = [0]
  setTuning(0);       // set PID tuning 1st element in array
  doBlockPID(0, 20);  // ambient: 15x 1min blockLoops = 15 min
}

void loop() 
{
  if (!cycleDone) {
    // loop thru steps
    for(theStep=0; theStep<numSteps; theStep++){
      setTuning(theStep);
      doBlockPID(0, 20);            // do ambient blockLoops, 20min
      doBlockPID(tempStep, 20);     // do ambient+step blockLoops, 20min
      cycleDone = true;
    }
    // turn off heaters
    analogWrite(heaterPin[0], 0);                       // heaters OFF
    analogWrite(heaterPin[1], 0);
    Serial.println("");
    Serial.println("cycle done.");
  }
}

void setTuning(int theGainArrIndex) {
  int theChan;
  double theKvalues[3];

  for(theChan = 0; theChan < numChannels; theChan++) {
    theKvalues[0] = Kvalues[0] * gainStepsPID[theChan][theGainArrIndex];
    theKvalues[1] = Kvalues[1] * gainStepsPID[theChan][theGainArrIndex];
    theKvalues[2] = Kvalues[2] * gainStepsPID[theChan][theGainArrIndex];
    // set to the new tuning values for PID
    PID_control[theChan].SetTunings(theKvalues[0], theKvalues[1], theKvalues[2]);
  }
}

void doBlockPID(int theStep, int numBlockLoops) {
  // theTemp = temp for et point
  // numBlockLoops = number of loops, each is 1min long
  int theChan;

  blockNum = 0;
  while (blockNum < (blockLoops*numBlockLoops)) {
    getAmbientData();               // get ambient data & heater temp
    getDS18B20data();
    currentMillis = millis();
    if ( (currentMillis - lastMillis) > theDelay ) {
      setPoint = theStep + temperatureAmbient;
      for (theChan=0; theChan < numChannels; theChan++) {
        tempCutOff[theChan] = false;
        PID_control[theChan].Compute();
        if (tempHeater[theChan] > cutOff) {
          PIDout[theChan] = 0;
          tempCutOff[theChan] = true;
        }
        analogWrite(heaterPin[theChan], PIDout[theChan]);
      }
      printData();
      lastMillis = currentMillis;
      blockNum += 1;
    }
  }
}
