#include <SPI.h>                  // SPI protocol
#include <Wire.h>                 // I2C protocol
#include <Servo.h>
#include "Blockhandling_Functions.h"

byte command;        // Stores the current command sent from the Pi
byte last_command;   // Stores the previous command sent from the Pi


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

  arm.detach();
  gate.detach();
  
  Serial.println("SECON 2016 Top Test");
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
  "ALL FORWARD...",        // command: 1
  "ALL BACKWARD...",       // command: 2
  "ARM FORWARD...",        // command: 3
  "ARM BACKWARD...",       // command: 4
  "GATE UP...",            // command: 5
  "GATE DOWN...",          // command: 6
  "MOTOR 4 FORWARD...",    // command: 7
  "MOTOR 4 BACKWARD...",   // command: 8 
  "MOTOR 3 FORWARD...",    // command: 9
  "MOTOR 3 BACKWARD...",   // command: 10 
  "MOTOR 2 FORWARD..."     // command: 11
  "MOTOR 2 BACKWARD...",   // command: 12
  "MOTOR 1 FORWARD...",    // command: 13
  "MOTOR 1 BACKWARD...",   // command: 14
  "MOTOR 8 FORWARD...",    // command: 15
  "MOTOR 8 BACKWARD...",   // command: 16 
  "MOTOR 7 FORWARD...",    // command: 17
  "MOTOR 7 BACKWARD...",   // command: 18 
  "MOTOR 6 FORWARD..."     // command: 19
  "MOTOR 6 BACKWARD...",   // command: 20
  "MOTOR 5 FORWARD...",    // command: 21
  "MOTOR 5 BACKWARD...",   // command: 22
  "MOTOR 4 HALF FORWARD...",    // command: 23
  "MOTOR 4 HALF BACKWARD...",   // command: 24
  "MOTOR 3 HALF FORWARD...",    // command: 25
  "MOTOR 3 HALF BACKWARD...",   // command: 26 
  "MOTOR 2 HALF FORWARD..."     // command: 27
  "MOTOR 2 HALF BACKWARD...",   // command: 28
  "MOTOR 1 HALF FORWARD...",    // command: 229
  "MOTOR 1 HALF BACKWARD...",   // command: 30
  "MOTOR 8 HALF FORWARD...",    // command: 31
  "MOTOR 8 HALF BACKWARD...",   // command: 32 
  "MOTOR 7 HALF FORWARD...",    // command: 33
  "MOTOR 7 HALF BACKWARD...",   // command: 34 
  "MOTOR 6 HALF FORWARD..."     // command: 35
  "MOTOR 6 HALF BACKWARD...",   // command: 36
  "MOTOR 5 HALF FORWARD...",    // command: 37
  "MOTOR 5 HALF BACKWARD...",   // command: 38
  
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
  if((command != 0xFF) | (command != 0x00) && (command != last_command))
  { 
    switch(command)
    {
      case 1:
          allForward();
          delay(1200);
          allStop();
          break;
          
      case 2:
          allBackward();  // The speed is currently hard-coded at "100"
          delay(1200);
          allStop();
          break;
          
      case 3:
          arm.attach(7);
          armForward(); // Arm Forward
          arm.detach();
          break;
          
      case 4:
          arm.attach(7);
          armBackward();  // Arm Backward
          arm.detach();
          gate.detach();
          break;
          
      case 5:
          gate.attach(8);
          gateUp(); // Gate Up
          break;
          
      case 6:
          gate.attach(8);
          gateDown();  // Gate Down
          break;
          
      case 7:
          motorForward(1); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(1);
          break;
      case 8:
          motorBackward(1);
          delay(1200);
          motorStop(1);
          break;
      case 9:
          motorForward(2); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(2);
          break;
      case 10:
          motorBackward(2);
          delay(1200);
          motorStop(2);
          break;
      case 11:
          motorForward(3); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(3);
          break;
      case 12:
          motorBackward(3);
          delay(1200);
          motorStop(3); 
          break;
      case 13:
          motorForward(4); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(4);
          break;
      case 14:
          motorBackward(4);
          delay(1200);
          motorStop(4);
          break;
      case 15:
          motorForward(5); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(5);
          break;
      case 16:
          motorBackward(5);
          delay(1200);
          motorStop(5);
          break;
      case 17:
          motorForward(6); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(6);
          break;
      case 18:
          motorBackward(6);
          delay(1200);
          motorStop(6);
          break;
      case 19:
          motorForward(7); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(7);
          break;
      case 20:
          motorBackward(7);
          delay(1200);
          motorStop(7);
          break;
      case 21:
          motorForward(8); // The speed is currently hard-coded at "100"
          delay(1200);
          motorStop(8);
          break;
      case 22:
          motorBackward(8);
          delay(1200);
          motorStop(8);
          break;
          
      case 23:
          motorForward(1); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(1);
          break;
      case 24:
          motorBackward(1);
          delay(250);
          motorStop(1);
          break;
      case 25:
          motorForward(2); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(2);
          break;
      case 26:
          motorBackward(2);
          delay(250);
          motorStop(2);
          break;
      case 27:
          motorForward(3); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(3);
          break;
      case 28:
          motorBackward(3);
          delay(250);
          motorStop(3); 
          break;
      case 29:
          motorForward(4); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(4);
          break;
      case 30:
          motorBackward(4);
          delay(250);
          motorStop(4);
          break;
      case 31:
          motorForward(5); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(5);
          break;
      case 32:
          motorBackward(5);
          delay(250);
          motorStop(5);
          break;
      case 33:
          motorForward(6); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(6);
          break;
      case 34:
          motorBackward(6);
          delay(250);
          motorStop(6);
          break;
      case 35:
          motorForward(7); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(7);
          break;
      case 36:
          motorBackward(7);
          delay(250);
          motorStop(7);
          break;
      case 37:
          motorForward(8); // The speed is currently hard-coded at "100"
          delay(250);
          motorStop(8);
          break;
      case 38:
          motorBackward(8);
          delay(250);
          motorStop(8);
          break;
          
      case 39:
          allBackward();  // The speed is currently hard-coded at "100"
          delay(500);
          allStop();
          break;
          
      case 40:
          Serial.println("------------------");
          break;
    }
    
    // Prints the current state
    print_state();
    // Sets the last_command to equal the command that just executed
    last_command = command;
  }
  delay(100);  // This delay allows for the command to take effect (can be adjusted)
}  // end of the loop
