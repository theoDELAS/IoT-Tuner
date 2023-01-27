#include "arduinoFFT.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <math.h>
#include <AsyncTCP.h>
#include <cstdlib> 
#include <ctime>
  
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const char* ssid = "Nokia3310";
const char* password =  "ynoviotmaster";
  
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String getCurrentFrequencyInHz(){
  return "375Hz"; 
}

String getRandomInstruction(){
  String arrayNum[2] = {"up  ", "down"};
  int RandIndex = rand() % 2;
  return arrayNum[RandIndex];
}

void displayFixedTextOnFirstLcdLine(){
  lcd.setCursor(0, 0);
  lcd.print("Note ");
}

void displayFixedTextOnSecondLcdLine(){
  lcd.setCursor(0, 1);
  lcd.print("Please try ");
}

void displayDataOnLcdDisplay(String currentTone){
  displayFixedTextOnFirstLcdLine();
  displayFixedTextOnSecondLcdLine();

  lcd.setCursor(5, 0);
  lcd.print(currentTone + "Hz");
  lcd.setCursor(11, 1);
  lcd.print(getRandomInstruction());
}
  
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  
  if(type == WS_EVT_CONNECT){
  
    Serial.println("Websocket client connection received");
     
  } else if(type == WS_EVT_DISCONNECT){
 
    Serial.println("Client disconnected");
  
  }
}
  
void setup(){
  Serial.begin(115200);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
  
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  server.begin();

  samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
}
  
void loop(){
  for(int i=0; i<SAMPLES; i++)
  {
      microSeconds = micros(); 
    
      vReal[i] = analogRead(35);
      vImag[i] = 0;

      while(micros() < (microSeconds + samplingPeriod))
      {
        //do nothing
      }
  }

  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  double roundedPeak = round(peak);
  Serial.println(roundedPeak);

  char roundedPeakStr[64];
  snprintf(roundedPeakStr, sizeof(roundedPeakStr), "%g", roundedPeak);

  ws.textAll(roundedPeakStr);
  displayDataOnLcdDisplay(roundedPeakStr);
  delay(40); //do one time
}
  