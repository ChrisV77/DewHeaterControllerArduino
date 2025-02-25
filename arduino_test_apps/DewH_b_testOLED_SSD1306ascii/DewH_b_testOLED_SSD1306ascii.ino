// Dew Heater Controller - test OLED: 0.96" 128 x 32 display
// using SSD1306ascii library
// - this is simpler and uses less memory than the SSD1306 adafruit library
// - each character is 6*8 pixels = width*height = 6 pixels/col, 8 pixels/row
// - position on screen set as (column, row) with (6,1). NB: diff to SSD1306 as
//    - row numbers used rather than pixels: uRow = 1

#include "Globals.h"    // include the global variables, constants etc

const int uCol = 6;      // standard text unit increment across a row
const int uRow = 1;      // standard text unit increment for each new row

void setup() {
  Serial.begin(9600);
  delay(1000); // wait for the OLED to power up
  setupOLEDascii();
}

void loop() {
  // simeple test for setting cursor and printing characters
  display.clear();  
  display.setInvertMode(1);
  for (int i = 0; i <= 2; i++) {
    if (i == 0) display.setInvertMode(1);   // inverted text colour
    if (i > 0) display.setInvertMode(0);    // normal text colour
    for (int j = 0; j <= 9; j++) {
     display.setCursor((j*uCol),(i*uRow)); 
      // display.setCursor((i*uRow),(j*uCol)); 
      display.print(j);
      delay(50);
    }
  }
  display.setCursor((0*uCol),(3*uRow)); 
  // display.setCursor((3*uRow),(0*uCol)); 
  display.print("0123456789");
  delay(1000);
}
