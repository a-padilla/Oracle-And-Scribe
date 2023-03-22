#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#define BURST_LEN 5
volatile bool received;
volatile uint8_t receivedData;
uint8_t* data = new uint8_t[BURST_LEN];
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); // create an object of board 1
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
  read_burst(data);

  // Read the input state of each digital pin and set the corresponding servo position
  for(int i=0; i<=5; i++){
    // Read the input state of the specified digital pin
    bool inputState = data[i];

    // Set the servo position based on the input state
    if (inputState) {
      if(i>2){
        pwm1.setPWM(i, 0, servoMax);
      }else{
        pwm1.setPWM(i, 0, servoMin);
      }
    } else {
      if(i>2){
        pwm1.setPWM(i, 0, servoMin);
      }else{
        pwm1.setPWM(i, 0, servoMax);
      }
    }
  }
}
