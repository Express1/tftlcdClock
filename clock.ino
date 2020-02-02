

/*
  // Color definitions
  #define ILI9341_BLACK       0x0000  ///<   0,   0,   0
  #define ILI9341_NAVY        0x000F  ///<   0,   0, 123
  #define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
  #define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
  #define ILI9341_MAROON      0x7800  ///< 123,   0,   0
  #define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
  #define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
  #define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
  #define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
  #define ILI9341_BLUE        0x001F  ///<   0,   0, 255
  #define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
  #define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
  #define ILI9341_RED         0xF800  ///< 255,   0,   0
  #define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
  #define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
  #define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
  #define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
  #define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
  #define ILI9341_PINK        0xFC18  ///< 255, 130, 198
*/

/*
  void setRTCTime()
  {
  DateTime dt(2019, 11, 11, 17, 21, 00, 0); // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
  }
*/

void tftcprint(uint16_t color, int y, int textSize)
// print buffer[] center justified on tft
{
  int x;
  //chard are 5X7 with one px space, 6 px width.
  x = (240 - strlen(buffer) * 6 * textSize) / 2;
  //if (x < 0) {
  //  x = 0;
  //};
  /*
    Serial.print (F("center x:"));
    Serial.print (x);
    Serial.print (F(" strlen:"));
    Serial.println (strlen(buffer));
      for (int i = 0; i < 16; i++)
    {
      byte x = buffer[i];
      Serial.print((buffer[i]));
      Serial.print(x);
      Serial.print(F(" "));

    }
    Serial.println("");
  */
  tftprint(color, x, y, textSize);
}


void tftprint(uint16_t color, int x, int y, int textSize)
// print buffer[] on tft, will delete previous text
// https://github.com/adafruit/Adafruit-GFX-Library/issues/16 , have print clear existing text
//To prevent having to clear the text before its drawn,
//like AKA mentions try using the bg color by calling setTextColor(foreground color, background color)
{
  //Serial.println (buffer);
  tft.setCursor(x, y);
  tft.setTextColor(color, ILI9341_BLACK);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(buffer);

}


void ptemp()
{

  // RTC temp sensor
  rtc.convertTemperature();
  float temperaturec = rtc.getTemperature();
  Serial.print(sensor.getTempC());
  Serial.print(F(" "));
  //

  // DS18B20 temp sensor
  //float
  temperaturec = sensor.getTempC() - 0.5;
  //Serial.println(sensor.getTempC());
  sensor.requestTemperatures();

  //

  float temperaturef;
  temperaturef = (temperaturec * 1.8) + 32;

  dtostrf(temperaturef, 5, 1, buffer);
  strcat(buffer, " F");
  tftcprint(ILI9341_YELLOW, 200, 3);
  strcpy(buffer, "o");
  tftprint(ILI9341_YELLOW, 149, 194, 2);

  dtostrf(temperaturec, 5, 1, buffer);
  strcat(buffer, " C");
  tftcprint(ILI9341_YELLOW, 200 + 8 * 4, 3);
  strcpy(buffer, "o");
  tftprint(ILI9341_YELLOW, 149, 194 + 8 * 4, 2);

#ifdef debug_s
  Serial.print(temperaturec); //read registers and display the temperature
  Serial.print(F(" C / "));
  Serial.print(temperaturef); //read registers and display the temperature
  Serial.println(F(" F"));
#endif
}

// BCD display parameters
#define S 3 //char size 20/2 or 30/3
#define P 30 //spacing

#define C ILI9341_GREEN //color on
#define O ILI9341_MAROON //color off

#define X 10 //X offset
#define Y 5 //Y offset

void bcdp(uint8_t val, uint8_t x )
{
  for (int i = 0; i < 4; i++) {
    if (val & (0b1000 >> i)) {
      strcpy(buffer, "\""); tftprint(C, x * P + X, P * i + Y, S);
    }
    else {
      strcpy(buffer, "\""); tftprint(O, x * P + X, P * i + Y, S);
    }
  }
}


