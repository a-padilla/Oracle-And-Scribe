#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); // create an object of board 1

int servoMin = 204;   // 0 degrees
int servoMax = 409;   // 90 degrees
int servoFrequency = 50; // servo update frequency, analog servos typically run at ~50 Hz

void setup() {
  // put your setup code here, to run once:
  pwm1.begin(); //start board(s)
  pwm1.setPWMFreq(servoFrequency); // set the servo operating freq


  // set pins 2-7 as inputs
  for(int i=2; i<=7; i++){
    pinMode(i, INPUT);
  }
}

void loop() {
  // Read the input state of each digital pin and set the corresponding servo position
  for(int i=0; i<=5; i++){
    // Read the input state of the specified digital pin
    bool inputState = digitalRead((i+2));

    // Set the servo position based on the input state
    if (inputState) {
      pwm1.setPWM(i, 0, servoMax);
    } else {
      pwm1.setPWM(i, 0, servoMin);
    }
  }
}