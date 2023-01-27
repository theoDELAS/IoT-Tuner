#include <Arduino.h>
#include "./models/Note.hpp"
#include "./models/Tuning.hpp"
#include <string>
using namespace std;

void setup() {
  Serial.begin(9600);
}

void loop() {
  note1.frequency = 440.0;
  note1.letter = 'A';
  note1.octave = 1;
  note1.offset = 10;

  Serial.println(note1.isValid(444));
  Serial.println(note1.isValid(300));
  delay(500);
}