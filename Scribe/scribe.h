#include <Arduino.h>

#define MAX_CHAR 100
#define MAX_LINES 5

// button pins for chars
const int prev_button_pin = 2;
const int next_button_pin = 15;

// button pins for lines
const int prev_line_button_pin = 5;
const int next_line_button_pin = 17;

// button pins for pages
// const int prev_page_button_pin = A0;
// const int next_page_button_pin = A1;

// led pins for beginning/end of line
const int last_line = 16;
const int end_line = 4;

// led pins for chars
const int led0 = 12;
const int led1 = 14;
const int led2 = 27;
const int led3 = 26;
const int led4 = 33;
const int led5 = 32;

// misc functions
char to_upper(char c);
bool is_lower(char c);
uint8_t decode(char c);

// bluetooth functions
// void bt_setup(SoftwareSerial& obj);
// void bt_loop(SoftwareSerial& obj);