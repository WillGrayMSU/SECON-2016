/*
 * Basic navigation command functions
 */ 

#include <Adafruit_MotorShield.h>
#include "Utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS.getMotor(4);


void move_forward(int speed){
	
    //FORWARD MOVEMENT at a constant speed 

	//Motor1
	Motor1->run(FORWARD);
	Motor1->setSpeed(speed);
	
	//Motor2
	Motor2->run(FORWARD);
	Motor2->setSpeed(speed);
	
	//Motor3
	Motor3->run(BACKWARD);
	Motor3->setSpeed(speed);
	
	//Motor4
	Motor4->run(BACKWARD);
	Motor4->setSpeed(speed);
}

void stop_movement(){
	
	//Release
	//Serial.print("tech");
	Motor1->run(RELEASE);
	
	//Serial.print("tech");
	Motor2->run(RELEASE);
	
	//Serial.print("tech");
	Motor3->run(RELEASE);
	
	//Serial.print("tech");
	Motor4->run(RELEASE);
}

void move_backward(int speed){
	
	//BACKWARD MOVEMENT at a constant speed
	//Motor1
	Motor1->run(BACKWARD);
	Motor1->setSpeed(speed);
	
	//Motor2
	Motor2->run(BACKWARD);
	Motor2->setSpeed(speed);
	
	//Motor3
	Motor3->run(FORWARD);
	Motor3->setSpeed(speed);
	
	//Motor4
	Motor4->run(FORWARD);
	Motor4->setSpeed(speed);
}

void strafe_right(int speed){
	
	//STRAFING RIGHT MOVEMENT at a constant speed
	//Motor1
	Motor1->run(BACKWARD);
	Motor1->setSpeed(speed);
	
	//Motor2
	Motor2->run(FORWARD);
	Motor2->setSpeed(speed);
	
	//Motor3
	Motor3->run(FORWARD);
	Motor3->setSpeed(speed);
	
	//Motor4
	Motor4->run(BACKWARD);
	Motor4->setSpeed(speed);
}

void strafe_left(int speed){
	
	//STRAFING LEFT MOVEMENT at a constant speed
	//Motor1
	Motor1->run(FORWARD);
	Motor1->setSpeed(speed);
	
	//Motor2
	Motor2->run(BACKWARD);
	Motor2->setSpeed(speed);
	
	//Motor3
	Motor3->run(BACKWARD);
	Motor3->setSpeed(speed);
	
	//Motor4
	Motor4->run(FORWARD);
	Motor4->setSpeed(speed);
}

void rotate_right(int speed){   
	
	//ROTATE RIGHT
	//Motor1
	Motor1->run(FORWARD);
	Motor1->setSpeed(speed);
	
	//Motor2
	Motor2->run(FORWARD);
	Motor2->setSpeed(speed);
	
	//Motor3
	Motor3->run(FORWARD);
	Motor3->setSpeed(speed);
	
	//Motor4
	Motor4->run(FORWARD);
	Motor4->setSpeed(speed);
}

void rotate_left(int speed){
	
	//ROTATE LEFT
	//Motor1
	Motor1->run(BACKWARD);
	Motor1->setSpeed(speed);
	
	//Motor2
	Motor2->run(BACKWARD);
	Motor2->setSpeed(speed);
	
	//Motor3
	Motor3->run(BACKWARD);
	Motor3->setSpeed(speed);
	
	//Motor4
	Motor4->run(BACKWARD);
	Motor4->setSpeed(speed);
}
