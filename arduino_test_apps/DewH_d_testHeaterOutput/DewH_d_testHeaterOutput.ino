// NUMBER OF HEATER CHANNELS. choose from 1 - 4 channels/heaters
const int   numChannels = 2;
const int   heaterPin[]  = {9, 10, 5, 6};       // MOSFET heater output channels on PWM pins. 4 possible channels = pins 9, 10, 5, 6
const bool  printIt = false;                    // serial print the power levels if you want to see them while looping

int loopNumber = 1;

// channel cycle variables
int       outputPower[numChannels];     // % power to heater: 0 - 100 %


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // setup mode pin & initialise power to 0%
  for (int currChan=0; currChan < numChannels; currChan++) {
    pinMode(heaterPin[currChan], OUTPUT);                   // set the heater output pin as output
    outputPower[currChan] = 0;                              // initially 0%
    outputToHeater(currChan, outputPower[currChan]);        // write 0% power to outputs
  }
  if (printIt) Serial.println("");

}

void loop() {
  if (printIt) {
    Serial.print("Loop");
    Serial.print(loopNumber);
    Serial.println(" power (%):");
  }

  // put your main code here, to run repeatedly:
  for (int currChan=0; currChan < numChannels; currChan++) {
    outputToHeater(currChan, 0);                              // write 0% power to outputs
  if (printIt) {
    Serial.print("Chan");
    Serial.print(currChan);
    Serial.print(": ");
  }
    for (int i=0; i <= 20; i++) {
      if (printIt) {
        Serial.print(i*5);
        Serial.print(" ");
      }
      outputPower[currChan] = i*5;                           // increment power from 0 - 80%
      outputToHeater(currChan, outputPower[currChan]);        // write 0% power to outputs
      delay(100);
    }
    for (int i=20; i >= 0; i--) {
      if (printIt) {
        Serial.print(i*5);
        Serial.print(" ");
      }
      outputPower[currChan] = i*5;                           // increment power from 0 - 80%
      outputToHeater(currChan, outputPower[currChan]);        // write 0% power to outputs
      delay(100);
    }
    outputToHeater(currChan, 0);                              // write 0% power to outputs
    if (printIt) {
      Serial.println("");
    }
  }
  loopNumber +=1;
}
