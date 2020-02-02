/*
   tft lcd Clock Arduino Clock

   Copyright 2019 Radu Constantinescu
   https://github.com/Express1/tftlcdClock

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.
*/

//11/1/2020 fix DOW and DST

//Libraries
//#include <avr/pgmspace.h>
#include<IoAbstraction.h>
#include <EepromAbstraction.h>
#include <ArduinoEEPROMAbstraction.h>

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include "Sodaq_DS3231.h"
#include  <SPI.h>

//DS18B20 temp sensor
#include <OneWire.h>
#include <DS18B20.h>

#define ONE_WIRE_BUS 6

OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensor(&oneWire);
//

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CLK 13

#define PINTFTLED 5
#define PINSPK 3

// used to change the LCD backlight, military time
#define STARTDAY 7
#define STARTNIGHT 22

// buttons
const int menubutton = A1;
const int minusbutton = 2;
const int plusbutton = 4;

//print info to serial
#define debug_s

char buffer[32];  //buffer
char ast[10];  //a string
uint8_t seconds; // seconds from timer chip
uint8_t minutes, hours, hours1; // last min value

uint8_t alarmon, alarmcount; //used durring alarm

uint8_t mode; // 0 = clock, 1 = BCD, 2 = setup
uint8_t menui; //menuitem

AvrEeprom anEeprom;
DateTime now;


//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
//https://forum.developer.sony.com/topic/59/hardware-spi-not-working-for-ili9340-9341-tft-display-sw-spi-does/5
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); //HARDWARE SPI, fast
//Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9, 11, 13, 8); //SOFTWARE SPI, slow


// we define all the strings in PROGMEM

// max# of char in the array+1
const int CHARCOUNT = 10;

/*Su=1 Mo=2 Tu=3 We=4 Th=5 Fr=6 Sa=7 */
const char dayofweek [7] [CHARCOUNT] PROGMEM = {
  { "Sunday" },
  { "Monday" },
  { "Tuesday" },
  { "Wednesday" },
  { "Thursday" },
  { "Friday" },
  { "Saturday" },
};

const char months [12] [CHARCOUNT] PROGMEM = {
  { "January" },
  { "February" },
  { "March" },
  { "April" },
  { "May" },
  { "June" },
  { "July" },
  { "August" },
  { "September" },
  { "October" },
  { "November" },
  { "December" }
};

const char h1224 [2] [CHARCOUNT] PROGMEM = {
  { "24 hour" },
  { "12 hour" }
};

const char clmode [2] [CHARCOUNT] PROGMEM = {
  { "Digi Clk" },
  { "BCD Clk" }
};

const char cls[15] PROGMEM = "              ";
const char alm[5 + 1] PROGMEM = "ALARM";

const char apm [2] [2 + 1] PROGMEM = {
  { "AM" },
  { "PM" }
};

const char wday [2] [6 + 1] PROGMEM = {
  { " Week " },
  { " Day " }
};

const char alarmdays1 [3] [CHARCOUNT] PROGMEM = {
  { "AlarmOff" },
  { "Alarm On" },
  { "Workdays" }
};
const int MENUCOUNT = 18;
uint8_t menuval[MENUCOUNT];   // values for menu items

// menu callback routines

// set time
void settime1 (void) {
  DateTime now = rtc.now(); // keep seconds
  //int year = menuval[5] + 2000;
  //Serial.print(F("YYY"));
  //Serial.println(year);
  DateTime dt((menuval[5] + 2000), menuval[2], menuval[3], menuval[0], menuval[1], now.second(), menuval[4]);
  // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
  mode = 3; //go back to setup
  //Serial.println (F("Routine 1"));
}

void settimemin (void) {
  DateTime now = rtc.now(); // reset seconds 
  DateTime dt((menuval[5] + 2000), menuval[2], menuval[3], menuval[0], menuval[1], 0, menuval[4]);
  // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
  mode = 3; //go back to setup
}



void exitmenu2 (void) {
  mode = 3; //go back to setup
  // write an array to the rom.
  //anEeprom.writeArrayToRom(0, (const unsigned char*)menuval, sizeof menuval);
  //Serial.println (F("Routine 2"));
}

void setexit3 (void) {
  tft.fillRect(0, 0, 240, 140, ILI9341_BLACK);
  clearrow(260, 2);
  clearrow(280, 2);
  seconds--;
  minutes--; // force update sec,min, hour
  hours--;
  mode = menuval[7]; // exit menu, go back to clock
  // write an array to the rom.
  anEeprom.writeArrayToRom(0, (const unsigned char*)menuval, sizeof menuval);
  myclock();
  //Serial.println (F("Routine 3"));
}

void setlcd (void) {
  analogWrite(PINTFTLED, menuval[15]);
  mode = 3; //go back to setup
}

void setlcdn (void) {
  analogWrite(PINTFTLED, menuval[16]);
  mode = 3; //go back to setup
}
// if you want to understand how this does work read
// https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs

struct MenuRecord
{
  uint8_t start;  // start at 0 or 1 or
  uint8_t count;  // how many, last is start+count-1
  char item[12 + 1]; // menu choice text to display
  char (*values)[CHARCOUNT];
  void (*set)();  // setup routine, call if we press set for an item
};

