/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: secon_robot_configuration.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: All the configuration options for the robot are
* in this file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        3/5/2014                SECON 2015
*********************************************************************/

#ifndef SECON_ROBOT_CONFIGURATION_H_
#define SECON_ROBOT_CONFIGURATION_H_

// Configurations
#define STATIC_ORDER                        1       // Game order configuration
#define SKIP_START_LIGHT                    0       // Skip the start light
#define SKIP_START_BUTTON                   0       // Skip the start button
#define SKIP_STATIC_COURSE_SELECTION        1       // Skip static course selection
#define SKIP_STATIC_TURN_LAYOUT_SELECTION   0       // Skip static turn layout selection
#define SKIP_BRANCH_LIST_SETUP              0       // Skip branch selection
#define SKIP_SPEED_CONFIGURATION            0       // Skip speed adjustment
#define SKIP_RECENTER_CONFIGURATION         0       // Skip the selection for recentering
// Skip various games
#define SKIP_SIMON                      0
#define SKIP_ETCH                       0
#define SKIP_RUBIKS                     0
#define SKIP_CARD                       0

// I2C Messages
char sz_playSimonString[BUFFSIZE] =     "Simon";
char sz_playRubiksString[BUFFSIZE] =    "Rubik";
char sz_playCardsString[BUFFSIZE] =     "Cards";
char sz_playEtchString[BUFFSIZE] =      "Etch.";
char sz_idleString[BUFFSIZE] =          "Idle.";
char sz_waitString[BUFFSIZE] =          "Wait.";
char sz_confString[BUFFSIZE] =          "Conf.";

#endif
