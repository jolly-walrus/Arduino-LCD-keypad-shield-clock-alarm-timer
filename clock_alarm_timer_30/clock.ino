void clock(){
  
  /*
   * Break up the clock variables into
   * their individual place values.  This allows 
   * each digit to be printed individually.
   */
  int dayTens = clockArray[2] / 10;
  int dayOnes = clockArray[2] % 10;
  int yearThousands = clockArray[0] / 1000;
  int yearHundreds = (clockArray[0] % 1000) / 100;
  int yearTens = (clockArray[0] % 100) / 10;
  int yearOnes = clockArray[0] % 10;
  int hourTens = clockArray[3] / 10;
  int hourOnes = clockArray[3] % 10;
  int minuteTens = clockArray[4] / 10;
  int minuteOnes = clockArray[4] % 10;
  int secondTens = clockArray[5] / 10;
  int secondOnes = clockArray[5] % 10;

  lcd.setCursor(0,0);
  lcd.print(dayName[clockArray[6]]);
  lcd.setCursor(4,0);
  lcd.print(monthName[clockArray[1]-1]);
  lcd.setCursor(8,0);
  lcd.print(dayTens);
  lcd.setCursor(9,0);
  lcd.print(dayOnes);
  lcd.setCursor(11,0);
  lcd.print(yearThousands);
  lcd.setCursor(12,0);
  lcd.print(yearHundreds);
  lcd.setCursor(13,0);
  lcd.print(yearTens);
  lcd.setCursor(14,0);
  lcd.print(yearOnes);
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
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.setCursor(6,1);
  lcd.print(secondTens);
  lcd.setCursor(7,1);
  lcd.print(secondOnes);
}
  
void setClock(long current){
  /*
   * The following variables (previous, cursorBlinkState,
   * cursorBlinkInterval) and the accompanying function are
   * used to blink a "cursor" without using the delay() function.
   * I don't want any delays other than those used for debouncing
   * the keypad buttons.
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
  
  // set the: year, month, day, hour, minute, second
  rtc.adjust(DateTime(clockArray[0], clockArray[1], clockArray[2], clockArray[3], clockArray[4], clockArray[5]));
   
  //take the current month and break it up into an array called localMonth
  //This allows each letter to be printed individually
  char localMonth[4];
  String(monthName[clockArray[1]-1]).toCharArray(localMonth, 4);
  // take the clock variables and break them up into 
  // their individual place values.  This allows each digit
  // to be printed individually.
  int dayTens = clockArray[2] / 10;
  int dayOnes = clockArray[2] % 10;
  int yearThousands = clockArray[0] / 1000;
  int yearHundreds = (clockArray[0] % 1000) / 100;
  int yearTens = (clockArray[0] % 100) / 10;
  int yearOnes = clockArray[0] % 10;
  int hourTens = clockArray[3] / 10;
  int hourOnes = clockArray[3] % 10;
  int minuteTens = clockArray[4] / 10;
  int minuteOnes = clockArray[4] % 10;
  int secondTens = clockArray[5] / 10;
  int secondOnes = clockArray[5] % 10;
  
  lcd.setCursor(4,0);
  if (cursorPosition == 5 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
    lcd.print(localMonth[1]);
    lcd.print(localMonth[2]);
  }
  else {
    lcd.print(localMonth[0]);
    lcd.print(localMonth[1]);
    lcd.print(localMonth[2]);
  }
  lcd.setCursor(8,0);
  lcd.print(dayTens);
  lcd.setCursor(9,0);
  if (cursorPosition == 10 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
  }
  else {
    lcd.print(dayOnes);
  }
  lcd.setCursor(11,0);
  lcd.print(yearThousands);
  lcd.print(yearHundreds);
  lcd.print(yearTens);
  if (cursorPosition == 15 && cursorBlinkState == HIGH){
    lcd.write(B11111111);
  }
  else {
    lcd.print(yearOnes);
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
   * Change it to position 5 (setClockMonth)
   */
   if (cursorPosition == 1){
    cursorPosition = 5;
   }

   if (cursorPosition == 5){
    setClockMonth();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 24 (setClockSecond).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 24;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 10 (setClockDay).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 10;
    }
  }

  if (cursorPosition == 10){
    setClockDay();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 5 (setClockMonth).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 5;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 15 (setClockYear).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 15;
    }
  }

  if (cursorPosition == 15){
    setClockYear();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 10 (setClockDay).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 10;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 18 (setClockHour).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 18;
    }
  }

  if (cursorPosition == 18){
    setClockHour();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 15 (setClockYear).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 15;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 21 (setClockMinute).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 21;
    }
  }

  if (cursorPosition == 21){
    setClockMinute();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 18 (setClockHour).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 18;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 24 (setClockSecond).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 24;
    }
  }

  if (cursorPosition == 24){
    setClockSecond();
    /*
     * If the user presses the LEFT button, and the LEFT button is not already held down,
     * move the cursor to position 21 (setClockMinute).
     */
    if (lcd_key == btnLEFT && leftHeldDown == false){
      leftHeldDown = true;
      cursorPosition = 21;
      }
    /*           
     * If the user presses the RIGHT button, and the RIGHT button is not already held down,           
     * move the cursor to position 5 (setClockMonth).
     */
    if (lcd_key == btnRIGHT && rightHeldDown == false){
      rightHeldDown = true;
      cursorPosition = 5;
    }
  }
}

