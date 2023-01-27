#pragma once
#include <string>
#include "./Note.hpp"
#include <array>
using namespace std;

struct Tuning {
  string name;
  std::array<Note,6> notes;
  Note invalidNote;
  
  
  Note getClosestNote(double frequency) {
    if (notes.empty()) { return invalidNote; };
    
    int diff = abs(notes[0].frequency - frequency) , idx{ 0 };
      for (int i = 0; i < sizeof(notes); i++) {
        if (abs(notes[i].frequency - frequency) < diff) {
            diff = abs(notes[i].frequency - frequency);
            idx = i;
        }
    }
    return notes[idx];
  }

  bool isValidNote(double inputFreq) {
    int offset = 12;
    return ((inputFreq >= notes[0].frequency - offset) && (inputFreq <= notes[sizeof(notes)].frequency + offset));
  }
};