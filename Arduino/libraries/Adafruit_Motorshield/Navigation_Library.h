/*
 * Basic navigation command functions
 */ 

#include <Adafruit_MotorShield.h>
#include "Utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS1 = Adafruit_MotorShield(0x60);
Adafruit_MotorShield AFMS2 = Adafruit_MotorShield(0x61);
Adafruit_MotorShield AFMS3 = Adafruit_MotorShield(0x62);


const int stepsPerRevolution = 200; //# of steps per rev for Stepper Motors

Adafruit_DCMotor *Motor1 = AFMS1.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS1.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS1.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS1.getMotor(4);

Adafruit_StepperMotor *Stepper1 = AFMS2.getStepper(stepsPerRevolution, 1);
Adafruit_StepperMotor *Stepper2 = AFMS3.getStepper(stepsPerRevolution, 1);
Adafruit_StepperMotor *Stepper3 = AFMS3.getStepper(stepsPerRevolution, 2);


int angle = 0;

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
	Motor3->setSpeed(speed+3);
	
	//Motor4
	Motor4->run(BACKWARD);
	Motor4->setSpeed(speed+3);
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
	Motor3->setSpeed(speed+3);
	
	//Motor4
	Motor4->run(FORWARD);
	Motor4->setSpeed(speed+3);
}

void strafe_right(int speed){
	
	//STRAFING RIGHT MOVEMENT at a constant speed
	//Motor1(Front Right)
	Motor1->run(BACKWARD);
	Motor1->setSpeed(speed);
	
	//Motor2(Back Right)
	Motor2->run(FORWARD);
	Motor2->setSpeed(speed);
	
	//Motor3(Back Left)
	Motor3->run(FORWARD);
	Motor3->setSpeed(speed);
	
	//Motor4(Front Left)
	Motor4->run(BACKWARD);
	Motor4->setSpeed(speed);
}

void strafe_left(int speed){
	
	//STRAFING LEFT MOVEMENT at a constant speed
	//Motor1(Front Right)
	Motor1->run(FORWARD);
	Motor1->setSpeed(speed);
	
	//Motor2(Back Right)
	Motor2->run(BACKWARD);
	Motor2->setSpeed(speed);
	
	//Motor3(Back Left)
	Motor3->run(BACKWARD);
	Motor3->setSpeed(speed);
	
	//Motor4(Front Left)
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

void raise_platform(int height){
	Stepper1->setSpeed(100000);
	Stepper2->setSpeed(100000);
	Stepper3->setSpeed(100000);
	
	for(int s = 0; s < height*stepsPerRevolution; s++)
	{
		Stepper1->step(1, FORWARD, SINGLE);
		Stepper2->step(1, FORWARD, SINGLE);
		Stepper3->step(1, FORWARD, SINGLE);
	}
	Stepper1->release();
	Stepper2->release();
	Stepper3->release();
}

void lower_platform(int height){
	Stepper1->setSpeed(100000);
	Stepper2->setSpeed(100000);
	Stepper3->setSpeed(100000);

	for(int s = 0; s < height*stepsPerRevolution; s++)
	{
		Stepper1->step(1, BACKWARD, SINGLE);
		Stepper2->step(1, BACKWARD, SINGLE);
		Stepper3->step(1, BACKWARD, SINGLE);
	}
	Stepper1->release();
	Stepper2->release();
	Stepper3->release();
}


byte convert_command(char command_char){

	byte command = 0;

	if(command_char == 'w')
		command = 2;
	else if(command_char == 's')
		command = 3;
	else if(command_char == 'a')
		command = 4;
	else if(command_char == 'd')
		command = 5;
	else if(command_char == 'q')
		command = 6;
	else if(command_char == 'e')
		command = 7;
	else
		command = 0;

	return command;
}