void setClockDay(){
  if (upPressTime > 0 && upHeldDown == false){ 
    // If the month has 31 days
    if (clockArray[1] == 1 || 
    clockArray[1] == 3 || 
    clockArray[1] == 5 || 
    clockArray[1] == 7 || 
    clockArray[1] == 8 || 
    clockArray[1] == 10 || 
    clockArray[1] == 12){
      if (clockArray[2] <= 30){
        clockArray[2]++;
      }
      else if (clockArray[2] == 31){
        clockArray[2] = 1;
      }
    }
    // if the month has 30 days
    else if (clockArray[1] == 4 || 
    clockArray[1] == 6 || 
    clockArray[1] == 9 || 
    clockArray[1] == 11){
      if (clockArray[2] <= 29){
        clockArray[2]++;
      }
      else if (clockArray[2] == 30){
        clockArray[2] = 1;
      }
    }
    // February and not a leap year
    else if (clockArray[1] == 2 && clockArray[0] % 4 != 0){
      if (clockArray[2] <= 27){
        clockArray[2]++;
      }
      else if (clockArray[2] == 28){
        clockArray[2] = 1;
      }
    }
    // February AND a leap year
    else if(clockArray[1] == 2 && clockArray[0] % 4 == 0){
      if (clockArray[2] <= 28){
        clockArray[2]++;
      }
      else if (clockArray[2] == 29){
        clockArray[2] = 1;
      }
    }
    // reset the up button
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN  && downHeldDown == false){
    downHeldDown = true;
    // If the month has 31 days
    if (clockArray[1] == 1 || 
    clockArray[1] == 3 || 
    clockArray[1] == 5 || 
    clockArray[1] == 7 || 
    clockArray[1] == 8 || 
    clockArray[1] == 10 || 
    clockArray[1] == 12){
      if (clockArray[2] >= 2){
        clockArray[2]--;
      }
      else if (clockArray[2] == 1){
        clockArray[2] = 31;
      }
    }
    // if the month has 30 days
    else if (clockArray[1] == 4 || 
    clockArray[1] == 6 || 
    clockArray[1] == 9 || 
    clockArray[1] == 11){
      if (clockArray[2] >= 2){
        clockArray[2]--;
      }
      else if (clockArray[2] == 1){
        clockArray[2] = 30;
      }
    }
    // February and not a leap year
    else if (clockArray[1] == 2 && clockArray[0] % 4 != 0){
      if (clockArray[2] >= 2){
        clockArray[2]--;
      }
      else if (clockArray[2] == 1){
        clockArray[2] = 28;
      }
    }
    else if(clockArray[1] == 2 && clockArray[0] % 4 == 0){  // February AND a leap year
      if (clockArray[2] >= 2){
        clockArray[2]--;
      }
      else if (clockArray[2] == 1){
        clockArray[2] = 29;
      }
    }
  }
}

void setClockMonth(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (clockArray[1] <= 11){
      clockArray[1]++;
    }
    else if (clockArray[1] == 12){
      clockArray[1] = 1;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (clockArray[1] >= 2){
      clockArray[1]--; 
    }
    else if (clockArray[1] == 1){
      clockArray[1] = 12; 
    }
  }
}

void setClockYear(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (clockArray[0] <= 2098){
      clockArray[0]++;
    }
    else if (clockArray[0] == 2099){
      clockArray[0] = 2014;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (clockArray[0] >= 2015){
      clockArray[0]--; 
    }
    else if (clockArray[0] == 2014){
      clockArray[0] = 2099; 
    }
  }
}

void setClockHour(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (clockArray[3] <= 22){
      clockArray[3]++;
    }
    else if (clockArray[3] == 23){
      clockArray[3] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (clockArray[3] >= 1){
      clockArray[3]--; 
    }
    else if (clockArray[3] == 0){
      clockArray[3] = 23; 
    }
  }
}

void setClockMinute(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (clockArray[4] <= 58){
      clockArray[4]++;
    }
    else if (clockArray[4] == 59){
      clockArray[4] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (clockArray[4] >= 1){
      clockArray[4]--; 
    }
    else if (clockArray[4] == 0){
      clockArray[4] = 59; 
    }
  }
}

void setClockSecond(){
  if (upPressTime > 0 && upHeldDown == false){  
    if (clockArray[5] <= 58){
      clockArray[5]++;
    }
    else if (clockArray[5] == 59){
      clockArray[5] = 0;
    }
    upPressTime = 0;
  }
  if (lcd_key == btnDOWN && downHeldDown == false){
    downHeldDown = true;
    if (clockArray[5] >= 1){
      clockArray[5]--; 
    }
    else if (clockArray[5] == 0){
      clockArray[5] = 59; 
    }
  }
}

