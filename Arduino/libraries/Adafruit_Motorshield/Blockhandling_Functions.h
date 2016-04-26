#include <Adafruit_MotorShield.h>
#include "Utility/Adafruit_PWMServoDriver.h"
#include <Wire.h>
#include <Servo.h>


Adafruit_MotorShield AFMS1 = Adafruit_MotorShield();
Adafruit_MotorShield AFMS2 = Adafruit_MotorShield(0x61);

Servo gate;
Servo arm;


Adafruit_DCMotor *Motor1 = AFMS1.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS1.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS1.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS1.getMotor(4);
Adafruit_DCMotor *Motor5 = AFMS2.getMotor(1);
Adafruit_DCMotor *Motor6 = AFMS2.getMotor(2);
Adafruit_DCMotor *Motor7 = AFMS2.getMotor(3);
Adafruit_DCMotor *Motor8 = AFMS2.getMotor(4);


int angle = 0;
int pos = 0;
int speed = 80;

void allForward(void){
  Motor1->run(FORWARD);
  Motor1->setSpeed(speed);
  Motor2->run(FORWARD);
  Motor2->setSpeed(speed);
  Motor3->run(FORWARD);
  Motor3->setSpeed(speed);
  Motor4->run(FORWARD);
  Motor4->setSpeed(speed);
  Motor5->run(FORWARD);
  Motor5->setSpeed(speed);
  Motor6->run(FORWARD);
  Motor6->setSpeed(speed);
  Motor7->run(FORWARD);
  Motor7->setSpeed(speed);
  Motor8->run(FORWARD);
  Motor8->setSpeed(speed);
}

void allBackward(void){
  Motor1->run(BACKWARD);
  Motor1->setSpeed(speed);
  Motor2->run(BACKWARD);
  Motor2->setSpeed(speed);
  Motor3->run(BACKWARD);
  Motor3->setSpeed(speed);
  Motor4->run(BACKWARD);
  Motor4->setSpeed(speed);
  Motor5->run(BACKWARD);
  Motor5->setSpeed(speed);
  Motor6->run(BACKWARD);
  Motor6->setSpeed(speed);
  Motor7->run(BACKWARD);
  Motor7->setSpeed(speed);
  Motor8->run(BACKWARD);
  Motor8->setSpeed(speed);
}

void motorForward(int motor){
  if (motor == 1){
    Motor1->run(FORWARD);
    Motor1->setSpeed(speed);
  }
  else if (motor == 2){
    Motor2->run(FORWARD);
    Motor2->setSpeed(speed);
  } 
  else if (motor == 3){
    Motor3->run(FORWARD);
    Motor3->setSpeed(speed);
  } 
  else if (motor == 4){
    Motor4->run(FORWARD);
    Motor4->setSpeed(speed);
  } 
  else if (motor == 5){
    Motor5->run(FORWARD);
    Motor5->setSpeed(speed);
  } 
  else if (motor == 6){
    Motor6->run(FORWARD);
    Motor6->setSpeed(speed);
  } 
  else if (motor == 7){
    Motor7->run(FORWARD);
    Motor7->setSpeed(speed);
  } 
  else if (motor == 8){
    Motor8->run(FORWARD);
    Motor8->setSpeed(speed);
  } }
  
 void motorBackward(int motor){
  if (motor == 1){
    Motor1->run(BACKWARD);
    Motor1->setSpeed(speed);
  }
  else if (motor == 2){
    Motor2->run(BACKWARD);
    Motor2->setSpeed(speed);
  } 
  else if (motor == 3){
    Motor3->run(BACKWARD);
    Motor3->setSpeed(speed);
  } 
  else if (motor == 4){
    Motor4->run(BACKWARD);
    Motor4->setSpeed(speed);
  } 
  else if (motor == 5){
    Motor5->run(BACKWARD);
    Motor5->setSpeed(speed);
  } 
  else if (motor == 6){
    Motor6->run(BACKWARD);
    Motor6->setSpeed(speed);
  } 
  else if (motor == 7){
    Motor7->run(BACKWARD);
    Motor7->setSpeed(speed);
  } 
  else if (motor == 8){
    Motor8->run(BACKWARD);
    Motor8->setSpeed(speed);
  } }

void allStop(void){
 Motor1->run(RELEASE);
 Motor2->run(RELEASE);
 Motor3->run(RELEASE);
 Motor4->run(RELEASE);
 Motor5->run(RELEASE);
 Motor6->run(RELEASE);
 Motor7->run(RELEASE);
 Motor8->run(RELEASE);}

void motorStop(int motor){
  if (motor == 1){
    Motor1->run(RELEASE);
  }
  else if (motor == 2){
    Motor2->run(RELEASE);
  } 
  else if (motor == 3){
    Motor3->run(RELEASE);
  } 
  else if (motor == 4){
    Motor4->run(RELEASE);
  } 
  else if (motor == 5){
    Motor5->run(RELEASE);
  } 
  else if (motor == 6){
    Motor6->run(RELEASE);
  } 
  else if (motor == 7){
    Motor7->run(RELEASE);
  } 
  else if (motor == 8){
    Motor8->run(RELEASE);
  } }


void armForward(void){
	for(angle=0; angle<75; angle += 1){	
	     arm.writeMicroseconds(1300);
	     delay(20);
	}
  }

void armBackward(void){
	for(angle=75;angle>=1; angle-=1){
	      arm.writeMicroseconds(1700);
	      delay(20);
	}
  }

void gateUp(void){
	for(pos=0; pos<90; pos++){
	    gate.write(90);
	    delay(20);
  }
}


void gateDown(void){
	for(pos=90;pos>0; pos--){
 		gate.write(180);
      	delay(20);
	}
}
