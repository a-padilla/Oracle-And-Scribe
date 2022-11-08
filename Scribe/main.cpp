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

long prev_last_debounce_time=0;
long next_last_debounce_time=0;
long prev_line_last_debounce_time=0;
long next_line_last_debounce_time=0;
long debounce_delay=250;

vector<string> lines;
// char** lines = (char**)malloc(MAX_LINES*sizeof(char*));

uint8_t str_ind=0, line_ind=0, setter;

char curr_char;
string curr_line;
// char* curr_line = (char*)malloc(MAX_CHAR*sizeof(char));

// functions
void poll(const int button_pin, long &lbt, int &button, char cl, char np);
char next_char();
char prev_char();
string next_line();
string prev_line();
// char* next_line();
// char* prev_line();
void set_led(uint8_t c);
int num_lines();

void setup() {
  // memset(lines, 0, MAX_LINES*sizeof(char*));

  lines.push_back("Bradley");
  lines.push_back("Torie");
  lines.push_back("Austin");
  // lines[0] = (char*)"Bradley";
  // lines[1] = (char*)"Johnson";
  // lines[2] = (char*)"Austin";

  curr_char = lines[line_ind][str_ind];
  curr_line = lines[line_ind];
  
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

  // conditions
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
        }else{
          if(np=='n'){
            curr_line=next_line();
          }else{
            curr_line=prev_line();
          }
        }
        lbt=millis();
      }
    }
}

char next_char(){
  // if(str_ind==strlen(lines[line_ind])-1){
  if(str_ind==lines[line_ind].length()-1){
    str_ind=0;
    return lines[line_ind][0];
  }else{
    str_ind++;
    return lines[line_ind][str_ind];
  }
}

char prev_char(){
  if(str_ind==0){
    // str_ind=strlen(lines[line_ind])-1;
    str_ind=lines[line_ind].length()-1;
    return lines[line_ind][str_ind];
  }else{
    str_ind--;
    return lines[line_ind][str_ind];
  }
}

// char* next_line(){
string next_line(){
  if(line_ind==num_lines()-1){
    line_ind=0;
    str_ind=0;
    return lines[line_ind];
  }else{
    line_ind++;
    str_ind=0;
    return lines[line_ind];
  }
}

// char* prev_line(){
string prev_line(){
  if(line_ind==0){
    line_ind=num_lines()-1;
    str_ind=0;
    return lines[line_ind];
  }else{
    line_ind--;
    str_ind=0;
    return lines[line_ind];
  }
}

void set_led(uint8_t c){
  // conditions for beginning and end of lines
  if(line_ind==num_lines()-1){
    digitalWrite(last_line, HIGH);
  }else{
    digitalWrite(last_line, LOW);
  }

  // if(str_ind==strlen(curr_line)-1){
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

int num_lines(){
  // int count=0;
  // for(unsigned i=0; i<MAX_LINES; i++){
  //   if(lines[i] == 0){
  //     break;
  //   }else{
  //     count++;
  //   }
  // }
  // return count;
  return lines.size();
}