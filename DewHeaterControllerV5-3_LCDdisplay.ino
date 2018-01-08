// Dew Heater Controller
// v5.3 using 0.96" 128x64 display, using I2C
// since v5.1 - in auto mode, allow to set manual for other channels

// global constants 
const int numHeaters = 2;           // max number of heaters
const float tempCutOff = 32;        // heater cut-off if too hot
const float dewPointThreshold = 6;  // threshold above dew point to start heater

// these are the pins being used for sensors, heaters etc
// DHT22 = digital 2. Ambient temperature & humidity
// DS18B20 = digital 10, 11, 12, 8. Sensor 1-4 temperature
// Heater Outputs = digital 3, 5, 6, 9. To MOSFET heater drivers for DS18B20 sensors 1-4
// Control switch = digital 4. To control stuff - switch between heater control = feedback or manual

// Control switch
int controlSwitchPin = 4;           // control switch pin
boolean changeMode = false;			// change to MODE

int heaterMode = 0;                 // mode: 0=Auto 1=OFF 2=Manual
int heaterManualPower = 0;          // power level if in Manual mode OR Auto & no sensor connected

// the display = 128*64 I2C
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// DHT22 for ambient temperature & humidity sensing
// on pin 2
#include <DHT.h>
#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
#define DHTPIN 2    // the DHT22 pin
DHT dht(DHTPIN, DHTTYPE);
float dewPointAmbient;
float temperatureAmbient;
float humidityAmbient;
boolean errorDHT22;

// MOSFET Driver modules = Heater Outputs 1-4 on PWM pins
int heaterDutyCycle[] = {0, 0, 0, 0};       // drive to heater
int heaterPin[] = {3, 5, 6, 9};             // heater pins

// DS18B20 for sensing temperature on heated elements, using Onewire library
// Have separate pin for each DS18B20. Easier to wire up and distinguish
#include <OneWire.h>
#include <DallasTemperature.h>
int DS18B20Pin[] = {10, 11, 12, 8};  // now have 3x DS18B20 temp sensors
OneWire oneWire1(DS18B20Pin[0]);
OneWire oneWire2(DS18B20Pin[1]);
OneWire oneWire3(DS18B20Pin[2]);
OneWire oneWire4(DS18B20Pin[3]);
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
DallasTemperature sensor3(&oneWire3);
DallasTemperature sensor4(&oneWire4);
boolean errorDS18B20[numHeaters];
float tempHeater[numHeaters];          // 4x heaters max

// general - delay for sensor and "i" for counting
int theDelay = 1000;          // need ~1sec delay after DS18B20 read in parasitic mode
int i;

void setup() {
	// initialize display
	// with the 128*64 SSD1306 display 
	// I2C addr 0x3C. But some might be 0x3D
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
	display.setTextColor(WHITE);
	display.setTextSize(1);

	// Display intro
	display.clearDisplay();     
	display.setCursor(16,0); 
	display.print("DEW HEATER: v5.3");
	display.setCursor(16,16); 
	display.print("AUTO & MAN MODES");
	display.display();
	delay(theDelay*3);

	// Dislay auto mode parameters
	display.clearDisplay();     
	display.setCursor(0,0); 
	display.print("Start in AUTO mode");
	display.drawRect(0, display.height()*0.35, display.width(), display.height()*0.05, WHITE);
	display.setCursor(16,16); 
	display.print("Threshold: ");
	display.print(dewPointThreshold,0);
	display.write(9);
	display.print("C");
	display.setCursor(16,24); 
	display.print("   Cutoff: ");
	display.print(tempCutOff,0);
	display.write(9);
	display.print("C");
	display.display();
	delay(theDelay*3);
  
	// mode selection PIN & display mode changes
	heaterMode = 2;            // set initial heater mode to 2=automatic mode
	pinMode(controlSwitchPin, INPUT);
	display.clearDisplay();     
	display.setCursor(0,0); 
	display.print("Press button to");
	display.setCursor(0,8); 
	display.print("change heater mode");
	display.setCursor(0,20); 
	display.print("OFF - MANUAL - AUTO");
	display.display();
	delay(theDelay*3);

	// setup heater settings and set to zero
	for (i = 0; i < numHeaters; i++) {
		pinMode(heaterPin[i], OUTPUT);  // sets the pin as output
		heaterDutyCycle[i] = 0;
		analogWrite(heaterPin[i] , heaterDutyCycle[i]);
	} 
}

