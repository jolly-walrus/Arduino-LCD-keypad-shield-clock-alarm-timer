void alarm1(){
  /*
   * Break up the main alarm variables into 
   * their individual place values. This makes for
   * easier printing to the LCD. It also opens up
   * the possibility of allowing the user to 
   * change the both the Tens and Ones values
   * for each main alarm variable.
   */
  int hourTens = alarm1Array[0] / 10;
  int hourOnes = alarm1Array[0] % 10;
  int minuteTens = alarm1Array[1] / 10;
  int minuteOnes = alarm1Array[1] % 10;
  
  lcd.setCursor(0,0);
  lcd.print("Alarm 1");
  lcd.setCursor(9,0);
  if (alarm1State == true){
     lcd.print("ON");
     }
     else {
     lcd.print("OFF");
     }
  lcd.setCursor(0,1);
  lcd.print(hourTens);
  lcd.setCursor(1,1);
  lcd.print(hourOnes);
  lcd.setCursor(2,1);
  lcd.print(":");
  lcd.setCursor(3,1);
  lcd.print(minuteTens);
  lcd.setCursor(4,1);
  lcd.print(minuteOnes);
}

void checkAlarm1(long current){
  // alarm will beep once per second
  long alarmInterval = 1000;
  static long previous = 0;
  static boolean blinkState = LOW;
  
  // If the clock and the alarm don't match,
  if(alarm1Array[0] != clockArray[3] || 
  alarm1Array[1] != clockArray[4]) {
    /*
     * Make sure a button press will acknowledge the alarm
     * the next time it goes off.
     */
    alarm1Acknowledged = false;
    // Reset the automatic turn-off.
    alarm1Elapsed = 0;
  }
  
  else if (alarm1Array[0] == clockArray[3] && // hour matches up
  alarm1Array[1] == clockArray[4] &&  // minute matches up
  alarm1Acknowledged == false){      // alarm has NOT been acknowledged with a button press or through an automatic reset
    anAlarmIsGoingOff = true;

    // blink an LED
    /*
     * #########################
     * #########################
     * #########################
     * If you want the program to do something other than blink an
     * LED and pulse a speaker, this is where you insert the code.
     * #########################
     * #########################
     * #########################
     */
    if(current - previous > alarmInterval) {
      // save the last time you blinked the pin 
      previous = current;   
      // if the pin is off turn it on and vice-versa:
      if (blinkState == LOW)
        blinkState = HIGH;
      else
        blinkState = LOW;
      // set the pin with the pinState of the variable:
      digitalWrite(ledPin, blinkState);
      if(blinkState == LOW){
        noTone(53);
      }
      if(blinkState == HIGH){
        tone(53, NOTE_G3);
      }
    }
              
    // wait for a button press
    /*
     * The recurringActionBlocker conditional is here so that the user
     * can only acknowledge one alarm at a time.  (If alarm1 AND the timer
     * are going off at the same time, the user will have to acknowledge both
     * alarms separteley [with two separte button clicks]).
     */
    if ((recurringActionBlocker == false) &&
    (lcd_key == btnSELECT ||
    lcd_key == btnUP ||
    lcd_key == btnDOWN ||
    lcd_key == btnLEFT ||
    lcd_key == btnRIGHT)){ 
      /*                 
       * Marking all the buttons as held down prevents unintended 
       * button-related events from ocurring. Say, for example,
       * the user is in the middle of setting the clock.  The cursor
       * is blinking, and a left or right button press will move
       * the cursor position.  The timer alarm starts going off.
       * The user presses the right button to acknowledge the alarm.
       * rightHeldDown is marked as true, and the cursor will stay put.                 
       */ 
      selectHeldDown = true;
      upHeldDown = true;
      downHeldDown = true;
      leftHeldDown = true;
      rightHeldDown = true;
      /*  
       *   Prevent button time variables from running
       *   in case the user keeps holding down a button
       *   after a timer reset.  For example, if the user
       *   presses the up button to reset the timer while the 
       *   timer alarm is going off, but he doesn't let go,
       *   this line of code prevents the up-button time 
       *   variables from changing, and thus prevents an unintended
       *   backlight state change.
       *   
       *   Setting recurringActionBlocker to true here also prevents 
       *   multiple alarms from being acknowledged with just 
       *   a single button press.
       */
      recurringActionBlocker = true;
      
      alarm1Acknowledged = true;
      anAlarmIsGoingOff = false;
      // turn the LED off;
      digitalWrite(ledPin, LOW);
      // turn the speaker off
      noTone(53);
      /*
       * Ensure that the next time the alarm goes off,
       * the LED and the speaker start flashing/ beeping immediately.
       */
      blinkState = LOW;
    } // End of manual button-press reset functions

    /*
     * Automatic reset: If the alarm has been blinking for more
     * than a minute, turn off the LED, turn off the speaker,
     * and acknowledge the alarm.
     */
    if (alarm1Elapsed >= 60000){
      // turn the LED off
      digitalWrite(ledPin, LOW);
      //turn the speaker off
      noTone(53);
      // ensure that the next time the alarm goes off, the alarm starts flashing/beeping immediately
      blinkState = LOW;
      anAlarmIsGoingOff = false;
      alarm1Acknowledged = true;
    } 
  }
}
  
