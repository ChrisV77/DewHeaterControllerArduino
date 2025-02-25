// OLED1306Ascii
#define OLED1306_ascii

#ifdef OLED1306_ascii
  #include <Wire.h>
  #include "SSD1306Ascii.h"
  #include "SSD1306AsciiAvrI2c.h"

  // SSD1306ascii library
  #define display_address 0x3c
  SSD1306AsciiAvrI2c display;
#endif