void loop() {
	// Read ambient temperature & humidity from DHT22. Calculate dew point if no error reading. Display ambient data
	errorDHT22 = getDHT22data();
	displayAmbientData();
  delay(theDelay);

	// cycle through the heaters
	for (i = 0; i < numHeaters; i++) {
    // check for mode change (press button to change mode)
    changeMode = changeControlMode();
    if (changeMode) {
      changeManualPower();
    }
		// read heaterSensor temp - return error if not connected
		errorDS18B20[i] = getDS18B20data(i);
		// set heater output - depends on heaterMode 0=OFF 1=MAN 2=AUTO
		setHeaterOutput(i);
		// display heater data - if no error in reading ambient or heater not OFF
		if (!errorDHT22 && (heaterMode != 0) ) {
			displayHeaterData(i);
		}
    delay(theDelay*2);
	}
}

boolean getDHT22data() {
	float logEx,dewPoint;
	boolean anError;
	// DHT22 - Reading temperature or humidity 
	// takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	humidityAmbient = dht.readHumidity();
	// Read temperature as Celsius (the default)
	temperatureAmbient = dht.readTemperature();
	delay(theDelay);

	if (isnan(temperatureAmbient) || isnan(humidityAmbient)) {
		// if error reading DHT22 set all to:
		temperatureAmbient = 100;
		humidityAmbient = 100;
		dewPointAmbient = 100;  
		anError = true;
	}
	else {
		// Calc dew point if no error reading DHT22
		// simple calc = dewPointAmbient = temperatureAmbient - ((100 - humidityAmbient)/5.0);
		// more complex calc
		logEx=0.66077 + 7.5*temperatureAmbient/(237.3+temperatureAmbient) + (log10(humidityAmbient) - 2);
		dewPointAmbient = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);    
		anError = false;
	}
	return anError;
}

boolean changeControlMode() {
	boolean changeMode = false;
  
	// Loop #1: cycle through 3 main heater modes OFF-MANUAL-AUTO to select
	while (digitalRead(controlSwitchPin) == HIGH) {
		changeMode = true;
		// display prompt
		display.clearDisplay();
		display.setCursor(0,0); 
		display.print("Release button to");
		display.setCursor(0,8); 
		display.print("select heater mode:");

		// cycle through modes
		heaterMode += 1;
		if (heaterMode > 2) {
			heaterMode = 0;
		}
		// display the main mode
		display.setCursor(12,18); 
		switch (heaterMode) {
			case 0:
				display.print("OFF    ");
				break;
			case 1:
				display.print("MANUAL ");
				break;
			case 2:
				display.print("AUTO   ");
			  break;
		}
		display.display();
		delay(theDelay*1.5);
	}
	return changeMode;
}

void changeManualPower() {
	boolean changeHeaterPower = false;
    if (heaterMode == 0) {
      heaterManualPower = 0;
    }
    else {
  		display.print("Selected");
	  	display.display();
		  while (digitalRead(controlSwitchPin) == HIGH) {
		  	// delay for release of the button
			  delay(theDelay);
		  }
		  delay(theDelay);
			// cycle through manual power levels if mode = Manual OR Auto
			while (changeMode && (heaterMode != 0) && (digitalRead(controlSwitchPin)==LOW)) {
				// display prompt
				display.clearDisplay();
				display.setCursor(0,0); 
				display.print("Press button to");
				display.setCursor(0,8); 
				display.print("select manual power:");

				// cycle power levels
				heaterManualPower += 1;
				if (heaterManualPower > 5) {
					heaterManualPower = 0;
				}
				// display the manual heater power
				display.setCursor(12,18); 
				switch (heaterManualPower) {
					case 0:
						display.print("0%   ");
						break;
					case 1:
						display.print("20%  ");
						break;
					case 2:
						display.print("40%  ");
						break;
					case 3:
						display.print("60%  ");
						break;
					case 4:
						display.print("80%  ");
						break;
					case 5:
						display.print("100% ");
						break;
				}
				display.display();
				delay(theDelay*1.5);
			}
		}
		display.print("Selected");
		display.display();
		while (digitalRead(controlSwitchPin) == HIGH) {
			delay(theDelay);
		}
}

boolean getDS18B20data(int theHeater) {
	boolean anError = false;
	// for each heater start the sensor, read temperatures & save to global array tempHeater

	switch (theHeater) {
		case 0:
			sensor1.begin();
			sensor1.requestTemperatures();
			tempHeater[theHeater] = sensor1.getTempCByIndex(0);
			break;
		case 1:
			sensor2.begin();
			sensor2.requestTemperatures();
			tempHeater[theHeater] = sensor2.getTempCByIndex(0);
			break;
		case 2:
			sensor3.begin();
			sensor3.requestTemperatures();
			tempHeater[theHeater] = sensor3.getTempCByIndex(0);
			break;
		case 3:
			sensor4.begin();
			sensor4.requestTemperatures();
			tempHeater[theHeater] = sensor4.getTempCByIndex(0);
			break;
	}
	if (tempHeater[theHeater] == -127) {
		anError = true;
	}
	return anError;
}

