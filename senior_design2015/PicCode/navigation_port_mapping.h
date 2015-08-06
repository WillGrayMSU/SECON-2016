/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName:   navigation_port_mapping.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC33EP512GP806
* Compiler: gcc-pic33
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Port definitions for the navigation PIC
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        2/18/2015               SECON 2015
*********************************************************************/

// Motor configs
#define LIN1_PULSE                      OC2RS                   // Pin D1
#define LIN2_PULSE                      OC3RS                   // Pin D2
#define RIN1_PULSE                      OC4RS                   // Pin D3
#define RIN2_PULSE                      OC5RS                   // Pin D4

#define RIGHT_ENCODER                   _RF6
#define LEFT_ENCODER                    _RD8

// Buttons for start/game order
#define START_BUTTON                    _RG15
#define SIMON_BUTTON                    _RG6
#define ETCH_BUTTON                     _RG7
#define CARD_BUTTON                     _RG8
#define RUBIKS_BUTTON                   _RG9

// Line 1
#define EMITTER_LINE1                   _RB8

#define SENSOR0_LINE1_IN                _RB0
#define SENSOR1_LINE1_IN                _RB1
#define SENSOR2_LINE1_IN                _RB2
#define SENSOR3_LINE1_IN                _RB3
#define SENSOR4_LINE1_IN                _RB4
#define SENSOR5_LINE1_IN                _RB5
#define SENSOR6_LINE1_IN                _RB6
#define SENSOR7_LINE1_IN                _RB7

#define SENSOR0_LINE1_OUT               _LATB0
#define SENSOR1_LINE1_OUT               _LATB1
#define SENSOR2_LINE1_OUT               _LATB2
#define SENSOR3_LINE1_OUT               _LATB3
#define SENSOR4_LINE1_OUT               _LATB4
#define SENSOR5_LINE1_OUT               _LATB5
#define SENSOR6_LINE1_OUT               _LATB6
#define SENSOR7_LINE1_OUT               _LATB7

// Line 2
#define EMITTER_LINE2                   _RF5

#define SENSOR0_LINE2_IN                _RB9
#define SENSOR1_LINE2_IN                _RF4
#define SENSOR2_LINE2_IN                _RB14
#define SENSOR3_LINE2_IN                _RB13
#define SENSOR4_LINE2_IN                _RB12
#define SENSOR5_LINE2_IN                _RB11
#define SENSOR6_LINE2_IN                _RB10
#define SENSOR7_LINE2_IN                _RC1

#define SENSOR0_LINE2_OUT               _LATF4
#define SENSOR1_LINE2_OUT               _LATB14
#define SENSOR2_LINE2_OUT               _LATB13
#define SENSOR3_LINE2_OUT               _LATB12
#define SENSOR4_LINE2_OUT               _LATB11
#define SENSOR5_LINE2_OUT               _LATB10
#define SENSOR6_LINE2_OUT               _LATB9
#define SENSOR7_LINE2_OUT               _LATC1

// Line 3
#define EMITTER_LINE3                   _RC15

#define SENSOR0_LINE3_IN                _RD7
#define SENSOR1_LINE3_IN                _RF0
#define SENSOR2_LINE3_IN                _RF1
#define SENSOR3_LINE3_IN                _RG1
#define SENSOR4_LINE3_IN                _RG0
#define SENSOR5_LINE3_IN                _RG14
#define SENSOR6_LINE3_IN                _RG12
#define SENSOR7_LINE3_IN                _RG13

#define SENSOR0_LINE3_OUT               _LATD7
#define SENSOR1_LINE3_OUT               _LATF0
#define SENSOR2_LINE3_OUT               _LATF1
#define SENSOR3_LINE3_OUT               _LATG1
#define SENSOR4_LINE3_OUT               _LATG0
#define SENSOR5_LINE3_OUT               _LATG14
#define SENSOR6_LINE3_OUT               _LATG12
#define SENSOR7_LINE3_OUT               _LATG13

// Line 4
#define EMITTER_LINE4                   _RC12

#define SENSOR0_LINE4_IN                _RD9
#define SENSOR1_LINE4_IN                _RD10
#define SENSOR2_LINE4_IN                _RD11
#define SENSOR3_LINE4_IN                _RD0
#define SENSOR4_LINE4_IN                _RC13
#define SENSOR5_LINE4_IN                _RC14
#define SENSOR6_LINE4_IN                _RD5
#define SENSOR7_LINE4_IN                _RD6

#define SENSOR0_LINE4_OUT               _LATD9
#define SENSOR1_LINE4_OUT               _LATD10
#define SENSOR2_LINE4_OUT               _LATD11
#define SENSOR3_LINE4_OUT               _LATD0
#define SENSOR4_LINE4_OUT               _LATC13
#define SENSOR5_LINE4_OUT               _LATC14
#define SENSOR6_LINE4_OUT               _LATD5
#define SENSOR7_LINE4_OUT               _LATD6

// Line 5
#define EMITTER_LINE5                   _RF6

#define SENSOR0_LINE5_IN                _RF5
#define SENSOR1_LINE5_IN                _RG9
#define SENSOR2_LINE5_IN                _RG8
#define SENSOR3_LINE5_IN                _RG7
#define SENSOR4_LINE5_IN                _RG6
#define SENSOR5_LINE5_IN                _RC2
#define SENSOR6_LINE5_IN                _RC1
#define SENSOR7_LINE5_IN                _RG15

#define SENSOR0_LINE5_OUT               _LATF5
#define SENSOR1_LINE5_OUT               _LATG9
#define SENSOR2_LINE5_OUT               _LATG8
#define SENSOR3_LINE5_OUT               _LATG7
#define SENSOR4_LINE5_OUT               _LATG6
#define SENSOR5_LINE5_OUT               _LATC2
#define SENSOR6_LINE5_OUT               _LATC1
#define SENSOR7_LINE5_OUT               _LATG15
