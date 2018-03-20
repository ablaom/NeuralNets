#ifndef GUARD_consoleinput
#define GUARD_consoleinput

// Copyright Anthony D. Blaom 2012
#include <string>

float read_float_from_console(const std::string prompt);
int read_int_from_console(const std::string prompt);
void dbg_message(const std::string prompt);

#endif