void pyear()
//DayWeekNumber: Calculate day of year and week of year
// this routine does come from http://arduino.joergeli.de
/*
  menuval[0] hour 0-23
  menuval[1] min 0 -59
  menuval[2] month 1-12
  menuval[3] dayofmonth 1-31
  menuval[4] dayofweek 0-6, 0=Monday
  menuval[5] year last 2 digits 19XX
*/
{
  int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  int DN, WN;
  // Number of days at the beginning of the month in a not leap year.
  //Start to calculate the number of day
  if (menuval[2] == 1 || menuval[2] == 2) {
    DN = days[(menuval[2] - 1)] + menuval[3]; //for any type of year, it calculate the number of days for January or february
  }                                                          // Now, try to calculate for the other months
  else if (((menuval[5] + 2000) % 4 == 0 && (menuval[5] + 2000) % 100 != 0) ||  (menuval[5] + 2000) % 400 == 0) { //those are the conditions to have a leap year
    DN = days[(menuval[2] - 1)] + menuval[3] + 1;                              // if leap year, calculate in the same way but increasing one day
  }
  else {                                                     //if not a leap year, calculate in the normal way, such as January or Febr
    DN = days[(menuval[2] - 1)] + menuval[3];
  }
  // Now start to calculate Week number
  if (menuval[4] == 0) {
    WN = (DN - 7 + 10) / 7;                                  //if it is sunday (time library returns 0)
  }
  else {
    WN = (DN - menuval[4] + 10) / 7;                                  // for the other days of week
  }

  itoa ((menuval[5] + 2000), buffer, 10);
  memcpy_P (&ast, wday[0], 7);
  strcat(buffer, ast);
  itoa (WN, ast, 10);
  strcat(buffer, ast);
  tftcprint(ILI9341_GREEN, 260, 2);
  memcpy_P (&buffer, wday[1], 7);
  //strcpy(buffer, " Day ");
  itoa (DN, ast, 10);
  strcat(buffer, ast);
  tftcprint(ILI9341_GREEN, 280, 2);
}


void doalarm()
{
  // if (alarmon == 0) {
  alarmon = 1;
  alarmcount = 60;
  //memcpy_P (&buffer, &alm, 6);
  //tftcprint(ILI9341_RED, 260, 4);
  doalarm1();
  // }
#ifdef debug_s
  Serial.println (F("Alarm"));
#endif

}

void doalarm1()
{
  if (alarmon == 1 && alarmcount > 0) {
    taskManager.scheduleOnce(1000, doalarm1);
    tone(PINSPK, 1000, 600);

    memcpy_P (&buffer, &alm, 6);
    if (alarmcount % 2 == 0) {
      tftcprint(ILI9341_RED, 260, 5);
    }
    else {
      tftcprint(ILI9341_BLACK, 260, 5);
    }

    alarmcount--;
#ifdef debug_s
    //Serial.println(alarmcount);
    //Serial.println(millis());
#endif
  }
  else {
    alarmon = 0;
    memcpy_P (&buffer, &alm, 6);
    tftcprint(ILI9341_BLACK, 260, 5);
    pyear();
    //setexit3();
  }
}


void pdate()
{
  //cleanup
  clearrow(150, 2);
  //get day of week string
  memcpy_P (&buffer, &dayofweek[menuval[4] - 1], CHARCOUNT);
  tftcprint(ILI9341_GREEN, 150, 2);

#ifdef debug_s
  Serial.print(buffer);
  Serial.print(F(" "));
#endif
  //cleanup
  clearrow(170, 2);
  //get month string
  //tftcprint(ILI9341_GREEN, 170, 2);
  memcpy_P (&buffer, &months[menuval[2] - 1], CHARCOUNT);
  itoa (menuval[3], ast, 10);
  strcat(buffer, " ");
  strcat(buffer, ast);
  tftcprint(ILI9341_GREEN, 170, 2);

#ifdef debug_s
  Serial.println(buffer);
#endif
}


void clockbcd()
{
  bcdp(seconds / 10, 6);
  bcdp(seconds % 10, 7);

  // hours
  bcdp(hours1 / 10, 0);
  bcdp(hours1 % 10, 1);

  // min
  bcdp(menuval[1] / 10, 3);
  bcdp(menuval[1] % 10, 4);

#ifdef debug_s
  Serial.print(hours1);
  Serial.print(F("%"));
  Serial.print(menuval[1]);
  Serial.print(F(" "));
#endif
}

