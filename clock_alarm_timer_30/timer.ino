void timer(long current){
  /*
   * Break timeLeft into individual values
   * (hours left, minutes left, seconds left,
   * milliseconds left) and also break those
   * into individual place values.  This makes printing
   * to the LCD and blinking the "cursor" much simpler.
   */
  int hoursLeftTens = (timeLeft / 3600000) / 10;
  int hoursLeftOnes = (timeLeft / 3600000) % 10;
  int minutesLeftTens = ((timeLeft % 3600000) / 60000) / 10;
  int minutesLeftOnes = ((timeLeft % 3600000) / 60000) % 10;
  int secondsLeftTens = ((timeLeft % 60000) / 1000) / 10;
  int secondsLeftOnes = ((timeLeft % 60000) / 1000) % 10;
  int millisecondsLeftTens = ((timeLeft % 1000) / 10) / 10;
  int millisecondsLeftOnes = ((timeLeft % 1000) / 10) % 10;
  /*
   * The following variables (previous, cursorBlinkState,
   * cursorBlinkInterval) and the accompanying function are
   * used to blink a "cursor" without using the delay() function.
   */
  static long previous = 0;
  static boolean cursorBlinkState = LOW;
  long cursorBlinkInterval = 500;
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
  lcd.print("Timer1");
    
  /*
   * If the timer is running, and the cursor is
   * hovering over the start/pause position,
   * print the "pause" symbol and blink a "cursor."
   * If the timer is not running, and the cursor is
   * hovering over the start/pause position,
   * print the "play" symbol and blink a "cursor."
   * If the cursor is NOT hovering over the start/pause
   * position, simply print the appropriate symbol; don't 
   * blink the cursor.
   */
  lcd.setCursor(8,0);
  if (cursorPosition == 9 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
  }
  else if (timerIsRunning == false){
    lcd.write(9);
  }
  else if (timerIsRunning == true){
    lcd.write(8);
  }
  lcd.setCursor(11,0);
  if (cursorPosition == 12 && cursorBlinkState == HIGH){
    lcd.print("\377eset");
  }
  else {
    lcd.print("Reset");
  }
  lcd.setCursor(0,1);
  lcd.print(hoursLeftTens);
  lcd.setCursor(1,1);
  lcd.print(hoursLeftOnes);
  lcd.setCursor(2,1);
  lcd.print(":");
  lcd.setCursor(3,1);
  lcd.print(minutesLeftTens);
  lcd.setCursor(4,1);
  lcd.print(minutesLeftOnes);
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.setCursor(6,1);
  lcd.print(secondsLeftTens);
  lcd.setCursor(7,1);
  lcd.print(secondsLeftOnes);
  lcd.setCursor(8,1);
  lcd.print(":");
  lcd.setCursor(9,1);
  lcd.print(millisecondsLeftTens);
  lcd.setCursor(10,1);
  lcd.print(millisecondsLeftOnes);

  /*
   * Every time the screen switches, the cursorPosition is reset to position 1.
   * Here on the timer screen, immediately set cursorPosition to position 9 (start/pause).
   */
  if (cursorPosition == 1){
    cursorPosition = 9;
  }
  
  // if cursor is on position 9, call startOrPauseTimer().
  if (cursorPosition == 9){
    startOrPauseTimer();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to the Reset position.
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 12;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to the Reset position
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 12;
    }
  }

  /*
   * if the cursor is on the reset position, wait for an UP or Down
   * button press to call the resetTimer() function
   */
  if (cursorPosition == 12){
    if (upPressTime > 0 && upHeldDown == false){
      upPressTime = 0;
      resetTimer();
    }
    if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    resetTimer();
    }
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to the start/pause position.
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 9;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to the start/pause position
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 9;
    }
  }
}

/*
 * "current" is currentMillis, sent from the main loop.
 */
