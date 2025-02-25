/*   
 *    these are the pins being used for all peripherals
 *   temp & temp/humidity sensors, heaters, LCD display
*/

// BME280 for ambient temperature & humidity sensing on I2C - ADAFRUIT libraries
#include <Wire.h>
#include <BME280I2C.h>
BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

// Ambient temperature/humidity sensor. Adafruit AM2320 = I2C.
float   dewPointAmbient;
float   temperatureAmbient;
float   humidityAmbient;
float   pressureAmbient;
float   errorValue;
bool    bmeStatus;
bool    errorAmbientSensor;