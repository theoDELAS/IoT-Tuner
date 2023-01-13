#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <cstdlib> 
#include <ctime>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

char getCurrentTone(){
  char arrayNum[5] = {'E', 'B', 'G', 'D', 'A'};
  int RandIndex = rand() % 5;
  return arrayNum[RandIndex];
}

String getCurrentFrequencyInHz(){
  return "375Hz"; 
}

String getInstruction(){
  String arrayNum[2] = {"up  ", "down"};
  int RandIndex = rand() % 2;
  return arrayNum[RandIndex];
}

String getRateInHz(){
  String arrayNum[10] = {"10Hz  ", "78Hz  ", "132Hz  ", "187Hz  ", "199Hz  ", "204Hz  ", "286Hz  ", "361Hz  ", "429Hz  ", "559Hz  "};
  int RandIndex = rand() % 10;
  return arrayNum[RandIndex];
}

void displayRateInHz(){
  lcd.setCursor(9, 0);
  lcd.print(getRateInHz());
}

void displayFixedTextOnFirstLcdLine(){
  lcd.setCursor(0, 0);
  lcd.print("Note ");
}

void displayFixedTextOnSecondLcdLine(){
  lcd.setCursor(0, 1);
  lcd.print("Please try ");
}

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop(){
  displayFixedTextOnFirstLcdLine();
  displayFixedTextOnSecondLcdLine();
  displayRateInHz();
  lcd.setCursor(5, 0);
  lcd.print(getCurrentTone());
  lcd.setCursor(11, 1);
  lcd.print(getInstruction());
  delay(500);
}