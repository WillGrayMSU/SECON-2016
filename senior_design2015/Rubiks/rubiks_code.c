/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: rubiks_code.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Controls the twisting of the rubiks cube
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/26/2014               SECON 2015
*********************************************************************/

#include "rubiks_code.h"

///////////////////////////////////////////////
//
// Rubiks Usage
//
///////////////////////////////////////////////

void rubik_init() {
    platform_up();
    twist_rubiks_counter();
}

void play_rubiks() {
    display_draw_number(RUBIKS_NUMBER);
    #ifdef DEBUG_BUILD
    printf("\n*** Spinng Rubiks ***\n");
    #endif

    // Pull in Rubiks
    game_arm_pull_rubiks();


    // Lower platform
    platform_rubiks();

    // Twist Rubiks
    twist_rubiks_clock();
    DELAY_MS(WAIT_TIME)

    // Reset platform
    platform_up();
    DELAY_MS(WAIT_TIME)
    twist_rubiks_counter();
}

///////////////////////////////////////////////
//
// Rubiks primitives
//
///////////////////////////////////////////////

void twist_rubiks_clock() {
    turn_servo_by_pulse(RUBIKS_TWIST, TWIST_CLOCK_PULSE);
}

void twist_rubiks_counter() {
    turn_servo_by_pulse(RUBIKS_TWIST, TWIST_COUNTER_PULSE);
}