void clockdigi()
{


  itoa (hours1, ast, 10);
  if (hours1 < 10) {
    //itoa (menuval[0], ast, 10);
    //if (menuval[0] < 10) {
    strcpy(buffer, "0");
    strcat(buffer, ast);
  }
  else {
    strcpy(buffer, ast);
  }
  tftcprint(ILI9341_WHITE, 10, 8);

#ifdef debug_s
  Serial.print(buffer);
  Serial.print(F(":"));
#endif

  // 12 hours menuval[6] 0=24h 1=12h
  if (menuval[6] == 1) {
    //strcpy(buffer, ampm);
    if (menuval[0] > 11) {
      memcpy_P (&buffer, &apm[1], 3); //PM
    }
    else {
      memcpy_P (&buffer, &apm[0], 3); //AM
    }
    tftprint(ILI9341_RED, 180, 115, 3);
  }

  // minutes
  itoa (menuval[1], ast, 10);
  if (menuval[1] < 10) {
    strcpy(buffer, "0");
    strcat(buffer, ast);
  }
  else {
    strcpy(buffer, ast);
  }
  tftcprint(ILI9341_WHITE, 80, 8);

#ifdef debug_s
  Serial.print(buffer);
  Serial.print(F(" "));
#endif

}


void myclock()
{
  now = rtc.now();
  while (seconds == now.second())  //we wait for the next second
  {
    taskManager.yieldForMicros(1000);
    now = rtc.now(); //get the current date-time
  }
  seconds = now.second();

  // update bcdclock once per sec
  if (mode == 1) clockbcd();

#ifdef debug_s
  if (mode == 0) { // only in clockdigi
    itoa (seconds, ast, 10);
    if (seconds < 10) {
      strcpy(buffer, "0");
      strcat(buffer, ast);
    }
    else {
      strcpy(buffer, ast);
    }
    tftcprint(ILI9341_RED, 300, 2);
  }
#endif

  // update once per min
  if (minutes != now.minute()) {
    menuval[0] = now.hour();
    menuval[1] = now.minute();
    menuval[2] = now.month();
    menuval[3] = now.date();
    menuval[4] = now.dayOfWeek();
    menuval[5] = uint8_t(now.year() - 2000);
    //
    minutes = now.minute();

    // update clockdigi
    hours1 = menuval[0];  //AM/PM hour correction
    if (hours1 > 12) hours1 = hours1 - menuval[6] * 12;
    if (hours1 < 1) hours1 = hours1 + menuval[6] * 12;

    if (mode == 0) clockdigi();

    ptemp(); // show temp once per min

    // check for alarm
    //menuval[8] alarm hour
    //menuval[9] alarm min
    //menuval[10] alarm days 0=off, 1=on, 2=workdays
    /*Su=1 Mo=2 Tu=3 We=4 Th=5 Fr=6 Sa=7 */
    if (menuval[8] == menuval[0] && menuval[9] == menuval[1] && menuval[10] > 0) {
      // alarm on, check day
      if (menuval[10] == 1) { //each day
        doalarm();
      }
      else if (menuval[10] == 2 && menuval[4] > 1 && menuval[4] < 7) { //weekday menuval[4]
        doalarm();
      }
    }

    // once per hour
    if (hours != now.hour()) {
      hours = now.hour();
      pdate();
      pyear();

      // set LCD backlight for day/night
      if (menuval[0] == STARTNIGHT)
        analogWrite(PINTFTLED, menuval[16]);

      if (menuval[0] == STARTDAY)
        analogWrite(PINTFTLED, menuval[15]);


      // check for DST
      //DST on
      if (menuval[12] == menuval[2]) //month
      {
        if (((menuval[3] + 1) / 7 + 1) == menuval[11] && menuval[0] == 2 && menuval[4] == 1) // week numer, 2AM, Sunday=day 1
        {
          DateTime now = rtc.now(); //DST on + 1 hour
          DateTime dt(now.year(), now.month(), now.date(), now.hour() + 1, now.minute(), now.second(), now.dayOfWeek());
          // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
          rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
        }
      }

      //DST off
      if (menuval[14] == menuval[2]) //month
      {
        if (((menuval[3] + 1) / 7 + 1) == menuval[13] && menuval[0] == 2 && menuval[4] == 1) // week numer, 2AM, sunday
        {
          DateTime now = rtc.now(); //DST off - 1 hour
          DateTime dt(now.year(), now.month(), now.date(), now.hour() - 1, now.minute(), now.second(), now.dayOfWeek());
          // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
          rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
        }
      }

    }
  }
}



