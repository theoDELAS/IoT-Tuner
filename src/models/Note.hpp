#pragma once
#include <string>
using namespace std;

struct Note {
  char letter; 
  double frequency;
  int octave;
  double offset;

  string getFullName() {
    return letter + std::to_string(octave);
  }

  bool isValid(double inputFreq) {
    return ((inputFreq > frequency - offset) && (inputFreq < frequency + offset));
  }

} note1, note2, note3, note4, note5, note6, invalidNote;