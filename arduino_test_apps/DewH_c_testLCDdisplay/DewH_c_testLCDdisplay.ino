// LCD display 16 x2. On I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F); // 0x3F or 0x27 is the I2C bus address for 16x2 LCD displays.

void setup() {
  lcd.begin(16,4);
  // lcd.backlight();   //open the backlight
  lcd.noBacklight();
}

void loop() {

  // for (int i = 0; i <= 12; i++) {
  //   lcd.clear();
  //   for (int j = 0; i <= 1; i++) {
  //     delay(150);
  //     lcd.setCursor(j,i);
  //     lcd.print("_no_");
  //     delay(150);
  //   }
  // }
  lcd.clear();
  delay(500);
  lcd.print("no_blink");
  delay(500);
}
