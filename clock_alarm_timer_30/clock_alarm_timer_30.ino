/*
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 
International License. To view a copy of this license, visit 
http://creativecommons.org/licenses/by-nc-sa/4.0/ 
or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
*/

/*
 * This sketch will have intro and comments cleaned up. 
 * TODO- remove this comment when done.
 */



/*
To do:

9. scattered TODOs   
11. Clean up Intro.  Link to future youtube videos. Link to reddit.  Link to github.  Link
    to future Instructable.
*/

/*
 * Make sure your LCD keypad shield does not have faulty circuitry! Many such shields
 * contain circuitry that will draw way too much current through the backlight.  
 * Here is a link to an official Arduino forum discussion on diagnosing 
 * and fixing this problem:
 *  
 *  http://forum.arduino.cc/index.php?topic=96747.0
 *  
 * I opted to go with a hardware fix.  I cut the through the copper trace on the 
 * shield that connects the backlight to the pin that goes to Arduino pin 10 and 
 * put in a diode.
 * 
 * If you have one of these faulty keypad shields, you will have to do a hardware fix
 * if you want to use this program.  I did not employ the software workaround
 * anywhere in my program.
 *  
 * Connect the positive lead of an 8ohm 0.25W speaker to a 100ohm resistor.  
 * Then, connect that to Arduino's pin 53.  Connect the negative lead to ground.
 * To test the polarity of the speaker: Briefly connect the speaker and resistor to 
 * power.  If the speaker pushes OUT, its polarity is correct. The lead going to positive 
 * is the positive lead.  The lead going to negative is the negative lead.
 * 
 * Connect an LED to pin 52.  I use a green 5mm LED and a 330 ohm resistor.
 * 
 * Connect a DS1307 RTC.  Ground to ground, 5v to 5v, SDA to SDA (Arduino pin 20 
 * on my Mega 2560 board), SCL to SCL (Arduino pin 21 on my Mega 2560 board).
 * 
 * Required libraries:
 * Elapsed millis library can be downloaded here:
 *  http://playground.arduino.cc/Code/ElapsedMillis
 * RTC library can be downloaded here:
 *  https://github.com/adafruit/RTClib
 */

#include <elapsedMillis.h>

elapsedMillis timeElapsed;  // used by the timer
elapsedMillis timerAlarmElapsed;  // keeps track of how long the timer alarm has been going off
elapsedMillis alarm1Elapsed;  // keeps track of how long the alarm1 alarm has been going off
elapsedMillis setModeElapsed;  //  keeps track of how long the screen has been on set mode.
elapsedMillis upElapsed; // keeps track of how long the up button is held down
elapsedMillis selectElapsed; // keeps track of how long the select button is held down

#include <LiquidCrystal.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "pitches.h"

RTC_DS1307 rtc;

#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_BACKLIGHT 10
#define ledPin 52

