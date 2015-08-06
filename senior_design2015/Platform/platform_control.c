/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: platform_control.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to control the platform that holds the
* rubiks and etch playing mechanisms.
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        10/13/2014              SECON 2015
*********************************************************************/

#include "platform_control.h"

///////////////////////////////////////////////
//
// Platform config
//
///////////////////////////////////////////////

void platform_init() {
    platform_up();
    turn_servo_by_pulse(RUBIKS_TWIST, TWIST_INITIAL_PULSE);
}

///////////////////////////////////////////////
//
// Platform usage
//
///////////////////////////////////////////////

void platform_up() {
    turn_servo_by_pulse(RUBIKS_PLATFORM, PLATFORM_UP_PULSE);
}

void platform_down() {
    turn_servo_by_pulse(RUBIKS_PLATFORM, PLATFORM_RUBIK_PULSE);
}

void platform_rubiks() {
    turn_servo_by_pulse(RUBIKS_PLATFORM, PLATFORM_RUBIK_PULSE);
    DELAY_MS(PLATFORM_WAIT);
}

void platform_etch() {
    turn_servo_by_pulse(RUBIKS_TWIST,TWIST_DIAG_PULSE);
    turn_servo_by_pulse(RUBIKS_PLATFORM, PLATFORM_ETCH_PULSE);
    DELAY_MS(PLATFORM_WAIT);
}

void platform_etch_undo() {
    platform_up();
    DELAY_MS(200);
    turn_servo_by_pulse(RUBIKS_TWIST, TWIST_INITIAL_PULSE);
}
