#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <string>
#define BURST_LEN 5
volatile bool received;
volatile uint8_t receivedData;
uint8_t* data = new uint8_t[BURST_LEN*6];
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); // create an object of board 1
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); // create an object of board 2
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x42); // create an object of board 3
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x43); // create an object of board 4
int servoMin = 204;   // 0 degrees
int servoMax = 250;   // ~3.22 degrees
int servoFrequency = 50; // servo update frequency, analog servos typically run at ~50 Hz

void spi_init() {
  delay(1500); //Delay to allow opening serial monitor at beginning
  Serial.begin(115200); //Sets up serial port so we can print debug
  pinMode(MISO, OUTPUT); //MISO as an output, everything else is input by default
  SPCR |= _BV(SPE); //Configures the Arduino as a slave
  received = false; //Initializes recieved flag as false
  SPI.attachInterrupt(); //Enables interrupt
}

void read_burst(uint8_t* data_arr) {
  int currentIndex = 0;
  
  //Main loop polls the ISR flag
  while(currentIndex != BURST_LEN){
    if(received) {
      //Updates flag and prints the recieved data
      received = false;
      data_arr[currentIndex] = receivedData;
      currentIndex++;
    }
  }
}

//Interrupt Service Routine for when SPI data is recieved
ISR (SPI_STC_vect)
{
  //Stores the data and updates the flag
  receivedData = SPDR;
  received = true;
}

void setChar(int characterNumber, string data) {
  for(int i=0; i<=5; i++){
      // Read the input state of the specified digital pin
      bool inputState = data[i];
      int servoValue = 0;

      // Set the servo position based on the input state
      if (inputState == '1') {
        if(i>2){
          servoValue = servoMax;
        }else{
          servoValue = servoMin;
        }
      } else {
        if(i>2){
          servoValue = servoMin;
        }else{
          servoValue = servoMax;
        }
      }

      //I know this is ugly and I'm sorry
      if(characterNumber == 1){
        pwm1.setPWM(i, 0, servoValue);
      }
      else if(characterNumber == 2){
        pwm1.setPWM(i+6, 0, servoValue);
      }
      else if(characterNumber == 3){
        pwm2.setPWM(i, 0, servoValue);
      }
      else if(characterNumber == 4){
        pwm2.setPWM(i+6, 0, servoValue);
      }
      else if(characterNumber == 5){
        pwm3.setPWM(i, 0, servoValue);
      }
      else if(characterNumber == 6){
        pwm3.setPWM(i+6, 0, servoValue);
      }
      else if(characterNumber == 7){
        pwm4.setPWM(i, 0, servoValue);
      }
      else if(characterNumber == 8){
        pwm4.setPWM(i+6, 0, servoValue);
      }
    }
}

void setup() {
  // put your setup code here, to run once:
  spi_init();
  pwm1.begin(); //start board(s)
  pwm1.setPWMFreq(servoFrequency); // set the servo operating freq

  // set pins 2-7 as inputs
  for(int i=2; i<=7; i++){
    pinMode(i, INPUT);
  }
}

void loop() {
  //Reads the SPI burst of data and stores it
  read_burst(data);

  //Sets each cell to the respective character
  for(int i = 0; i < BURST_LEN; i++) {
    string characterData = ""; 
    
    //Creates a string of 6 characters corresponding to the current character
    for(int j = 0; j < 6; j++) {
      characterData = characterData + (char)data[i*6 + j]
    }

    //Sets the character to display on the servos
    setChar(i, characterData);
  }
  // Read the input state of each digital pin and set the corresponding servo position
}





//Thinking
  //For burst_len of 5, 2 characters per board, we will use 3 boards
  //First 2 characters are pwm1, next 2 are pwm2, etc
  //Likely means we will have an if to check what to do based on i
  //Maybe can generalize a function for the inner for loop which would allow something like
  /*
  setChar1
  setChar2
  setChar3
  ...
  */
  //This is a bit silly though because it requires manual expansion rather than paramaterizing it.
  //The issue with paramaterizing it is that each uses a different pwm, eg. pwm1 vs pwm2 ...
  //Maybe can do something with interfaces?
  //Or more easily just make a setChar(int deviceNumber, string data) function
    //This should have a first check to see whether j > 2 and assign 
      //int servoValue = servoMax/servoMin;
    //Then, use servoValue in an if else chain to simplify the pwm1, pwm2, etc.