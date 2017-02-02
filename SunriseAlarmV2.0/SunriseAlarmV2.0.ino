//Sunrise Alarm Clock
//Arturo Abruzzini 2017

//include libraries
#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <LiquidCrystal.h>

//initiate vars
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); 

bool alarm_on=false;
bool backlight_on=true;
bool lights_on=false;

int pin_backlight=1;
int pin_btn1=8; //mode
int pin_btn2=9; //alarm on/off, lights on/off, increment
int pin_btn3=10; //backlight on/off, select value to edit
int pin_lights=11;

time_t t;
#define TIME_MSG_LEN 11 // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER 'T' // Header tag for serial time sync message
int now_hour, now_min, now_sec, now_day, now_month, now_year;
String now_day_str, now_month_str;
int alarm_hour=7;
int alarm_min=0;

int mode=1;

int alarm_lenght=30; //min
float R; //exponential rate for light turn on

void setup() {
  // setup libraries, rtc, lcd, pinmodes
  Wire.begin();
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");
  lcd.begin(16,2);
  pinMode(pin_btn1, INPUT);
  pinMode(pin_btn2, INPUT);
  pinMode(pin_btn3, INPUT);
  pinMode(pin_lights, OUTPUT);
  pinMode(pin_backlight, OUTPUT);
  digitalWrite(pin_btn1, HIGH);
  digitalWrite(pin_btn2, HIGH);
  digitalWrite(pin_btn3, HIGH);
  digitalWrite(pin_backlight, HIGH);
 
  //get time from computer if connected
  
  
  //set exponential rate of lights turning on
  R = (alarm_lenght * log10(2))/(log10(255));
}

void loop() {
  //display time
  plot_time();

  //check is it's time for the alarm, and set the lights if it is
  check_alarm();

  //set backlight
  if (backlight_on) {
    digitalWrite(pin_backlight, LOW);
  } else {
    digitalWrite(pin_backlight, HIGH);
  }

  //set lights
  if (lights_on) {
    analogWrite(pin_lights, 255);
  } else {
    analogWrite(pin_lights, 0);
  }
  
  // check mode var and go to mode
  switch (mode) {
    case 1:
      base_mode();
      break;
    case 2:
      edit_alarm_hour_mode();
      break;
    case 3:
      edit_alarm_min_mode();
      break;
    case 4:
      edit_time_hour_mode();
      break;
    case 5:
      edit_time_min_mode();
      break;
    case 6:
      edit_time_sec_mode();
      break;
  }

  delay(200);
}

void plot_time() {
  // get time and booleans and display time date and alarm time if on
  now_hour = hour();
  now_min = minute();
  now_sec = second();
  now_day = day();
  now_month = month();
  now_day_str = dayShortStr(weekday());                     //check if it works
  now_month_str = monthShortStr(now_month);
  now_year = year();

  //display "HH:MM:SS" top left
  lcd.setCursor(0,0);
  if (now_hour<=9) lcd.print("0");
  lcd.print(now_hour);
  lcd.print(":");
  if (now_min<=9) lcd.print("0");
  lcd.print(now_min);
  lcd.print(":");
  if (now_sec<=9) lcd.print("0");
  lcd.print(now_sec);

  //display "DAY DD MON" bottom centre
  lcd.setCursor(3,1);
  lcd.print(now_day_str);                           //check if it works
  lcd.setCursor(7,1);
  lcd.print(now_day);
  lcd.setCursor(10,1);
  lcd.print(now_month_str);

  //display "A HH:MM" top right if alarm is on
  if (alarm_on==true) {
    lcd.setCursor(9,0);
    lcd.print("A");
    if (alarm_hour<=9) lcd.print("0");
    lcd.print(alarm_hour);
    lcd.print(":");
    if (alarm_min<=9) lcd.print("0");
    lcd.print(alarm_min);
  } else {
    lcd.setCursor(9,0);
    lcd.print("      ");                            //check if it works
  }
}

void check_alarm() {
  // check if time matches alarm time
  int now_time_in_minutes = now_min+now_hour*60;
  int alarm_time_in_minutes = alarm_min+alarm_hour*60;
  int min_since_alarm = now_time_in_minutes-alarm_time_in_minutes;

  //if it's still alarm time, set the light to correct intensity
  //ramping up from 0 to full brightness over alarm_lenght minutes
  if (min_since_alarm<=alarm_lenght) {
    int brightness = pow (2, (min_since_alarm / R)) - 1;
    analogWrite(pin_lights, brightness);
    backlight_on=true;
  }

}

