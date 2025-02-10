# Arduino Dew Heater Controller
This is a relatively easy to build for an Arduino based automatic dew heater controller for telescopes. It AUTOMATICALLY heats the lens/mirror on your telescope to stop the dreaded dew.

In this build, the power drive to the heater is automatically controlled by 
- the difference between the ambient dew point and the current temperature of the heater on your lens/mirror (auto-HEATER MODE, uses PID feedback control)
- or more simply by the difference between ambient temperature and dew point (AUTO-ambient mode, not feedback control). 
The heater power can also be set manually if desired, or turned off. These two modes also act as default modes in case of sensor read errors, ot heater exceeding a cutoff tempertaure.

The device can be controled
- by a PC using the included windows app
- or by one button on the Arduino controller

At present an Arduino Uno or Nao can be used. Supported devices are:
- Display: either a 16x2 LCD, or a 128x64 OLED. The display is essential if using mode control with a button on the Arduino controller
- ambient temperature/humidity sensor: a BME280 or AM2320 (BME280 is better)
- heater temperature sensor: a DS18B20 in parasitic mode for each heater channel (supports up to 4x channels)
- MOSFET driver for the channel heaters

All these can be attached to the Arduino through extension I/O boards. So not much soldering invovled
For details on the build read  instructions_v6.PDF. This has instructions on the build, tesing etc.

You can use commercially available dew straps (although I haven’t tested my system with these, so no responsibility taken), or you can make your own with nichrome wire. I have included instructions for this. I power it all from a standard 12V battery used for telescopes.