void checkTheTimer(long current){
  /*
   * timerBeepInterval is the number of millisecons between beeps
   * when the timer is going off.
   */
  long timerBeepInterval = 500;  
  static long previous = 0;
  static boolean blinkState = LOW;
  timeLeft = totalTime - cumulativeOn;
  if (timerIsRunning == false){
    lastCumulative = cumulativeOn;
    timeElapsed = 0;
    timerAlarmElapsed = 0;
  }
  if (timerIsRunning == true && timeLeft > 0){
    cumulativeOn = lastCumulative + timeElapsed;
    timerAlarmElapsed = 0;
  }
  
  /*
   * If the timer has run out, take the following actions.
   * 
   * ################################
   * ################################
   * ################################
   * If you want to make the program do something else other than blink
   * an LED and pulse a speaker when the timer runs out, 
   * this is where you insert your code.
   * ################################
   * ################################
   * ################################
   */
  // timerAlarmElapsed controls how long the timer alarm will beep
  if(timerIsRunning == true && timeLeft <= 0 && timerAlarmElapsed < 30000){
    // Display all zeroes
    timeLeft = 0; 
    anAlarmIsGoingOff = true;
    // blink an LED and pulse a tone on the speaker
    if(current - previous > timerBeepInterval) {
      // save the last time you blinked the pin 
      previous = current;   
      // if the pin is off turn it on and vice-versa:
      if (blinkState == LOW)
        blinkState = HIGH;
      else
        blinkState = LOW;
      // set the pin with the pinState of the variable:
      digitalWrite(ledPin, blinkState);
      // pulse a speaker.
      if(blinkState == LOW){
        noTone(53);
      }
      if(blinkState == HIGH){
        tone(53, NOTE_D3);
      }
    }
              
    // wait for a button press
    /*
      The recurringActionBlocker conditional is here so that the user
      can only acknowledge one alarm at a time.  (If alarm1 AND the timer
      are going off at the same time, the user will have to acknowledge both
      alarms separteley [with two separte button clicks]).
    */
   if ((recurringActionBlocker == false) &&
    (lcd_key == btnSELECT ||
    lcd_key == btnUP ||
    lcd_key == btnDOWN ||
    lcd_key == btnLEFT ||
    lcd_key == btnRIGHT)){
      /*  
       *   Marking all the buttons as held down prevents unintended
       *   button related events from occurring.  Say, for example,
       *   the user is in the middle of setting the clock.  the cursor
       *   is blinking, and a left or right button press will move
       *   the cursor position.  The timer alarm starts going off.
       *   The user presses the RIGHT button to acknowledge the alarm.
       *   rightHeldDown is marked as true, so the cursor stays put.
       */
      selectHeldDown = true;
      upHeldDown = true;
      downHeldDown = true;
      leftHeldDown = true;
      rightHeldDown = true; 
    
      /*
       * Prevent button time variables from running
       *  in case the user keeps holding down a button
       *  after a timer reset.  For example, if the user
       *  presses the up button to reset the timer while the 
       *  timer alarm is going off, but he doesn't let go,
       *  this line of code prevents the up-button time 
       *  variables from changing, and thus prevents an unintended
       *  backlight state change.
       */
      recurringActionBlocker = true;
      
      lcd.clear();
      timerIsRunning = false;
      resetTimer();
      blinkState = LOW;
      digitalWrite(ledPin, LOW);
      noTone(53);
    } // End of manual (button press) timer reset
    
  }

  /*
   * Auto-reset: If no button is pressed after 30s,
   * reset the timer.
   */
  if(timerIsRunning == true && timeLeft <= 0 && timerAlarmElapsed >= 30000){
    blinkState = LOW;
    digitalWrite(ledPin, LOW);
    noTone(53);
    resetTimer();
  }
    
}

void startOrPauseTimer(){
  /*
   * If either the up or down button is pushed,
   * and an alarm is not going off, start or pause the timer.
   */
  if (upPressTime > 0 && upHeldDown == false && timeLeft > 0){
    lcd.clear();
    timerIsRunning = !timerIsRunning;
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false && timeLeft > 0){
    downHeldDown = true;
    lcd.clear();
    timerIsRunning = !timerIsRunning;
  } 
}
  
void setTimer(long current){
  totalTime = timerArray[0] * 3600000 + timerArray[1] * 60000 + timerArray[2] * 1000;
  int hourTens = timerArray[0] / 10;
  int hourOnes = timerArray[0] % 10;
  int minuteTens = timerArray[1] / 10;
  int minuteOnes = timerArray[1] % 10;
  int secondTens = timerArray[2] / 10;
  int secondOnes = timerArray[2] % 10;
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
  lcd.print("Hr:Mn:Sc");
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
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.setCursor(6,1);
  lcd.print(secondTens);
  lcd.setCursor(7,1);
  if (cursorPosition == 24 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
  }
  else {
    lcd.print(secondOnes);
  }

  /*
   * After switching screens, or switching form main screen
   * to setMode, cursorPosition is reset to position 1.
   * Change it to position 18 (hour)
   */
   if (cursorPosition == 1){
    cursorPosition = 18;
   }

  if (cursorPosition == 18){
    setTimerHour();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 24 (seconds).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 24;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 21 (minutes).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 21;
    }
  }

  if (cursorPosition == 21){
    setTimerMinute();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 18 (hours).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 18;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 24 (seconds).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 24;
    }
  }

  if (cursorPosition == 24){
    setTimerSecond();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 21 (minutes).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 21;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 18 (hours).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 18;
    }
  }
} 

void setTimerHour(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (timerArray[0] <= 98){
      timerArray[0]++;
    }
    else if (timerArray[0] == 99){
      timerArray[0] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (timerArray[0] >= 1){
      timerArray[0]--; 
    }
    else if (timerArray[0] == 0){
      timerArray[0] = 99; 
    }
  }
}

void setTimerMinute(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (timerArray[1] <= 58){
      timerArray[1]++;
    }
    else if (timerArray[1] == 59){
      timerArray[1] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (timerArray[1] >= 1){
      timerArray[1]--; 
    }
    else if (timerArray[1] == 0){
      timerArray[1] = 59; 
    }
  }
}

void setTimerSecond(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (timerArray[2] <= 58){
      timerArray[2]++;
    }
    else if (timerArray[2] == 59){
      timerArray[2] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (timerArray[2] >= 1){
      timerArray[2]--; 
    }
    else if (alarm1Array[1] == 0){
      timerArray[2] = 59; 
    }
  }
}

void resetTimer(){
  lcd.clear();
  anAlarmIsGoingOff = false;
  timerIsRunning = false;
  cumulativeOn = 0;
  timeLeft = totalTime;
  digitalWrite(52, LOW);
}