void setAlarm1(long current){
  /*
   * Break up the main alarm variables into 
   * their individual place values. This makes for
   * easier printing to the LCD. It also makes it much
   * easier to blink a proper cursor. It also opens up
   * the possibility of allowing the user to 
   * change the both the Tens and Ones values
   * for each main alarm variable.
   */
  int hourTens = alarm1Array[0] / 10;
  int hourOnes = alarm1Array[0] % 10;
  int minuteTens = alarm1Array[1] / 10;
  int minuteOnes = alarm1Array[1] % 10;
  /*
   * The following variables (previous, cursorBlinkState,
   * cursorBlinkInterval) and the accompanying function are
   * used to blink a "cursor" without using the delay() function.
   */
  static long previous = 0;
  static boolean cursorBlinkState = LOW;
  long cursorBlinkInterval = 300;
  if(current - previous > cursorBlinkInterval) {
    // save the last time the "cursor" turned on 
    previous = current;  
    // if the cursor is off turn it on and vice-versa:
    if (cursorBlinkState == LOW)
      cursorBlinkState = HIGH;
    else
      cursorBlinkState = LOW;
  }

  lcd.setCursor(0,0);
  lcd.print("Alarm 1");
  lcd.setCursor(9,0);
  if (cursorPosition == 10 && cursorBlinkState == HIGH){
    if (alarm1State == true){
      lcd.print("\377N");
    }
    else{
      lcd.print("\377FF");
    }
  }
  else if (alarm1State == true){
    lcd.print("ON");
  }
  else if (alarm1State == false){
    lcd.print("OFF");
  }
  lcd.setCursor(0,1);
  lcd.print(hourTens);
  lcd.setCursor(1,1);
  if (cursorPosition == 18 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
  }
  else {
    lcd.print(hourOnes);
  }
  lcd.setCursor(2,1);
  lcd.print(":");
  lcd.setCursor(3,1);
  lcd.print(minuteTens);
  lcd.setCursor(4,1);
  if (cursorPosition == 21 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
  }
  else {
    lcd.print(minuteOnes);
  }

  /*
   * After switching screens, or switching form main screen
   * to setMode, cursorPosition is reset to position 1.
   * Change it to position 10 (setAlarm1State)
   */
   if (cursorPosition == 1){
    cursorPosition = 10;
   }

   if (cursorPosition == 10){
    setAlarm1State();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 21 (setAlarm1Minute).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 21;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 18 (setAlarm1Hour).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 18;
    }
  }

  if (cursorPosition == 18){
    setAlarm1Hour();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 10 (setAlarm1State).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 10;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 24 (setAlarm1Minute).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 21;
    }
  }

  if (cursorPosition == 21){
    setAlarm1Minute();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 18 (setAlarm1Hour).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 18;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 10 (setAlarm1State).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 10;
    }
  }
}

void setAlarm1State(){
  if (upPressTime > 0 && upHeldDown == false)  {
    lcd.clear();
    alarm1State = !alarm1State;
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    lcd.clear();
    alarm1State = !alarm1State;
  }
}

void setAlarm1Hour(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (alarm1Array[0] <= 22){
      alarm1Array[0]++;
    }
    else if (alarm1Array[0] == 23){
      alarm1Array[0] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (alarm1Array[0] >= 1){
      alarm1Array[0]--; 
    }
    else if (alarm1Array[0] == 0){
      alarm1Array[0] = 23; 
    }
  }
}

void setAlarm1Minute(){
  if (upPressTime > 0 && upHeldDown == false) { 
    if (alarm1Array[1] <= 58){
      alarm1Array[1]++;
    }
    else if (alarm1Array[1] == 59){
      alarm1Array[1] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (alarm1Array[1] >= 1){
      alarm1Array[1]--; 
    }
    else if (alarm1Array[1] == 0){
      alarm1Array[1] = 59; 
    }
  }
}

