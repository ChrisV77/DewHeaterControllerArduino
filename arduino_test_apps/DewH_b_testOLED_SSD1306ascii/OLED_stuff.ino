#ifdef OLED1306_ascii

void setupOLEDascii() {
  display.begin(&Adafruit128x32, display_address);
  display.setFont(Adafruit5x7);
}

#endif
