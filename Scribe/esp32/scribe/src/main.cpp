#include "scribe.h"

// initializations for buttons
int prev_burst_button=LOW, next_burst_button=LOW, prev_page_button=LOW, next_page_button=LOW;
long prev_burst_last_debounce_time=0, next_burst_last_debounce_time=0;
long prev_page_last_debounce_time=0, next_page_last_debounce_time=0;

vector<string> book, page;
int burst_ind=0, page_ind=0;
uint8_t setter;
string curr_burst, curr_page;
BluetoothSerial SerialBT;

/**
 * @brief Initialize the ESP32: Hard-code initial book values, print initial location info, and set pin modes.
 * 
 */
void setup() {
  bt_setup(SerialBT);

  book.push_back("Bradley Torie Austin");
  book.push_back("Hello World");
  curr_page = book[page_ind];
  curr_burst = burst_from_page(curr_page, burst_ind);
  print_info(page_ind, burst_ind, (int)book.size(), curr_page, curr_burst);
  
  // buttons
  pinMode(prev_burst_button_pin, INPUT);
  pinMode(next_burst_button_pin, INPUT);
  pinMode(prev_page_button_pin, INPUT);
  pinMode(next_page_button_pin, INPUT);

  // spi to arduino
  spi_init();
}

/**
 * @brief ESP32 loop: poll buttons, set the current burst and the LEDs.
 * 
 */
void loop() {
  poll(prev_burst_button_pin, prev_burst_last_debounce_time, prev_burst_button, 'b', 'p', SerialBT, page, book, curr_page, curr_burst, page_ind, burst_ind);
  poll(next_burst_button_pin, next_burst_last_debounce_time, next_burst_button, 'b', 'n', SerialBT, page, book, curr_page, curr_burst, page_ind, burst_ind);
  poll(prev_page_button_pin, prev_page_last_debounce_time, prev_page_button, 'p', 'p', SerialBT, page, book, curr_page, curr_burst, page_ind, burst_ind);
  poll(next_page_button_pin, next_page_last_debounce_time, next_page_button, 'p', 'n', SerialBT, page, book, curr_page, curr_burst, page_ind, burst_ind);

  curr_burst = burst_from_page(curr_page, burst_ind);
  write_burst(curr_burst);
}