//mode 1
void base_mode() {
  // base mode

  lcd.noBlink();
  // if btn_1 is pressed, go to edit_alarm_hour_mode-2
  if (digitalRead(pin_btn1)==0) {
    mode = 2;
  }
  // if btn_2 is pressed, if backlight is on, switch alarm on/off
  if (digitalRead(pin_btn2)==0) {
    if (backlight_on==true) {
      alarm_on =! alarm_on;
    }
    // if btn_2 is pressed, if backlight is off, switch lights on/off
    else {
      lights_on =! lights_on;
    }
  }

  // if btn_3 is pressed, switch backlight on/off
  if (digitalRead(pin_btn3)==0) {
    backlight_on =!backlight_on;
  }
}

//mode 2
void edit_alarm_hour_mode() {
  // edit alarm hour mode
  lcd.setCursor(11,0);
  lcd.blink();
  // if btn_1 is pressed, go to edit_time_hour_mode-4
  if (digitalRead(pin_btn1)==0) {
    mode = 4;
  }
  // if btn_2 is pressed, add 1 to alarm_hour
  if (digitalRead(pin_btn2)==0) {
    alarm_hour += 1;
    if (alarm_hour>=24) alarm_hour -= 24;
  }
  // if btn_3 is pressed, go to edit_alarm_min_mode-3
  if (digitalRead(pin_btn3)==0) {
    mode = 3;
  }
}

//mode 3
void edit_alarm_min_mode() {
  // edit alarm min mode
  lcd.setCursor(14,0);
  lcd.blink();
  // if btn_1 is pressed, go to base_mode-1
  if (digitalRead(pin_btn1)==0) {
    mode = 1;
  }
  // if btn_2 is pressed, add 5 to alarm_min
  if (digitalRead(pin_btn2)==0) {
    alarm_min += 5;
    if (alarm_min>=60) alarm_min -= 60;
  }
  // if btn_3 is pressed, go to base_mode-1
  if (digitalRead(pin_btn3)==0) {
    mode = 1;
  }
}

//mode 4
void edit_time_hour_mode() {
  // edit time hour mode
  lcd.setCursor(1,0);
  lcd.blink();
  // if btn_1 is pressed, go to base_mode-1
  if (digitalRead(pin_btn1)==0) {
    mode = 1;
  }
  // if btn_2 is pressed, add 1 to time_hour
  if (digitalRead(pin_btn2)==0) {
    now_hour +=1;
    if (now_hour>=24) now_hour -= 24;
    setTime(now_hour,now_min,now_sec,now_day,now_month,now_year);
    RTC.set(now());
  }
  // if btn_3 is pressed, go to edit_time_min_mode-5
  if (digitalRead(pin_btn3)==0) {
    mode = 5;
  }
}

//mode 5
void edit_time_min_mode() {
  // edit time min mode
  lcd.setCursor(4,0);
  lcd.blink();
  // if btn_1 is pressed, go to base_mode-1
  if (digitalRead(pin_btn1)==0) {
    mode = 1;
  }
  // if btn_2 is pressed, add 1 to time_min
  if (digitalRead(pin_btn2)==0) {
    now_min +=1;
    if (now_min>=60) now_min -= 60;
    setTime(now_hour,now_min,now_sec,now_day,now_month,now_year);
    RTC.set(now());
  }
  // if btn_3 is pressed, go to edit_time_sec_mode-6
  if (digitalRead(pin_btn3)==0) {
    mode = 6;
  }
}

//mode 6
void edit_time_sec_mode() {
  // edit time sec mode
  lcd.setCursor(7,0);
  lcd.blink();
  // if btn_1 is pressed, go to base_mode-1
  if (digitalRead(pin_btn1)==0) {
    mode = 1;
  }
  // if btn_2 is pressed, set now_sec=0
  if (digitalRead(pin_btn2)==0) {
    now_sec = 0;
    setTime(now_hour,now_min,now_sec,now_day,now_month,now_year);
    RTC.set(now());
  }
  // if btn_3 is pressed, go to base_mode-1
  if (digitalRead(pin_btn3)==0) {
    mode = 1;
  }

}


