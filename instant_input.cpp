#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sstream>
#include <iostream>
#include <string>
#include <cctype>

using namespace std;

/* Use this variable to remember original terminal attributes. */

struct termios default_terminal_attributes;

void reset_input_mode (void) {
  tcsetattr (STDIN_FILENO, TCSANOW, &default_terminal_attributes);
}

void set_input_mode () {
  struct termios tattr;
  char *name;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &default_terminal_attributes);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

/* This routine waits for input of an unsigned integer of length
   <length> (with leading zeros allowed), or a single non-digit
   character. It returns the integer if one is entered and -1 if a
   lone non-digit char is entered instead. In the latter case the
   non-digit char is stored in the char referenced by
   <action_code>. In the former case, <action_code> is set to '0'. If
   digits are followed by non-digits, or a successfully entered digit
   is strictly bigger than <bound>, then the routine complains and
   asks the user to try again. To reject all digit input, call the
   routine with bound = -1 and length = 0. */
int get_inkey(char& action_code, const int bound, const int length, const string prompt){
  static const char DELETE_KEY_CHAR = static_cast<char>(127);
  static const char RETURN_KEY_CHAR = static_cast<char>(10);

  bool del_last_char(string&);
  bool is_integer(const string);

  char input_char;
  string input_str;
  int cursor_pos;
  int temp;

  int ret = -2;
  while (ret < -1) {
    // initialize the input variables
    input_char = '\0';
    input_str = ""; 
    cursor_pos = 0;

    // get the input
    cout << prompt << flush;
    set_input_mode();
    while ((cursor_pos < max(1,length)) && !(cursor_pos == 1 && 
!(isdigit(input_char) || input_char == DELETE_KEY_CHAR || input_char == RETURN_KEY_CHAR))) {
      read(STDIN_FILENO, &input_char, 1);
      if (input_char == DELETE_KEY_CHAR) {
	if (cursor_pos != 0) {
	  --cursor_pos;
	  if (del_last_char(input_str) == false) {
	    cout << "Error in metafield.cpp: "
		 << "Applying del_last_char to a zero-length string"
		 << endl;
	  }
	  cout << '\b' << flush;
	  fflush(stdout);
	}
      } else if (input_char != RETURN_KEY_CHAR) {
	++cursor_pos;
	input_str=input_str + input_char;
	putchar(input_char);
	fflush(stdout);
      }
    }
    reset_input_mode();
    if (cursor_pos == 1 && !isdigit(input_char)) {
      ret = -1;
      action_code = input_char;
    } else if (is_integer(input_str)) {
      stringstream ss(input_str); /* this line and the next put int
			  version of input_str into ret */
      ss >> ret;
      if (ret > bound) {
	ret = -2;
	cout << endl << "Invalid entry." << endl;
      } else {
	action_code = '0'; }
    } else {
      cout << endl << "Invalid entry." << endl;
    }
  }
  return ret;
}

/* the following returns true and deletes one character from the
   referenced string <s> if <s> has non-zero length, and
   otherwise returns false.*/
bool del_last_char(string& s) {
  if (s.size() !=0) {
  s = s.substr(0,s.size()-1);
  return true;
  } else {
    return false;
  }
}

/* if the string parameter represents an unsigned integer the function
   returns true; false otherwise. */
bool is_integer(const string s) {
  const string::size_type len = s.size();
  bool ret = true;
  for (int i = 0; i != len; ++i) {
    if (!isdigit(s[i])) ret = false; 
  }
  if (len == 0) ret = false;
  return ret;
}