const MenuRecord menu[MENUCOUNT] PROGMEM = {
  { 0, 23, "Time hour", NULL , settime1 }
  , { 0, 59, "Time min", NULL , settimemin }
  , { 1, 12, "Month", months , settime1 }
  , { 1, 31, "Date", NULL, settime1 }
  , { 1, 7, "Day of week", dayofweek, settime1 }
  , { 19, 199, "Year", NULL, settime1 }
  , { 0, 1, "Clock 12/24H", h1224, setexit3 }
  , { 0, 1, "Clock mode", clmode, setexit3 }
  , { 0, 23, "Alarm hour", NULL, exitmenu2 }
  , { 0, 59, "Alarm min", NULL, exitmenu2 }
  , { 0, 2, "Alarm Days", alarmdays1 , exitmenu2 }
  , { 0, 5, "DSTonWeek", NULL , exitmenu2 }
  , { 0, 12, "DSTonMo", NULL , exitmenu2 }
  , { 0, 5, "DSToffWeek", NULL , exitmenu2 }
  , { 0, 12, "DSToffMo", NULL , exitmenu2 }
  , { 10, 255, "LCD Day", NULL , setlcd }
  , { 10, 255, "LCD Night", NULL , setlcdn }
  , { 0, 1, "Exit Setup", NULL, setexit3 }
};


/*
  menuval[0] hour 0-23
  menuval[1] min 0 -59
  menuval[2] month 1-12
  menuval[3] dayofmonth 1-31
  menuval[4] dayofweek 1-7, 1=Monday
  menuval[5] year last 2 digits 19XX
  menuval[6] 0=24h, 1=12h
  menuval[7] 0=digi, 1=bcd
  menuval[8] alarm hour
  menuval[9] alarm min
  menuval[10] alarm days 0=off, 1=on, 2=workdays
  menuval[11] DST on Week
  menuval[12] DST on Mo
  menuval[13] DST off Week
  menuval[14] DST off Mo
  menuval[15] LCD Light Day
  menuval[16] LCD Light Night
*/


void clearrow(int y, int textsize)
{
  //strcpy(buffer, "                ");
  memcpy_P (&buffer, &cls, 15);
  tftcprint(ILI9341_RED, y, textsize);
}

void onmenubutton(uint8_t pin, bool heldDown) {
  if (alarmon == 1) { //alarm stop
    alarmon = 0;
    noTone(PINSPK);
    // clear screen, print year info
    memcpy_P (&buffer, &alm, 6);
    tftcprint(ILI9341_BLACK, 260, 4);
    pyear();
  }
  else
  {
    if (!heldDown) {
      MenuRecord thisItem;
      switch (mode) {
        case 0 :
        case 1 :
          {
            mode = 3;  //setup mode
            menui = 0;
            clearrow(260, 2);
            strcpy(buffer, "Setup");
            tftcprint(ILI9341_RED, 260, 2);
            clearrow(280, 2);
            clearrow(300, 2);
            PROGMEM_readAnything (&menu [menui], thisItem);
            strcpy(buffer, thisItem.item);
            tftcprint(ILI9341_RED, 280, 2);
          }
          break;
        case 3 :
          { //show value to change
            if (menui == MENUCOUNT - 1) { //exit menu selection
              setexit3();
            }
            else {
              PROGMEM_readAnything (&menu [menui], thisItem);
              mode = 4; // the menu is selected to change values
              // show value on screen
              if (thisItem.values == NULL) {
                itoa (menuval[menui], buffer, 10);
              }
              else {
                //read day of week from menu record *
                memcpy_P (&buffer, &thisItem.values[menuval[menui] - thisItem.start], CHARCOUNT);
              }
              tftcprint(ILI9341_RED, 300, 2);
            }
          }
          break;
        case 4 :
          { //write value and go back to menu
            PROGMEM_readAnything (&menu [menui], thisItem);
            // now write the values and go back to mode 3
            thisItem.set();
            //mode = 3;
            //fix screen, delete line @ 300
            clearrow(300, 2);
            //tftcprint(ILI9341_RED, 300, 2);
          }
          break;
      }
    }
    //Serial.print(F("Button menu "));
    //Serial.println(heldDown ? F("Held") : F("Pressed"));
    //Serial.println(mode);
  }
}


