/*
Test PID with multiple temp steps from ambient using
- parameters: single base Kpid values, but different gainPID values for each channel & each successive step
  - so for each channel Kpid = base Kpid * gainPID[for each channel & step]
- does
  - baseline 30 min @ ambient then repeats steps
    - 20 min @ ambient (ambient temp+3C step)
    - 30 min @ step up (ambient temp)
  - then turn off heaters
- use PID_simple_testing app - this will monitor & write it to a csv file <PID_date.csv> in the public folder
*/

#include "globals.h"
// timing
int periodStartBaseline = 30; // timing of steps (min)
int periodStepUp = 20;
int periodStepDown = 30;

// stemp from ambient temp
double tempStep = 3.0;        // step size from ambient +C

// PID stepping/incrementin parameters
const int numSteps = 4;     // number of gain values in below array
const double gainStepsPID[numChannels][numSteps] = { 
  {1, 2, 4, 6},         // gain during each step for channel 1 (0)
  {1, 2, 4, 6}          // gain during each step for channel 2 (1)
};
double Kvalues[3] = { 10, 10, 0 };  // baseline values of Kp, Ki, Kd

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
  setTuning(0);                         // set PID tuning 1st element in array
  doBlockPID(0, periodStartBaseline);   // ambient: 30x 1min blockLoops = 30 min
}

void loop() 
{
  if (!cycleDone) {
    // loop thru steps
    for(theStep=0; theStep<numSteps; theStep++){
      setTuning(theStep);
      doBlockPID(tempStep, periodStepUp);              // do ambient blockLoops, 20min
      doBlockPID(0, periodStepDown);     // do ambient+step blockLoops, 20min
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