int pinState = LOW;   // used to blink a pin without delay
int lcd_key     = 0;
int adc_key_in  = 0;
int screen      = 0;  // controls which screen-mode (clock, alarm, timer, interval timer) is displayed
int cursorPosition = 1;   // defines the position of the blink cursor in "set mode"
int alarm1Array[] = {0,0}; // this array will hold the alarm1 time
int clockArray[7];  // this array will pass/get passed the real time
int timerArray[] = {0,0,2}; // this array will hold the timer
long selectPressTime = 0;
long upPressTime = 0;
long cumulativeOn = 0;  // keeps track of how long the timer has been running down
long lastCumulative = 0;  // used to keep track of how long the timer has been running down in siutations where the timer is paused/restarted
long totalTime = 2000; // holds the amount of time put on the timer
long timeLeft = 0;     // timer countdown- when it hits 0, the timer alarm goes off
boolean backlightState = true; // start out with the backlight on
boolean setMode = false;
boolean anAlarmIsGoingOff = false;
boolean alarm1State = false;  //alarm1 is off by default
boolean alarm1Acknowledged = false;
boolean timerIsRunning = false;
boolean recurringActionBlocker = false;  // used to stop button presses from triggering the same actions more than once.
boolean selectHeldDown = false; // used to stop "SELECT" button dependent variables from changing while the "SELECT" button is being held down
boolean upHeldDown = false; // used to stop "UP" button dependent variables from changing as the "UP" button is being held down.
boolean downHeldDown = false;
boolean leftHeldDown = false;
boolean rightHeldDown = false;
const char* dayName[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
const char* monthName[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Custom arrow symbol
byte play[8] = {
  B10000,
  B11000,
  B11100,
  B11110,
  B11100,
  B11000,
  B10000,
  B00000
};
// Custom pause symbol
byte pause[8] = {
  B00000,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B00000
};

// read the buttons
int read_LCD_buttons(){
  // read the value from the sensor
  adc_key_in = analogRead(0);
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // add approx 50 to those values and check to see if values are close
  if (adc_key_in > 1000) return btnNONE; // make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 195)  return btnUP; 
  if (adc_key_in < 380)  return btnDOWN; 
  if (adc_key_in < 555)  return btnLEFT; 
  if (adc_key_in < 790)  return btnSELECT;  
  return btnNONE;  // when all others fail, return this...
}

// ########################################  Setup  ########################################

void setup() {
  Serial.begin(57600);
  // TODO: what is #ifdef AVR
  #ifdef AVR
  Wire.begin();
  /* TODO: what is this #else? I think this is from the code I pulled from
   *  DFROBOT (where I got my lcd shield)
   */
  #else
    Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  #endif
    rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  pinMode(0, INPUT);
  pinMode(ledPin, OUTPUT);            // the onboard LED is set as an output
  digitalWrite(ledPin, LOW);         // onboard LED is on at start
  pinMode(LCD_BACKLIGHT, OUTPUT);    //LCD Backlight is controlled by pin 10 and is an OUTPUT
  digitalWrite(LCD_BACKLIGHT, backlightState); // Turn the backlight on
  lcd.createChar(9, play);
  lcd.createChar(8, pause);
  lcd.begin(16, 2);              // start the library
}

// ########################################  Main Loop  ########################################

void loop() {
  // currentMillis is equal to the time in milliseconds arduino has been running 
  unsigned long currentMillis = millis();
  lcd_key = read_LCD_buttons();  // read the buttons
  //Update the real time
  DateTime now = rtc.now();
  /* As long as the clock is not being actively set,
     enter the real time into an array.
  */
  if ((screen != 0) || (screen == 0 && setMode != true)){
    clockArray[0] = now.year();   
    clockArray[1] = now.month();
    clockArray[2] = now.day();
    clockArray[3] = now.hour();
    clockArray[4] = now.minute();
    clockArray[5] = now.second();
    clockArray[6] = now.dayOfWeek();
  }

  /*
   * ###############################
   * ###############################
   * ###############################
   * Actions tied to pressing buttons
   * ###############################
   * ###############################
   * ###############################
   */

  // Trigger defferent events depending on which buttons are pressed
  switch (lcd_key){
    /*
     * Pressing and releasing the SELECT button advances through the various screens
     * pressing and holding the SELECT button enters the "set mode" for each screen.
     * pressing and releasing the SELECT button, or pressing and holding the SELECT button,
     * while in "set mode," exits "set mode."
     */
    case btnSELECT:
      /*
       * "SELECT" button dependent actions in the main loop are blocked 
       * until the "SELECT" button is released.  
       * (btnNONE changes this variable back)
       */
      selectHeldDown = true; 
      /*
       * The only other button with press and hold functionality
       * is the up button.  Keep the time dependent variable that 
       * enables this functionality at 0.
       */
      upElapsed = 0;
      /*
       * If a press-and-hold event has just occurred, and the user has
       * not yet released the button, don't log the button press time.
       * This is to prevent the same action from occurring multiple times
       * if the user keeps the button held down.
       * 
       * Also, if an alarm is going off, don't log the button press time.
       * When an alarm is going off, pressing the SELECT button (or any button
       * for that matter) will simply turn the alarm off, and do nothing else.
       */
      if (recurringActionBlocker == true || anAlarmIsGoingOff == true){
        selectElapsed = 0;
      }
      /*
       * Otherwise, do log the elapsed time that the SELECT button
       * accrues while it is held down.  A second variable is needed
       * (selectPressTime) as selectElapsed returns to 0 as soon as the 
       * SELECT button is released (release of the SELECT button is a 
       * conditional for pretty much all of the functions
       * with actions triggered by the SELECT button.
       */
      else {
        selectPressTime = selectElapsed;
      }
      // If the select button has been held down for longer than 1.5s,
      if (selectElapsed > 1500){
        /*
         * Prevent multiple press-and-hold events from occurring
         * if the user keeps the button held down.
         */
        recurringActionBlocker = true;
        // enter, or exit, set-mode, 
        setMode = !setMode;
        // clear the screen,
        lcd.clear();
        // return the cursor to the home position,
        cursorPosition = 1;
        // prevent any simple-press events from occurring.
        selectPressTime = 0; 
      }
      delay(10); // TODO: debounce necessary?
      break;
      
    case btnUP:
      /*
       * "UP" button dependent actions are blocked until the "UP"
       * button is released.  btnNONE resets this.
       */
      upHeldDown = true;       
      /*
       * The only other button with press and hold functionality
       * is the "SELECT" button.  Keep the time dependent variable that 
       * enables its press-and-hold functionality at 0.
       */
      selectElapsed = 0;
      // if the user presses any buttons, stay in setMode for additional time
      setModeElapsed = 0;
      /*
       * If a press-and-hold event has just occurred, and the user has
       * not yet released the button, don't log the button press time.
       * This is to prevent the same action from occurring multiple times
       * if the user keeps the button held down.
       * 
       * Also, if an alarm is going off, don't log the button press time.
       * When an alarm is going off, pressing the UP button (or any button
       * for that matter) will simply turn the alarm off, and do nothing else.
       */
      if (recurringActionBlocker == true || anAlarmIsGoingOff == true){
        upElapsed = 0;
      }
      /*
       * Otherwise, do log the elapsed time that the UP button
       * accrues while it is held down.  A second variable is needed
       * (upPressTime) as upElapsed returns to 0 as soon as the 
       * UP button is released (release of the UP button is a 
       * conditional for pretty much all of the functions
       * with actions triggered by the UP button.
       */
      else {
        upPressTime = upElapsed;
      } 
      // If the up button has been held down for longer than 1.5s,
      if (upElapsed > 1500){
        /*
         * Prevent multiple press-and-hold events from occurring
         * if the user keeps the button held down.
         */
        recurringActionBlocker = true;
        // turn the backlight on or off, 
        backlightState =! backlightState;
        digitalWrite(LCD_BACKLIGHT, backlightState);
        // prevent any simple-press events from occurring.
        upPressTime = 0; 
      }
      delay(10);
      break;
 
    case btnDOWN:
      // Keep the other buttons' time-dependent variables at 0.
      upElapsed = 0;
      selectElapsed = 0;
      // if the user presses any buttons, stay in setMode for additional time
      setModeElapsed = 0;
      /*
        When the alarm is going off, pressing the down button (or 
        any button for that matter) will simply turn off the alarm,
        and nothing else.) NOTE: btnDOWN is a special case here.
        Notice that when lcd_key = btnDOWN, downHeldDown is NOT
        set to true.  downHeldDown is set to true after actions outside of the main loop
        have taken place. Meaning, the down button can trigger actions without first being
        released. For example, if the screen is on the timer, 
        and the cursor is over the start/pause symbol, a down button press will IMMEDIATELY
        start or pause the timer. Only then is downHeldDown set to true. This presents
        a sticky situation when an alarm is going off. Say, for whatever reason, 
        the checkTheTimer() function is placed in the main loop after screen selection.
        The screen is on the timer, and the cursor is over the start/pause symbol. An
        alarm is going off.  The user presses the down button.  This starts the timer
        and creates all sorts of bugginess.  The alarm never got acknowledged.
          So, to keep it simple, when an alarm is going off, and the user presses the down 
        button to acknowledge the alarm, lcd_key is immediately set as btnUP.  ALL 
        actions requiring an up button press also require the up button to first be released
        as well. 
      */
      if (anAlarmIsGoingOff == true){
        lcd_key = btnUP; 
      }
      break;

    case btnLEFT:
    
      // Keep the other buttons' time-dependent variables at 0.
      upElapsed = 0;
      selectElapsed = 0;
      // if the user presses any buttons, stay in setMode for additional time
      setModeElapsed = 0;
      if (anAlarmIsGoingOff == true){
        lcd_key = btnUP; 
      }
      delay(10);  // debounce?
    break;
    
    case btnRIGHT:

      // Keep the other buttons' time-dependent variables at 0.
      upElapsed = 0;
      selectElapsed = 0;
      // if the user presses any buttons, stay in setMode for additional time
      setModeElapsed = 0;
      if (anAlarmIsGoingOff == true){
        lcd_key = btnUP; 
      }
      delay(10);  // debounce?
    break; 
    
    case btnNONE:
      selectElapsed = 0;
      upElapsed = 0;
      selectHeldDown = false;
      upHeldDown = false;
      downHeldDown = false;
      leftHeldDown = false;
      rightHeldDown = false;
      recurringActionBlocker = false; 
    break; 
  } // end switch

  // constantly check to see if the timer has run out.
  checkTheTimer(currentMillis);
  
  /*
   * If the alarm is on, and the program is not in set mode,
   * check the alarm.  The alarm will not go off in set mode.
   * I don't want to unintentionally set off alarms and their
   * accompanying actions while I am setting the alarm or the clock
   * (which the alarm references).
   */
  if (alarm1State == true && setMode == false) { 
    checkAlarm1(currentMillis);
  }
  /*
   * Once the alarm matches the clock, alarm1Elapsed keeps track of 
   * how long the alarm has been going off.
   */
  else {
    alarm1Elapsed = 0;
  }
 
     
// ########################################  Advancing through screens using the select button   ########################################
    
  // if the select button has been pressed for less than 1.5s,
  if (selectPressTime > 0
  // and released,
  && selectHeldDown == false
  // and we are not in set-mode
  && setMode == false){
    // set selectPressTime back to 0 in anticipaiton of another button press,
    selectPressTime = 0; 
    // clear the screen,
    lcd.clear();
    // advance the screen,
    screen ++;
    // and return the cursor to the home position.
    cursorPosition = 1;
  }
  // if the select button has been pressed for less than 1.5s,   
  else if (selectPressTime > 0
  // and released,
  && selectHeldDown == false
  // and we ARE in set-mode,
  && setMode == true){
    // set select PressTime back to 0 in anticipation of another button press,
    selectPressTime = 0;
    // clear the screen,
    lcd.clear();
    // return the cursor to the home position,
    cursorPosition = 1;
    // and exit set mode.
    setMode = false;
  }

  /*
   * Determine which screen to display on the LCD
   */
  if (screen == 0 && setMode == false){  
    clock();        
  }
  else if (screen == 0 && setMode == true){
    setClock(currentMillis);
  }
  else if (screen == 1 && setMode == false){
    alarm1();
  }
  else if (screen == 1 && setMode == true){
    setAlarm1(currentMillis);
  }
  else if (screen == 2 && setMode == false){
    timer(currentMillis);
  }
  else if (screen == 2 && setMode == true){
    setTimer(currentMillis);
  }
  else if (screen >= 3){
    screen = 0;           // return to first screen
  }

  // if the program is not in setMode, continually set the following variable to 0
  if (setMode == false){
    setModeElapsed = 0;
  }
  /*
   * If the program stays in set mode for more than 15s, exit set mode.
   * This is to ensure that no alarms are missed in the event that the user walks
   * away from the device before exiting set mode.
   */
  if (setModeElapsed >= 15000){ 
    // The characters need to be cleared if setmode is allowed to lapse back into regular mode.
    lcd.clear();
    setMode = false;
    cursorPosition = 1;
  }
  
}  // End of Main Loop

