# DewHeaterPCinterface_v2
windows app to control arduino based dew heater via USB

Version 2 (v2) updates
- works with Arduino project 'DewHeater_v6' and onwards
- can have 1-4 heater channels. You can only set this in the arduino code (config.h)
- mode settings are global: apply to all heater channels, but temperature read errors (or no temp sensor used) default this to other modes (see below)

USING IN THE APP (all the control boxes)

USB: DEW HEATER. 
- This sets the USB comms.
- Port: select the com port with the arduino dew heater. NB: Dew Heater USB must connected before starting app.
- Open/Close. 
- text will show if disconnected, connected, or waiting for connection. Can take up a few secs to connect (depends on state of dew heater cycle). NB: sometimes might ned to shut down app and restart for connection (have figured out why it does this).

AMBIENT. 
- Readings from the AM2302/BME280 temperature/humidity sensor
- every cycle it update the readings from the ambient sensor
    - temperature (C), humidity (%) & dew point (C)  the arduino updates every 12sec
    - displays the time of sensor read. The dew heater has a cycle time of 20sec (you can change this by editing the arduino code - better if longer as some temperature reads take time).

GLOBAL MODE SETTINGS.
- This set the global mode of all channels. 
    - NB: this is overriden on individual channels when there are read errors, or a sensor not being used (ambient or heater channel sensors, depends on mode)
- after making any mode changes the SET button will go red. You then have to click 'SET' to write to arduino Dew Heater controller, the SET button will then go black again.
- There are 4x Modes: 1=Off 2=Manual power 3=AUTO-ambient 4=AUTO-heater. Mode settings will not be accesible above the one you are using (e.g. if using manual mode 2 you won't be able to alter those for 3 or 4).
- The modes:

	1. Off: turns heater(s) off.

	2. Manual: set the heater power manually
		- This is basic. Making this project is overkill if this is all you want. There are cheaper and simpler alternatives around.
		- SETTINGS:
    		- Manual Power (%) - set level. Allowed values from 0 - 100%. I generally have it at 10-30 % (its surprising how little power you need to warm you optics a couple of degrees). DON'T OVERDO IT !
		- You can figure out the actual power for 100%: Power (W) = 12V * Resistance of your heater (ohms). You'll have figure this out by trial-and-error in bad dew point conditions. It depends upon humidity and your heater element.
		- DEFAULTS:
			- if heater sensor > cut-off value, channel will default to OFF (see cut-off in extra settings below)

	3. Auto-ambient: sets heater power according to ambient relative humidity (RH). 
		- Requires: 1x ambient sensor.
		- This is the first level of automation.
		- SETTINGs:
			- Max Pwr (%) - the maximum power. I use ~20 - 30%, a value similar to or a bit above the manual power.
			- Ambient threshold = 5C. This is roughly where humidity is increasing enough to potentially start causing dew issues.
		-  It sets heater power between 0% - Max Pwr% (AUTO-ambient). This is determined by how far ambient temp if above the dew point, relative to the ambient threshold value you set, e.g. sets power at:
			- Max % when ambient temp - dew point = 0
			- 0 % when ambient temp - dew point >= ambient threshold
			- Increases from 0 - Max % as ambient temp - dew point decreases from ambient threshold to 0
		- DEFAULTS: 
			- if no ambient sensor or a read error, all channels will default to MANUAL
			- if heater sensor > cut-off value, channel will default to OFF (see cut-off in extra settings below)

	4. Auto-heater: uses feedback controlled from the heater temperature sesnor to set heater power. 
		- Requires: 1x ambient sensor and a heater temperature sensor for each channel.
		- This is the best control. It's why I built this controller. It uses very little power, only enough to stop dew dewing, by keeping the heater temperature a set threshold value above dew point, e.g.
		- SETTINGS:
			- Target Temp (C)' - set heater temp target to a value above dew point. NB: if this is below amient, it will set the target to = ambient - dew point. I usually set it to 5C above dew point. Don't overdo this value !
		- NB:
			- if no heater sensor or a read error, that channel will default to auto-ambient mode (other channels not affected)
			- if no ambient sensor or a read error, all channels will default to MANUAL.
			- if heater sensor > cut-off value, channel will default to OFF (see cut-off in extra settings below)

CHANNELS 1-4.
- This will display the readings and settings for all channels being used. The number of boxes displayed is set by the arduino (can have 1-4 heater channels)
- These display the current setting & readings for each channel during the current read cycle
	- Mode = as set in the Global mode settings BOX, or defaults if read errors (see above for defaults from auto-heater & auto-ambient mode)
	- Heater = the current temperature of the sensor in the heater (if connected)
	- Power Bar = bar line of the current % power (from 0 - 100%)
	- The last box will display one of the following dependning on the mode:
		- OFF/MANUAL: power (%), 0 - 100%
		- AUTO-AMBIENT: the ambient temperature relative to dew point. This is what determines the power level (see above in global mode settings). Any value below the ambient threshold (in global settings) will turn on the heater
		- AUTO-HEATER: Set Point temperature (C). This is the acutal traget temperature value for the heater (dew point + SetPoint for Auto-heater from global settings, or ambient temp if this value is less).

EXTRA SETTINGS.
- temp cut-off: sets the temperature at which the heater cuts-off. NB: only works in AUTO-heater mode if there is a heater sensor.
- Save data file: this saves the dew heater mode, parameters and readings to a file. In: 'c://users/public'. Handy - you can see if any read errors during use, and you can figure out the power used for different modes

