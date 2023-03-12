#pragma once
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <string>
#include <vector>
using namespace std;

#define MAX_PAGES 3
#define BURST_LEN 5

// BLUETOOTH
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// button pins for chars
const int prev_burst_button_pin = 2;
const int next_burst_button_pin = 15;

// button pins for pages
const int prev_page_button_pin = 19;
const int next_page_button_pin = 18;

// led pins for beginning/end of line
const int last_burst_pin = 4;
const int last_page_pin = 16;


// led pins for chars
const int led0 = 12;
const int led1 = 14;
const int led2 = 27;
const int led3 = 26;
const int led4 = 33;
const int led5 = 32;


/* ========== BLUETOOTH ========== */


/**
 * @brief Begin ESP32 bluetooth server.
 * 
 * @param SerialBT Serial bluetooth object.
 */
void bt_setup(BluetoothSerial& SerialBT);

/**
 * @brief Check to see if a page is available to read. If one is, then read it.
 * 
 * @param SerialBT Serial bluetooth object, since we are checking for page from over BT.
 * @param page Object to store the loaded page.
 * @return true The page object contains a new page.
 * @return false The page object does not contain a new page.
 */
bool bt_loop(BluetoothSerial& SerialBT, vector<string>& page);



/* ========== MISC FUNCTIONS ========== */

/**
 * @brief Return the uppercase version of passed character.
 * 
 * @param c Lowercase character.
 * @return char Uppercase version of passed character.
 */
char to_upper(char c);

/**
 * @brief Checks if character is a lowercase letter.
 * 
 * @param c Any character.
 * @return true The passed character is a lowercase letter.
 * @return false The passed character is not a lowercase letter.
 */
bool is_lower(char c);

/**
 * @brief Decodes the Braille ASCII of a character.
 * 
 * @param c Any character.
 * @return uint8_t Braille ASCII version of passed character.
 */
uint8_t decode(char c);

/**
 * @brief Trims newline and line feed characters off string.
 * 
 * @param s Any string.
 * @return string Passed string with no newline or line feed characters.
 */
string trim(string s);

/**
 * @brief Prints the current line and accentuates the current letter.
 * 
 * @param page_ind Current page number in book.
 * @param line_ind Current line number in page.
 * @param str_ind Current letter number in line.
 * @param book_size Total pages in book.
 * @param page_size Total lines in page.
 * @param curr_line Current line.
 */
void print_info(int page_ind, int burst_ind, int book_size, int page_size, string curr_page, string curr_burst);

/**
 * @brief Return burst given the current page and burst index.
 * 
 * @param current_page Current page.
 * @param burst_index Burst index.
 * @return string Burst at the given burst index.
 */
string burst_from_page(string current_page, int burst_index);

/**
 * @brief Determines whether or not the current burst is the last burst of the page.
 * 
 * @return true The current burst is the last burst.
 * @return false The current burst is not the last burst.
 */
bool last_burst(string curr_page, int burst_ind);

/**
 * @brief Translate single char to string of a braille ascii byte.
 * 
 * @param chr Single character to convert to braille ascii.
 * @return string String of braille ascii byte from char.
 */
string char_to_braille(char chr);

/**
 * @brief Translate burst string to string of braille ascii bytes.
 * 
 * @param burst Burst to convert to braille ascii.
 * @return string String of braille ascii bytes from burst.
 */
string burst_to_braille(string burst);