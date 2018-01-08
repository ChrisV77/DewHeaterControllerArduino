# TelescopeDewHeaterController
This is a relatively easy to build for an Arduino based automatic dew heater controller for telescopes. It heats the lens/mirror on your telescope to stop the dreaded dew. The build is based on the excellent Dew Heater controller built in IceInSpace 2010 by Bob Stephens at
http://www.iceinspace.com.au/63-597-0-0-1-0.html

In this build, the power drive to the heater is automatically controlled by the difference between the ambient dew point and the current temperature of the heater on your lens/mirror. The heater power can also be set manually if desired. It also uses a 128x64 OLED display to show all the data. It would be simple to convert this to a more basic 20x2 or 20x4 LCD display (I will do this eventually).

To make life simpler my project uses the Arduino nano with an extension I/O board. So all the components CAN BE attached with pre-made connectors. There is NO circuit board making. There is NO soldering onto circuit boards. The ONLY soldering is for the plugs and sockets to connect it to the outside world. It is also inexpensive.

For details on the build read the Dew_heater_detailos PDF.

There are 4x Arduino files:
-	3x test files to check the DHT22 sensor, DS18B20 sensor and then both together. These assume the display is working.
-	The DewHeaterControlledVx-x file contains the working code for the complete system.

You can use commercially available dew straps (although I haven’t tested my system with these, so no responsibility taken), or you can make your own with nichrome wire. I have included instructions for this. I power it all from a standard 12V battery used for telescopes.
