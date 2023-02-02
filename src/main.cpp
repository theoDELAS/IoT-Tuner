#include "arduinoFFT.h"
#include <Arduino.h>
#include "./models/Note.hpp"
#include "./models/Tuning.hpp"
#include <string>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <math.h>
#include <AsyncTCP.h>
#include <cstdlib> 
#include <ctime>
using namespace std;
  
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

void displayDataOnLcdDisplay(String currentFullNoteName, String currentFrequency, String instruction){
  displayFixedTextOnFirstLcdLine();
  lcd.setCursor(5, 0);
  lcd.print(currentFullNoteName.c_str());
  lcd.setCursor(8, 0);
  lcd.print(currentFrequency + "Hz");
  lcd.setCursor(0, 1);
  lcd.print(instruction);
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

  note1.letter = 'E';
  note1.frequency = 82.41;
  note1.octave = 2;
  note1.offset = 10;

  note2.letter = 'A';
  note2.frequency = 110;
  note2.octave = 2;
  note2.offset = 10;

  note3.letter = 'D';
  note3.frequency = 146.83;
  note3.octave = 3;
  note3.offset = 10;

  note4.letter = 'G';
  note4.frequency = 196;
  note4.octave = 3;
  note4.offset = 10;

  note5.letter = 'B';
  note5.frequency = 246.94;
  note5.octave = 3;
  note5.offset = 10;

  note6.letter = 'E';
  note6.frequency = 329.63;
  note6.octave = 4;
  note6.offset = 10;

  invalidNote.letter = 'Z';
  invalidNote.frequency = 999;
  invalidNote.octave = 10;
  invalidNote.offset = 10;

  standard.name = "Standard";
  standard.notes = {{
    note1, note2, note3, note4, note5, note6
  }};
  standard.invalidNote = invalidNote;

  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  double roundedPeak = round(peak);

  char roundedPeakStr[64];
  snprintf(roundedPeakStr, sizeof(roundedPeakStr), "%g", roundedPeak);

  Note closestNote = standard.getClosestNote(roundedPeak);

  char closestNoteFullNameStr[64];
  snprintf(closestNoteFullNameStr, sizeof(closestNoteFullNameStr), "%g", closestNote.getFullName());

  char closestNoteFrequencyStr[64];
  snprintf(closestNoteFrequencyStr, sizeof(closestNoteFrequencyStr), "%g", closestNote.frequency);

  String instruction = standard.getInstruction(roundedPeak, closestNote.frequency).c_str();

  displayDataOnLcdDisplay(closestNote.getFullName().c_str(), roundedPeakStr, instruction);

  Serial.println(closestNoteFullNameStr);

  String analyzedNoteData = "{frequency: " + String(roundedPeak) + ", closestNote: " + closestNote.getFullName().c_str() + ", instruction: " + instruction + '}';

  // String all = String(roundedPeak) + " " + closestNote.getFullName().c_str() + " " + instruction;

  ws.textAll(analyzedNoteData);

  delay(40); //do one time
}