void setHeaterOutput(int theHeater) {
	// set output duty cycle for heaters
	float aboveDewPoint;
	float theDutyCycle = 0;

	if (errorDHT22 || (heaterMode == 0)) {
		// OFF MODE or error reading DHT22=ambient
		theDutyCycle = 0;
	}
	else {
		if (heaterMode == 1) {
      // MAN mode - set heater to manual level
      theDutyCycle = 51 * (heaterManualPower);
      if (!errorDS18B20[theHeater] && ( tempHeater[theHeater] >= tempCutOff ) ) {
        // if sensor connected and over cut-off switch heater OFF
        theDutyCycle = 0;
      }
		}
		if (heaterMode == 2) {
			// AUTO MODE
			if (errorDS18B20[theHeater]) {
				// Heater to manual level if DS18B20 not connected
				theDutyCycle = 51 * (heaterManualPower);
			}
			else {
				if ( tempHeater[theHeater] >= tempCutOff )  {
					// Heater OFF if above cut-off
					theDutyCycle = 0;
				}
				else  {
					// Heater ON if below cut-off
					// if all OKAY calculate dew point and duty cycle
					aboveDewPoint = tempHeater[theHeater] - dewPointAmbient;
					// restrict between 0 & threshold
					aboveDewPoint = constrain( aboveDewPoint , 0.0 , dewPointThreshold );
					// PWM 0 - 100% duty cycle EQUIV TO analog 0 - 255
					theDutyCycle = 255 * (( dewPointThreshold - aboveDewPoint ) / dewPointThreshold );
				}
			}
		}
	}
	// set the heater's duty cycle & write to the heater
	heaterDutyCycle[theHeater] = theDutyCycle;
	analogWrite(heaterPin[theHeater] , heaterDutyCycle[theHeater]);   
}

void displayAmbientData() {
	display.clearDisplay();
	display.setCursor(16,0); 
	display.print("AMBIENT");
	display.setCursor(44,8); 
	display.print("Temp:");
	display.setCursor(20,16); 
	display.print("Humidity:");
	if (errorDHT22) {
		// error reading DHT22
		display.setCursor(80,8); 
		display.print("error");
		display.setCursor(80,16); 
		display.print("error");
	}
	else {
		// if ambient read okay display DHT22 data
		display.setCursor(80,8); 
		display.print(temperatureAmbient, 0);
		display.write(9);
		display.print("C");
		display.setCursor(80,16); 
		display.print(humidityAmbient, 0);
		display.print("%");
		display.setCursor(20,24); 
		display.print("DewPoint: ");
		display.setCursor(80,24); 
		display.print(dewPointAmbient, 0);
		display.write(9);
		display.print("C");
	}
	display.display();
}

void displayHeaterData( int theHeater ) {
	float thePerCentDutyCycle;

	// display the heater number
	display.clearDisplay();
	display.setCursor(0,0); 
	display.print("HEATER ");
	display.print(theHeater+1);
	display.print(": ");

	// display the heater temp (if not a temp read error)
	if(errorDS18B20[i]) {
    display.print("no sensor");
	}
  else {
    display.print(tempHeater[ theHeater] , 0);
	  display.write(9);
	  display.print("C");
    display.print("(+");
    display.print(tempHeater[ theHeater] - dewPointAmbient , 0);
    display.print(")");
  }  
  
	// display the % power (duty cycle)
	thePerCentDutyCycle = 100 * heaterDutyCycle[ theHeater ] / 255;
	display.setCursor(4*12,8); 
	uint8_t color = WHITE;
	display.drawRect(0, display.height()*0.35, display.width()*0.02, display.height()*0.3, WHITE);
	display.drawRect(display.width()*0.23, display.height()*0.35, display.width()*0.02, display.height()*0.3, WHITE);
	display.drawRect(display.width()*0.48, display.height()*0.35, display.width()*0.02, display.height()*0.3, WHITE);
	display.drawRect(display.width()*0.73, display.height()*0.35, display.width()*0.02, display.height()*0.3, WHITE);
	display.drawRect(display.width()*0.98, display.height()*0.35, display.width()*0.02, display.height()*0.3, WHITE);
	display.fillRect(0, display.height()*0.4, display.width()*thePerCentDutyCycle/100, display.height()*0.2, WHITE);
	display.setCursor(0,24); 
	display.print("Power: ");    
	display.print(thePerCentDutyCycle , 0);
	display.print("% ");
  if(!errorDS18B20[i] && (heaterMode == 2) ) {
      display.print("AUTO ");
  }
  if(errorDS18B20[i] || (heaterMode == 1) ) {
      display.print("MAN ");
  }
  if (tempHeater[ theHeater] >= tempCutOff) {
    // if above cut-off
    display.print("CUTOFF");
  }
	display.display();
}

