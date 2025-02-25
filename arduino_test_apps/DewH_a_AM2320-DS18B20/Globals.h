/*   
 *    these are the pins being used for all peripherals
 *   temp & temp/humidity sensors, heaters, LCD display
*/

// 16x2 LCD display = I2C.
// nothing to define here

// Ambient temperature/humidity sensor. Adafruit AM2320 = I2C.
float   dewPointAmbient;
float   temperatureAmbient;
float   humidityAmbient;
boolean errorAM2320;

// 2x channels with Heaters & can have DS18B20 temp sensors
//  - comes from - https://github.com/milesburton/Arduino-Temperature-Control-Library
const int numChannels   = 2;
const int DS18B20pin[]  = {7, 8};      // on pins 7 & 8
const int heaterPin[]   = {9, 10};      // MOSFET Driver modules = Heater Outputs 1-2 on PWM pins 9 & 10.
// DS18B20s
float     tempSensor[numChannels];     // DS18B20 sensor temperatures from sensors1/2
boolean   errorDS18B20[numChannels];   // true if error reading channel temp
const int resDS18B20    = 11;           // resolution of DS18B20 read. I think 9-12 = 0.5 - 0.125 degC
