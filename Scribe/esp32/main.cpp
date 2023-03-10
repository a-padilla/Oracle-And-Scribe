#include <Arduino.h>
#include <vector>
#include <string>
using std::string;
using std::vector;
#include "scribe.h"

int prev_burst_button = LOW;
int next_burst_button = LOW;
int prev_page_button = LOW;
int next_page_button = LOW;

long prev_burst_last_debounce_time=0;
long next_burst_last_debounce_time=0;
long prev_page_last_debounce_time=0;
long next_page_last_debounce_time=0;
long debounce_delay=250;

vector<string> book;
vector<string> page;
int char_ind=0, burst_ind=0, page_ind=0;
uint8_t setter;
string curr_burst;
string curr_page;
bool new_page=false;

BluetoothSerial SerialBT;

/* ========== FUNCTIONS ========== */

/**
 * @brief 
 * 
 * @param button_pin Pin of ESP32 to which the button is connected.
 * @param lbt Last debounce time of button.
 * @param button State of the button (HIGH/LOW).
 * @param cl Tells whether the button is for a character, line, or page.
 * @param np Tells whether the butto is for the next or previous.
 */
void poll(const int button_pin, long &lbt, int &button, char cl, char np);

/**
 * @brief Update the char and burst index and get the next burst in a page.
 * 
 * @return string Next burst. 
 */
string next_burst();

/**
 * @brief Update the char and burst index and get the prev burst in a page.
 * 
 * @return string Previous burst.
 */
string prev_burst();

/**
 * @brief Updates the page index and returns the next page. If on the last page, then calls get_page.
 * 
 * @return string Next page.
 */
string next_page();

/**
 * @brief Updates the page index and returns the previous page.
 * 
 * @return string Previous page.
 */
string prev_page();

/**
 * @brief Set the LEDs with the current character.
 * 
 * @param c Current character.
 */
void set_led(uint8_t c);

/**
 * @brief Calls bt_loop. If a new page is loaded, put the new page in the book.
 * 
 */
void get_page();

/**
 * @brief Convert page from a vector of strings to one string.
 * 
 * @param old_page Page as vector of strings.
 * @return string Page as one string.
 */
string change_page_format(vector<string> old_page);

/**
 * @brief Initialize the ESP32: Hard-code initial book values, print initial line info, and set pin modes.
 * 
 */
void setup() {
  bt_setup(SerialBT);
  page.push_back("Bradley");
  page.push_back("Torie");
  page.push_back("Austin");
  book.push_back(change_page_format(page));

  page.clear();
  page.push_back("Hello");
  page.push_back("World");
  book.push_back(change_page_format(page));

  curr_page = book[page_ind];
  curr_burst = burst_from_page(curr_page, burst_ind);
  print_info(page_ind, burst_ind, (int)book.size(), (int)curr_page.size(), curr_page, curr_burst);
  
  //leds
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(last_burst_pin, OUTPUT);
  pinMode(last_page_pin, OUTPUT);
  // buttons
  pinMode(prev_burst_button_pin, INPUT);
  pinMode(next_burst_button_pin, INPUT);
  pinMode(prev_page_button_pin, INPUT);
  pinMode(next_page_button_pin, INPUT);
}


/**
 * @brief ESP32 loop: poll buttons, set the current line and character, and set the LEDs.
 * 
 */
void loop() {
  // get state of  prev char button
  poll(prev_burst_button_pin, prev_burst_last_debounce_time, prev_burst_button, 'b', 'p');
  
  // get state of next char button
  poll(next_burst_button_pin, next_burst_last_debounce_time, next_burst_button, 'b', 'n');

  // get state of  prev page button
  poll(prev_page_button_pin, prev_page_last_debounce_time, prev_page_button, 'p', 'p');
  
  // get state of next page button
  poll(next_page_button_pin, next_page_last_debounce_time, next_page_button, 'p', 'n');

  // conditions
  // curr_page = book[page_ind];
  curr_burst = burst_from_page(curr_page, burst_ind);
  if(curr_burst!=""){
    setter = decode(curr_burst[0]);
    set_led(setter);
  }
}

