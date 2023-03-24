#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#define BURST_LEN 5
volatile bool received;
volatile uint8_t receivedData;
uint8_t* data = new uint8_t[BURST_LEN*6];
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); // create an object of board 1
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); // create an object of board 2
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
  while(currentIndex != BURST_LEN*6){
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

void setBurst() {
  int servoValue = servoMin;
  int high = servoMin;
  int low = servoMax;
  
  for(int i=0; i<BURST_LEN*6; i++){
      //Invert high and low every 3 characters because the servos are reversed in direction
      if(i%3 == 0) {
        int temp = high;
        high = low;
        low = temp;
      }

      //Assign servoValue to the respective high or low angle depending on its value
      if (data[i] == '1') {
        servoValue = high;
      }
      else {
        servoValue = low;
      }
      
      //Decide on which pwm to use based on the position in the data array
      if(i/16 < 1)
        pwm1.setPWM(i, 0, servoValue);
      else
        pwm2.setPWM(i-16, 0, servoValue);
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
  setBurst();
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