// pin values
//const int minusbutton = 2;
//const int plusbutton = 4;
void onminusbutton(uint8_t pin, bool heldDown) {
  MenuRecord thisItem;
  PROGMEM_readAnything (&menu [menui], thisItem);

  switch (mode) {
    case 3 :
      {
        if (pin == minusbutton) {
          menui--;
          if (menui == 255) menui = MENUCOUNT - 1;
        }
        if (pin == plusbutton) {
          menui++;
          if (menui == MENUCOUNT) menui = 0;
        }

        clearrow(280, 2);
        PROGMEM_readAnything (&menu [menui], thisItem);
        strcpy(buffer, thisItem.item);
        tftcprint(ILI9341_RED, 280, 2);
      }
      break;
    case 4 :
      {
        if (pin == minusbutton) {
          if (menuval[menui] == thisItem.start)
          {
            menuval[menui] = thisItem.count;
          }
          else
          {
            menuval[menui]--;
          }
        }

        if (pin == plusbutton) {
          if (menuval[menui] == thisItem.count)
          {
            menuval[menui] = thisItem.start;
          }
          else
          {
            menuval[menui]++;
          }
        }

        clearrow(300, 2);
        if (thisItem.values == NULL) {
          itoa (menuval[menui], buffer, 10);
        }
        else {
          //read day of week from menu record *
          memcpy_P (&buffer, &thisItem.values[menuval[menui] - thisItem.start], CHARCOUNT);
        }
        tftcprint(ILI9341_RED, 300, 2);
      }
      break;
  }

  //Serial.println(pin);
  //Serial.println(menui);
  //Serial.print(F("Button minus "));
  //Serial.println(heldDown ? F("Held") : F("Pressed"));
}




void setup ()
{
  Wire.begin();
  rtc.begin();

  //DS18B20 tempp sensor
  sensor.begin();
  sensor.requestTemperatures();

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);

  //lcd backlight
  pinMode(PINTFTLED, OUTPUT);
  

  //speaker
  pinMode(PINSPK, OUTPUT);
  tone(PINSPK, 1000, 100);

#ifdef debug_s
  Serial.begin (115200);
  Serial.println (F(">"));
#endif
  // read settings from EEPROM
  anEeprom.readIntoMemArray((const unsigned char*)menuval + 6, 0 + 6, sizeof menuval - 6); //skip rtc values 0-5

  // fix the values we read if the eeprom is empty
  for (int i = 0; i < sizeof menuval; i++) {
    //Serial.println (menuval[i]);
    if (menuval[i] == 255) menuval[i] = 0; // if we did read 255 from eeprom zero the parameter.
  }
  // fix Month and date if eeprom was empty, first start
  if (menuval[2] == 0) menuval[2] = 1; // Month Jan
  if (menuval[3] == 0) menuval[3] = 1; // Date 1

  // LCD Backlight
  if (menuval[15] < 10) menuval[15] = 10;  //otherwise we cannot see the display
  if (menuval[16] < 10) menuval[16] = 10;  
  analogWrite(PINTFTLED, menuval[15]);  // we start the clock durring day
  
  //setRTCTime();
  
  // read RTC values from clock chip
  now = rtc.now();
  menuval[0] = now.hour();
  menuval[1] = now.minute();
  menuval[2] = now.month();
  menuval[3] = now.date();
  menuval[4] = now.dayOfWeek();
  menuval[5] = uint8_t(now.year() - 2000);

  // hours, AM/PM correction
  hours1 = menuval[0];
  if (hours1 > 12) hours1 = hours1 - menuval[6] * 12;
  if (hours1 < 1) hours1 = hours1 + menuval[6] * 12;

  mode = menuval[7];  //set mode 0 = digi, 1 = bcd

  /*
    for (int i = 0; i < 12; i++)
    {
      // make a copy of the current one
      memcpy_P (&buffer, &months[i], CHARCOUNT);
      Serial.println (buffer);
    } // end of for loop

    memcpy_P (&buffer, &months[2], CHARCOUNT);
    for (int i = 0; i < 32; i++)
    {
      byte x = buffer[i];
      Serial.print(x);
      Serial.print(F(" "));
    }

    // string concat with int example
    int aaa = 30;
    char ast[10];
    itoa (aaa, ast, 10);
    strcat(buffer, " ");
    strcat(buffer, ast);
    Serial.println(F(""));
    Serial.println (buffer);
    Serial.println (strlen(buffer));

    // Menu array in PROGMEM
    MenuRecord thisItem;
    PROGMEM_readAnything (&menu [4], thisItem);
    Serial.println (thisItem.item);
    Serial.println (thisItem.count);
    //thisItem.set();
    //read day of week from menu record *
    memcpy_P (&buffer, &thisItem.values[4], CHARCOUNT);
    Serial.println (buffer);
  */

  //iobuttons
  // First we set up the switches library, giving it the task manager and tell it to use arduino pins
  // We could also of chosen IO through an i2c device that supports interrupts.
  // If you want to use PULL DOWN instead of PULL UP logic, change the true to false below.
  // true = button connected to ground
  switches.initialise(ioUsingArduino(), true);

  // now we add the switches, we dont want the spinwheel button to repeat, so leave off the last parameter
  // which is the repeat interval (millis / 20 basically) Repeat button does repeat as we can see.
  //switches.addSwitch(spinwheelClickPin, onSpinwheelClicked);
  //switches.addSwitch(repeatButtonPin, onRepeatButtonClicked, 25);

  switches.addSwitch(menubutton, onmenubutton);
  switches.addSwitch(minusbutton, onminusbutton, 25);
  switches.addSwitch(plusbutton, onminusbutton, 25);


  // run the clock display
  taskManager.scheduleFixedRate(990, myclock);  //once per second+sync

}

// end of setup

void loop () {
  taskManager.runLoop();
}
