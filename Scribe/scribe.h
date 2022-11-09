#pragma once
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <string>
#include <vector>
using namespace std;

#define MAX_PAGES 3

// BLUETOOTH
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void bt_setup(BluetoothSerial& SerialBT);
bool bt_loop(BluetoothSerial& SerialBT, vector<string>& page);

// button pins for chars
const int prev_button_pin = 2;
const int next_button_pin = 15;

// button pins for lines
const int prev_line_button_pin = 5;
const int next_line_button_pin = 17;

// button pins for pages
const int prev_page_button_pin = 19;
const int next_page_button_pin = 18;

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
string trim(string s);

// bluetooth functions
// void bt_setup(SoftwareSerial& obj);
// void bt_loop(SoftwareSerial& obj);