void poll(const int button_pin, long &lbt, int &button, char cl, char np){
    button=digitalRead(button_pin);
    if(((long)millis()-lbt)>debounce_delay){
      // if button pressed
      if(button==HIGH){
        if(cl=='b'){
          if(np=='n'){
            curr_burst=next_burst();
            print_info(page_ind, burst_ind, (int)book.size(), (int)curr_page.size(), curr_page, curr_burst);
          }else{
            curr_burst=prev_burst();
            print_info(page_ind, burst_ind, (int)book.size(), (int)curr_page.size(), curr_page, curr_burst);
          }
        }else if(cl=='p'){
          if(np=='n'){
            curr_page=next_page();
            curr_burst = burst_from_page(curr_page, burst_ind);
            print_info(page_ind, burst_ind, (int)book.size(), (int)curr_page.size(), curr_page, curr_burst);
          }else{
            curr_page=prev_page();
            curr_burst = burst_from_page(curr_page, burst_ind);
            print_info(page_ind, burst_ind, (int)book.size(), (int)curr_page.size(), curr_page, curr_burst);
          }
        }
        lbt=millis();
      }
    }
}

string next_burst(){
  if(!last_burst(curr_page, burst_ind)){
    char_ind += BURST_LEN;
    burst_ind++;
    return burst_from_page(curr_page, burst_ind);
  }else{
    char_ind=0;
    burst_ind=0;
    return burst_from_page(curr_page, burst_ind);
  }
}

string prev_burst(){
  if(burst_ind==0){
    char_ind=0;
    return curr_burst;
  }else{
    burst_ind--;
    char_ind-=BURST_LEN;
    return burst_from_page(curr_page, burst_ind);
  }
}

string next_page(){
  if(page_ind==book.size()-1){
    get_page();
    return book[page_ind];
  }else{
    page_ind++;
    char_ind=0;
    burst_ind=0;
    return book[page_ind];
  }
}

string prev_page(){
  if(page_ind==0){
    page_ind=0;
    char_ind=0;
    burst_ind=0;
    return book[page_ind];
  }else{
    page_ind--;
    char_ind=0;
    burst_ind=0;
    return book[page_ind];
  }
}

void get_page(){
  // bluetooth
  new_page=bt_loop(SerialBT, page);
  if(new_page){
    if(book.size()==MAX_PAGES){
      book.erase(book.begin());
    }
    book.push_back(change_page_format(page));
    char_ind=0;
    burst_ind=0;
    page_ind=book.size()-1;
  }
}

string change_page_format(vector<string> old_page){
  string _page;
  for(unsigned i=0; i<old_page.size(); i++){
    _page.append(old_page[i]);
    if(i!=old_page.size()-1)
      _page.append(" ");
  }
  return _page;
}

void set_led(uint8_t c){
  // conditions for beginning and end of page
  if(page_ind==book.size()-1){
    digitalWrite(last_page_pin, HIGH);
  }else{
    digitalWrite(last_page_pin, LOW);
  }

  if(last_burst(curr_page, burst_ind)){
    digitalWrite(last_burst_pin, HIGH);
  }else{
    digitalWrite(last_burst_pin, LOW);
  }

  // conditions for characters
  if((c & 0b1) == 1){
    digitalWrite(led5, HIGH);
  }else{
    digitalWrite(led5, LOW);
  }
  if(((c>>1) & 0b1) == 1){
    digitalWrite(led4, HIGH);
  }else{
    digitalWrite(led4, LOW);
  }
  if(((c>>2) & 0b1) == 1){
    digitalWrite(led3, HIGH);
  }else{
    digitalWrite(led3, LOW);
  }
  if(((c>>3) & 0b1) == 1){
    digitalWrite(led2, HIGH);
  }else{
    digitalWrite(led2, LOW);
  }
  if(((c>>4) & 0b1) == 1){
    digitalWrite(led1, HIGH);
  }else{
    digitalWrite(led1, LOW);
  }
  if(((c>>5) & 0b1) == 1){
    digitalWrite(led0, HIGH);
  }else{
    digitalWrite(led0, LOW);
  }
}