#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define BURST_LEN 5

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); // create an object of board 1
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); // create an object of board 2

int servoMin = 204, servoMax = 250, servoFrequency = 50;
const int ss_pin=13, mosi_pin=12, miso_pin = 11, sck_pin=10;

char burst[BURST_LEN*6+1];

void spi_init() {
  pinMode(ss_pin, INPUT);
  pinMode(mosi_pin, INPUT);
  pinMode(sck_pin, INPUT);
  pinMode(miso_pin, OUTPUT);
  digitalWrite(miso_pin, LOW);
}

void read_burst() {
  digitalWrite(miso_pin, HIGH);
  int i=0;
  while(digitalRead(ss_pin)==LOW && i < BURST_LEN*6){
    if(digitalRead(sck_pin) == HIGH){
      digitalWrite(miso_pin, LOW);
      (bool)digitalRead(mosi_pin) ? burst[i]='1' : burst[i]='0';
      i++;
      digitalWrite(miso_pin, HIGH);
      while(digitalRead(sck_pin) == HIGH);
    }  
  }
  digitalWrite(miso_pin, LOW);
  burst[BURST_LEN*6]='\0';
}

void set_burst(){
  for(int i=0; i<BURST_LEN*6; i++){
    // Read the input state of the specified digital pin
    bool inputState = burst[i]=='1';

    // Set the servo position based on the input state
    if (inputState) {
      if(i%6>2){
        if(i/16<1)
          pwm1.setPWM(i, 0, servoMax);
        else
          pwm2.setPWM(i-16, 0, servoMax);
      }else{
        if(i/16<1)
          pwm1.setPWM(i, 0, servoMin);
        else
          pwm2.setPWM(i-16, 0, servoMin);
      }
    } else {
      if(i%6>2){
        if(i/16<1)
          pwm1.setPWM(i, 0, servoMin);
        else
          pwm2.setPWM(i-16, 0, servoMin);
      }else{
        if(i/16<1)
          pwm1.setPWM(i, 0, servoMax);
        else
          pwm2.setPWM(i-16, 0, servoMax);  
      }
    }
  }
}

void setup() {
  // spi
  spi_init();

  // pca9685s
  pwm1.begin(); //start board(s)
  pwm1.setPWMFreq(servoFrequency); // set the servo operating freq
  pwm2.begin();
  pwm2.setPWMFreq(servoFrequency);
}

void loop() {
  if(digitalRead(ss_pin)==LOW){
    read_burst();
    digitalWrite(ss_pin, HIGH);
    set_burst();
  }
}
