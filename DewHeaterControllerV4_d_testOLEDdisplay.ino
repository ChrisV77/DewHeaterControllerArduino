// Dew Heater Controller - test OLED display
// for v4.0

// the display = 128*64 I2C
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

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
}

void loop() {
  // Dislay auto mode parameters
  display.clearDisplay();     
  display.display();
  delay(theDelay*3);
  display.setCursor(0,0); 
  display.print("Test OK");
  display.display();
  delay(theDelay*3);
}

