/****************************************************************
 SECON_Navigation.ino
 
 This is the lowest level motor control code for the SECON 2016 
 navigation system
 
 References:
   Written by Nick Gammon
   
   February 2011
****************************************************************/
#include <SPI.h>                  // SPI protocol
#include <Wire.h>                 // I2C protocol
#include "Navigation_Library.h"   // Navigation functions
#include <Servo.h>

byte command;        // Stores the current command sent from the Pi
byte last_command;   // Stores the previous command sent from the Pi

int speed;
int direction;

void setup (void)
{
  Serial.begin (115200);   // debugging
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);

  // Data is sent on Master In, Slave Out
  pinMode(MISO, OUTPUT);

  // Now turn on interrupts
  SPI.attachInterrupt();
  
  // Create with the default frequency 1.6KHz
  AFMS1.begin();  
  AFMS2.begin();
  AFMS3.begin();  
  
  Serial.println("SECON 2016 Drive Test");
  delay(3000);  // Wait a few seconds for setup to complete
}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  command = SPDR;  // grab byte from SPI Data Register
  //Serial.println(command);
  
  delay(1500);
}  // end of interrupt routine SPI_STC_vect


// Char array that stores each state name for printing purposes
const char* apsz_state_names[] = {
  "STOPPED...",           // command: 1
  "MOVING FORWARD...",    // command: 2
  "MOVING BACKWARD...",   // command: 3
  "MOVING LEFT...",       // command: 4 
  "MOVING RIGHT...",      // command: 5
  "ROTATING LEFT...",     // command: 6 
  "ROTATING RIGHT..."     // command: 7
};


// Prints the current state of the program
void print_state()
{
  if(command != last_command)
  {
    Serial.println(apsz_state_names[command-1]);
  }
}


// Main loop of the program
void loop (void)
{ 
  // Checks that command is not (0 or 0xFF) and not the same as last_command
  if((command != 0xFF) | (command != 0x00) && command != last_command)
  { 
    speed = (command << 2) & 0x7F;
    if(command ){
      direction = command >> 5;
    }
    switch((direction) & 0x07)
    {
      case 1:
          if(command == 46)
            stop_movement();
          else if(command == 50)
            raise_platform(6.2);
          else if(command == 54)
            raise_platform(9.4);
          else if(command == 58)
            lower_platform(6.2);
          else if(command == 62)
            lower_platform(9.4);
          break;
          
      case 2:
          move_forward(speed);  
          break;
          
      case 3:
          move_backward(speed); 
          break;
          
      case 4:
          strafe_left(speed);  
          break;
          
      case 5:
          strafe_right(speed); 
          break;
          
      case 6:
          rotate_left(speed);  
          break;
          
      case 7:
          rotate_right(speed); 
          break;
          
      case 8:
          raise_platform(speed); 
          break;
          
      case 9:
          lower_platform(speed);
          break;  
  }
    // Prints the current state
    print_state();
    
    // Sets the last_command to equal the command that just executed
    last_command = command;
  }
  delay(100);  // This delay allows for the command to take effect (can be adjusted)
}  // end of the loop
