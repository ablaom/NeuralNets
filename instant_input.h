#ifndef GUARD_instant_input
#define GUARD_instant_input

/* header file for functions to switch on and off instant input
   terminal mode. For most applications, just use the function
   <get_inkey> described below */
#include <string>

void reset_input_mode ();
void set_input_mode ();

/* This next routine waits for input of an unsigned integer of length
   <length> (with leading zeros allowed), or a single non-digit
   character. It returns the integer if one is entered and -1 if a
   lone non-digit char is entered instead. In the latter case the
   non-digit char is stored in the char referenced by
   <action_code>. In the former case, <action_code> is set to '0'. If
   digits are followed by non-digits, or a successfully entered digit
   is strictly bigger than <bound>, then the routine complains and
   asks the user to try again. To reject all digit input, call the
   routine with bound = -1 and length = 0. */
int get_inkey(char& action_code, const int bound, const int length, const std::string prompt);

bool del_last_char(std::string&);
bool is_integer(const std::string);


#endif
