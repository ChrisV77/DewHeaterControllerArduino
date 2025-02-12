# Arduino_Dew_Heater_Controller
This is a relatively easy to build for an Arduino based automatic dew heater controller for telescopes. It *automatically* heats the optics on your telescope to stop the dreaded dew.

In this build, the power drive to the heater is automatically controlled by two modes
- *AUTO-heater mode*: automatic control driven by the difference between the ambient dew point and the current temperature of the heater on your lens/mirror (PID feedback control)
- *AUTO-ambient mode*: automatic control driven the difference between ambient temperature and dew point (not feedback control).
Auto-heater is much better but requires and extra sensor and wiring to the heater.

Heater power can also be set manually if desired, or turned off. These two modes also act as default modes in case of sensor read errors, ot heater exceeding a cutoff tempertaure. If these are all you want - this project is overkill.

The device can be controlled
- by a PC using the included windows app
- or by one button on the Arduino controller in conjunction with a display on the device
Both display current ambient and heater readings, plus the setting for the mode being used

At present an Arduino Uno or Nano can be used, with a:
- Display: either a 16x2 LCD, or a 128x64 OLED. The display is essential if using mode control with a button on the Arduino controller
- ambient temperature/humidity sensor: a BME280 or AM2320 (BME280 is better)
- heater temperature sensor: a DS18B20 in parasitic mode for each heater channel (supports up to 4x channels)
- MOSFET driver for each heater channel
- heater straps: you can use commercially available dew straps, or you can make your own with nichrome wire. I have included instructions for this. If using AUTO-heater mode then the heater strap will reqiuire an extra 2-wires for the temperature sensor.
- I power it all from a standard 12V battery used for telescopes.

All these can be attached to the Arduino through extension I/O boards. So not much soldering invovled:
- pure wiring if using Nano with a terminal block expansion board
- soldering required if using an Uno with a PCB protoboard.

The included files are:
- *Arduino code*: this folder contains the files to install on the Arduino
- *windowsapp*: this contains the windows app if running and display on a PC.
- *instructions_v6.pdf*: contains the rationale for the device, build options, assembly isntructions, and software installation & use.

