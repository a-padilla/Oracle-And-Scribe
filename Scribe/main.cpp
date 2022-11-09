#include <Arduino.h>
#include <vector>
#include <string>
using std::string;
using std::vector;
#include "scribe.h"

int prev_button = LOW;
int next_button = LOW;
int prev_line_button = LOW;
int next_line_button = LOW;
int prev_page_button = LOW;
int next_page_button = LOW;

long prev_last_debounce_time=0;
long next_last_debounce_time=0;
long prev_line_last_debounce_time=0;
long next_line_last_debounce_time=0;
long prev_page_last_debounce_time=0;
long next_page_last_debounce_time=0;
long debounce_delay=250;

vector<vector<string>> book;
vector<string> page;
uint8_t str_ind=0, line_ind=0, page_ind=0, setter;
char curr_char;
string curr_line;
vector<string> curr_page;

bool new_page=false;

BluetoothSerial SerialBT;

// functions
void poll(const int button_pin, long &lbt, int &button, char cl, char np);
char next_char();
char prev_char();
string next_line();
string prev_line();
vector<string> next_page();
vector<string> prev_page();
void set_led(uint8_t c);
void get_page();

void setup() {
  bt_setup(SerialBT);
  page.push_back("Bradley");
  page.push_back("Torie");
  page.push_back("Austin");
  book.push_back(page);

  curr_page = book[page_ind];
  curr_line = curr_page[line_ind];
  curr_char = curr_line[str_ind];
  
  //leds
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(last_line, OUTPUT);
  pinMode(end_line, OUTPUT);
  // buttons
  pinMode(prev_button_pin, INPUT);
  pinMode(next_button_pin, INPUT);
  pinMode(prev_line_button_pin, INPUT);
  pinMode(next_line_button_pin, INPUT);
  pinMode(prev_page_button_pin, INPUT);
  pinMode(next_page_button_pin, INPUT);
}

void loop() {
  // get state of  prev char button
  poll(prev_button_pin, prev_last_debounce_time, prev_button, 'c', 'p');
  
  // get state of next char button
  poll(next_button_pin, next_last_debounce_time, next_button, 'c', 'n');

  // get state of  prev line button
  poll(prev_line_button_pin, prev_line_last_debounce_time, prev_line_button, 'l', 'p');
  
  // get state of next line button
  poll(next_line_button_pin, next_line_last_debounce_time, next_line_button, 'l', 'n');

  // get state of  prev page button
  poll(prev_page_button_pin, prev_page_last_debounce_time, prev_page_button, 'p', 'p');
  
  // get state of next page button
  poll(next_page_button_pin, next_page_last_debounce_time, next_page_button, 'p', 'n');

  // conditions
  curr_line = curr_page[line_ind];
  curr_char = curr_line[str_ind];
  setter = decode(curr_char);
  set_led(setter);
}

void poll(const int button_pin, long &lbt, int &button, char cl, char np){
    button=digitalRead(button_pin);
    if(((long)millis()-lbt)>debounce_delay){
      // if button pressed, get next char
      if(button==HIGH){
        if(cl=='c'){
          if(np=='n'){
            curr_char=next_char();
          }else{
            curr_char=prev_char();
          }
        }else if(cl=='l'){
          if(np=='n'){
            curr_line=next_line();
          }else{
            curr_line=prev_line();
          }
        }else if(cl=='p'){
          if(np=='n'){
            curr_page=next_page();
          }else{
            curr_page=prev_page();
          }
        }
        lbt=millis();
      }
    }
}

char next_char(){
  if(str_ind==page[line_ind].length()-1){
    str_ind=0;
    return curr_page[line_ind][0];
  }else{
    str_ind++;
    return curr_page[line_ind][str_ind];
  }
}

char prev_char(){
  if(str_ind==0){
    str_ind=curr_page[line_ind].length()-1;
    return curr_page[line_ind][str_ind];
  }else{
    str_ind--;
    return curr_page[line_ind][str_ind];
  }
}

string next_line(){
  if(line_ind==curr_page.size()-1){
    line_ind=0;
    str_ind=0;
    return curr_page[line_ind];
  }else{
    line_ind++;
    str_ind=0;
    return curr_page[line_ind];
  }
}

string prev_line(){
  if(line_ind==0){
    line_ind=curr_page.size()-1;
    str_ind=0;
    return curr_page[line_ind];
  }else{
    line_ind--;
    str_ind=0;
    return curr_page[line_ind];
  }
}

vector<string> next_page(){
  if(page_ind==book.size()-1){
    get_page();
    return book[page_ind];
  }else{
    page_ind++;
    line_ind=0;
    str_ind=0;
    return book[page_ind];
  }
}

vector<string> prev_page(){
  if(page_ind==0){
    page_ind=0;
    line_ind=0;
    str_ind=0;
    return book[page_ind];
  }else{
    page_ind--;
    line_ind=0;
    str_ind=0;
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
    book.push_back(page);
    curr_page=book[page_ind];
    str_ind=0;
    line_ind=0;
    page_ind=book.size()-1;
  }
}

void set_led(uint8_t c){
  // conditions for beginning and end of page
  if(line_ind==curr_page.size()-1){
    digitalWrite(last_line, HIGH);
  }else{
    digitalWrite(last_line, LOW);
  }

  if(str_ind==curr_line.length()-1){
    digitalWrite(end_line, HIGH);
  }else{
    digitalWrite(end_line, LOW);
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