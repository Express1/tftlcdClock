TFT LCD Arduino Clock - Just another Arduino Clock...

This one is using a 320X240 ILI9341 tft lcd connected over SPI, the speed is acceptable for a clock display and will not use as many I/O pins as a parallel connected one.
Here is a link for the display:
https://www.aliexpress.com/item/32889801161.html?spm=a2g0s.9042311.0.0.29594c4dvsZfeP

The clock has a Digital display mode and a Binary BCD display mode selectabe from the menu.

Other modules used: DS3231 Clock module, remove the 1N4148 diode if you use that with a CR2032 battery, we do not want to charge the battery. DS18B20 temperature sensor module for better temp readings - there is a temperature sensor inside DS3231 also but the chip will heat up once powered on and will show about 1-2C more than the actual temp in few minutes.

The LCD I/O is 3v3 and therefore we use some resistors to convert from 5V to 3v3. There are 3 buttons for setup and a speaker for alarm. A piezo  or electromagnetic buzzer can be used.

The clock has an enclosure and a stand designed in OpenSCAD, make one using your 3DPrinter.

The menu of this clock is designed around a struct in PROGMEM in order to use less RAM, for those interested this can be reused as a menu for other projects. It was an interesting exercise to create and is working quite nice with the three buttons: "-", "Setup" and "+". Can be easily extended as necessary.

All the libraries used are in a zip file (unzip under your libraries folder), the font file form Adafruit_GFX is modified “ char is replaced by a full circle character used by the BCD clock display and the second half of the table is removed in order to save space.

IOAbstraction lib is used to provide the low level functions for buttons and event scheduler, many thanks to Dave!
https://www.thecoderscorner.com/products/arduino-libraries/io-abstraction/ioabstraction-pins-io-expanders-shiftreg/




