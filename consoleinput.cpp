// Copyright Anthony D. Blaom 2012 

#include <iostream>
#include <string>
#include <sstream>
#include "consoleinput.h"

using namespace std;

float read_float_from_console(const string prompt) {
  float ret;
  string input = "";
  while (true) {
    cout << prompt << flush;
    getline(cin, input);
    stringstream ss(input);
    if (ss >> ret)
      break;
    cout << "Only floating point decimal input is allowed here. Please try again." << endl << endl;
  }
  return ret;
}

int read_int_from_console(const string prompt) {
  int ret;
  string input = "";
  while (true) {
    cout << prompt << flush;
    getline(cin, input);
    stringstream ss(input);
    if (ss >> ret)
      break;
    cout << "Only integer input is allowed here. Please try again." << endl << endl;
  }
  return ret;
}

void dbg_message(const string prompt) {
  cout << endl << prompt << flush;
}

