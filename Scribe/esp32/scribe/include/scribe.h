#pragma once
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <string>
#include <vector>
using namespace std;

#define MAX_PAGES 3 // most amount of pages allowed in a book
#define BURST_LEN 5 // number of characters in a burst
#define DEBOUNCE_DELAY 250 // debounce time

// BLUETOOTH CHECK
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// DEFINE PIN NUMBERS
const int slave_select_pin = 26, mosi_pin = 25, miso_pin = 33, sck_pin = 32;
const int prev_burst_button_pin=2, next_burst_button_pin=15;
const int prev_page_button_pin=19, next_page_button_pin=18;

/* =========== BT FUNCTIONS =========== */

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


/* ========== CORE FUNCTIONS ========== */

/**
 * @brief Checks if a button was pressed. Different functionality based on which button is pressed.
 * 
 * @param button_pin Pin of ESP32 to which the button is connected.
 * @param lbt Last debounce time of button.
 * @param button State of the button (HIGH/LOW).
 * @param cl Tells whether the button is for a burst or page.
 * @param np Tells whether the button is for the next or previous.
 * @param SerialBT Serial BlueTooth object.
 * @param page Holder for new page.
 * @param book Entire book.
 * @param curr_page Current page.
 * @param curr_burst Current burst.
 * @param page_ind Page index.
 * @param burst_ind Burst index.
 */
void poll(const int button_pin, long &lbt, int &button, char bp, char np, BluetoothSerial& SerialBT, vector<string>& page, vector<string>& book, string& curr_page, string& curr_burst, int& page_ind, int& burst_ind);

/**
 * @brief Update the burst index and get the next burst in a page.
 * 
 * @param curr_page Current page.
 * @param curr_burst Current burst.
 * @param page_ind Page index.
 * @param burst_ind Burst index.
 * @return string Next burst.
 */
string next_burst(string& curr_page, string& curr_burst, int& page_ind, int& burst_ind);

/**
 * @brief Update the burst index and get the prev burst in a page.
 * 
 * @param curr_page Current page.
 * @param curr_burst Current burst.
 * @param page_ind Page index.
 * @param burst_ind Burst index.
 * @return string Previous burst.
 */
string prev_burst(string& curr_page, string& curr_burst, int& page_ind, int& burst_ind);

/**
 * @brief Updates the page index and returns the next page. If on the last page, then calls get_page.
 * 
 * @param book Entire book.
 * @param curr_burst Current burst.
 * @param page_ind Page index.
 * @param burst_ind Burst index.
 * @param SerialBT Serial BlueTooth object.
 * @param page Holder for new page.
 * @return string Next page.
 */
string next_page(vector<string>& book, string& curr_burst, int& page_ind, int& burst_ind, BluetoothSerial& SerialBT, vector<string>& page);

/**
 * @brief Updates the page index and returns the previous page.
 * 
 * @param book Entire book.
 * @param curr_burst Current burst.
 * @param page_ind Page index.
 * @param burst_ind Burst index.
 * @return string Previous page.
 */
string prev_page(vector<string>& book, string& curr_burst, int& page_ind, int& burst_ind);

/**
 * @brief Calls bt_loop. If a new page is loaded, put the new page in the book.
 * 
 * @param SerialBT Serial BlueTooth object.
 * @param page Holder to get new page.
 * @param book Entire book.
 * @param curr_burst Current burst.
 * @param page_ind Page index.
 * @param burst_ind Burst index.
 */
void get_page(BluetoothSerial& SerialBT, vector<string>& page, vector<string>& book, string& curr_burst, int& page_ind, int& burst_ind);

/**
 * @brief Decodes the Braille ASCII of a character.
 * 
 * @param c Any character.
 * @return uint8_t Braille ASCII version of passed character.
 */
uint8_t decode(char c);

/**
 * @brief Return burst given the current page and burst index.
 * 
 * @param current_page Current page.
 * @param burst_index Burst index.
 * @return string Burst at the given burst index.
 */
string burst_from_page(string current_page, int burst_index);

/**
 * @brief Translate burst string to string of braille ascii bytes.
 * 
 * @param burst Burst to convert to braille ascii.
 * @return string String of braille ascii bytes from burst.
 */
string burst_to_braille(string burst);

/**
 * @brief Determines whether or not the current burst is the last burst of the page.
 * 
 * @param curr_page Current page.
 * @param burst_ind Location of current burst.
 * @return true The current burst is the last burst.
 * @return false The current burst is not the last burst.
 */
bool last_burst(string curr_page, int burst_ind);


/* ========= HELPER FUNCTIONS ========= */

/**
 * @brief Convert page from a vector of strings to one string.
 * 
 * @param old_page Page as vector of strings.
 * @return string Page as one string.
 */
string change_page_format(vector<string> old_page);

/**
 * @brief Translate single char to string of a braille ascii bytes.
 * 
 * @param chr Single character to convert to braille ascii.
 * @return string String of braille ascii byte from char.
 */
string char_to_braille(char chr);

/**
 * @brief Prints the current page and accentuates the current burst.
 * 
 * @param page_ind Current page index in book.
 * @param burst_ind Current burst index in page.
 * @param book_size Total pages in current book.
 * @param curr_page Current page.
 * @param curr_burst Current burst.
 */
void print_info(int page_ind, int burst_ind, int book_size, string curr_page, string curr_burst);

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
 * @brief Trims newline and line feed characters off string.
 * 
 * @param s Any string.
 * @return string Passed string with no newline or line feed characters.
 */
string trim(string s);

/* ========= SPI FUNCTIONS ========= */
/**
 * @brief Initializes the SPI module.
 */
void spi_init();

/**
 * @brief Transmits each character in the burst via SPI.
 * 
 * @param current_burst Any string BURST_LEN long.
 */
void write_burst(string burst_ascii);
