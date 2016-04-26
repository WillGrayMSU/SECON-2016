#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "Utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_Motorshield();

Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS.getMotor(4);

void setup(){
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  // turn on motor
  myMotor->run(RELEASE);
}

void loop() {
 Serial.print("tick");

//Motor1
  Motor1->run(FORWARD);
  Motor1->setSpeed(150);  
  
  //Motor2
  Motor2->run(FORWARD);
  Motor2->setSpeed(150);  

  
  //Motor3
  Motor3->run(BACKWARD);
  Motor3->setSpeed(150);  

  
  //Motor4
  Motor4->run(BACKWARD);
  Motor4->setSpeed(150);  
  
  delay(10000);
  
  //Release
  Serial.print("tech");
  Motor1->run(RELEASE);
  
  Serial.print("tech");
  Motor2->run(RELEASE);
  
  Serial.print("tech");
  Motor3->run(RELEASE);
  
  Serial.print("tech");
  Motor4->run(RELEASE);
  delay(5000);
}
