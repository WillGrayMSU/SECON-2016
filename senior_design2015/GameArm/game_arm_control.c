/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: game_arm_control.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Controller for the arm that grabs all the games
* and pulls them into the robot
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        11/5/2014               SECON 2015
*********************************************************************/

#include "game_arm_control.h"

///////////////////////////////////////////////
//
// Game arm config
//
///////////////////////////////////////////////

void game_arm_init() {
    // Slide forward
    game_arm_slide_forward();

    // Raise the arm
    game_arm_raise();

    // Slide it back
    game_arm_slide_back();
}

///////////////////////////////////////////////
//
// Game arm usage
//
///////////////////////////////////////////////

void game_arm_release() {
    #ifdef DEBUG_BUILD
    printf("Releasing Game\n");
    #endif

    // Slide forward
    game_arm_slide_forward();

    // Raise the arm
    game_arm_raise();

    // Slide it back
    game_arm_slide_back();
}

void game_arm_pull_simon() {
    #ifdef DEBUG_BUILD
    printf("Grabbing Simon\n");
    #endif

    // Slide forward
    game_arm_slide_forward();

    // Put the arm down
    game_arm_lower();

    // Grab the Simon
    game_arm_slide_back();

    // Release Simon, we don't need to hold it
    game_arm_release();
}

void game_arm_pull_rubiks() {
    #ifdef DEBUG_BUILD
    printf("Grabbing Rubiks\n");
    #endif

    // Slide forward
    game_arm_slide_forward();

    // Put the arm down
    game_arm_lower();

    // Shimmy it in
    game_arm_shimmy(100, 0, 5);

    // Let go
    game_arm_release();
}

void game_arm_pull_etch() {
    #ifdef DEBUG_BUILD
    printf("Grabbing Etch\n");
    #endif

    // Slide forward
    game_arm_slide_forward();

    // Put the arm down
    game_arm_lower();

    // Grab Etch
    game_arm_slide(70);

    // Let go
    game_arm_release();
}

void game_arm_hold_etch() {
    #ifdef DEBUG_BUILD
    printf("Holding Etch\n");
    #endif

    // Slide forward
    game_arm_slide_forward();

    // Put the arm down
    game_arm_lower();

    // Hold the etch
    game_arm_slide(70);
}

void game_arm_lower() {
    game_arm_pivot(0);
    DELAY_MS(ARM_WAIT);
}

void game_arm_raise() {
    game_arm_pivot(100);
    DELAY_MS(ARM_WAIT);
}

void game_arm_pivot(uint8_t u8_percentage) {
    uint16_t u16_newPivotPulse;
    uint16_t u16_pivotPulseDifference;

    u16_pivotPulseDifference = (u8_percentage/100.0) * ARM_POSITION_DIFFERENCE;

    u16_newPivotPulse = ARM_POSITION_DOWN + u16_pivotPulseDifference;

    turn_servo_by_pulse(ARM_POSITION, u16_newPivotPulse);
}

void game_arm_slide_forward() {
    game_arm_slide(100);
}

void game_arm_slide_back() {
    game_arm_slide(0);
}

void game_arm_slide_right(int8_t i8_percentage) {
    uint16_t u16_newRightPulse;
    uint16_t u16_rightPulseDifference;

    #ifdef DEBUG_BUILD
    printf("Sliding right arm to %i percent\n", i8_percentage);
    #endif

    if (i8_percentage > 100) {
        i8_percentage = 100;
    }
    if (i8_percentage < 0) {
        i8_percentage = 0;
    }

    u16_rightPulseDifference = (i8_percentage/100.0) * ARM_SLIDE_RIGHT_DIFFERENCE;

    u16_newRightPulse = ARM_SLIDE_RIGHT_BACK + u16_rightPulseDifference;

    turn_servo_by_pulse(ARM_SLIDE_RIGHT, u16_newRightPulse);
}

void game_arm_slide_left(int8_t i8_percentage) {
    uint16_t u16_newLeftPulse;
    uint16_t u16_leftPulseDifference;

    #ifdef DEBUG_BUILD
    printf("Sliding left arm to %i percent\n", i8_percentage);
    #endif

    if (i8_percentage > 100) {
        i8_percentage = 100;
    }
    if (i8_percentage < 0) {
        i8_percentage = 0;
    }

    u16_leftPulseDifference = (i8_percentage/100.0) * ARM_SLIDE_LEFT_DIFFERENCE;

    u16_newLeftPulse = ARM_SLIDE_LEFT_BACK - u16_leftPulseDifference;

    turn_servo_by_pulse(ARM_SLIDE_LEFT, u16_newLeftPulse);
}

void game_arm_slide(uint8_t u8_percentage) {
    #ifdef DEBUG_BUILD
    printf("Sliding the arm to %u percent\n", u8_percentage);
    #endif

    game_arm_slide_left(u8_percentage);
    game_arm_slide_right(u8_percentage);
    DELAY_MS(ARM_WAIT);
}

void game_arm_shimmy(int8_t i8_startPosition, int8_t i8_endPosition, int8_t i8_numPositions) {
    int8_t i;
    int8_t i8_step = (i8_startPosition - i8_endPosition)/i8_numPositions;

    #ifdef DEBUG_BUILD
    printf("Shimmying the arm %i positions from %i to %i with steps of %i\n", i8_numPositions, i8_startPosition, i8_endPosition, i8_step);
    #endif

    for(i = 0;i < i8_numPositions;i++) {
        if (i % 2 == 0) {
            game_arm_slide_left(i8_startPosition - (i8_step*i));
            game_arm_slide_right(i8_startPosition - (i8_step*(i + 1)));
        }
        else {
            game_arm_slide_left(i8_startPosition - (i8_step*(i + 1)));
            game_arm_slide_right(i8_startPosition - (i8_step*i));
        }
        DELAY_MS(ARM_WAIT/2);
    }
    game_arm_slide(i8_endPosition);
}
