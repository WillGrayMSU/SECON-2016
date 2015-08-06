/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: etch_code.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions and constants to draw IEEE on an
* etch-a-sketch using servos
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/26/2014               SECON 2015
*********************************************************************/

#include "etch_code.h"

///////////////////////////////////////////////
//
// Etch Usage
//
///////////////////////////////////////////////

void etch_init() {
    stop_servo(ETCH_VERTICAL);
    stop_servo(ETCH_HORIZ);
}

void play_etch() {
    display_draw_number(ETCH_NUMBER);
    #ifdef DEBUG_BUILD
    printf("\n*** Drawing 'IEEE' ***\n");
    #endif

    // Pull in etch/get arm out of the way
    game_arm_pull_etch();

    // Lower knobs on etch
    platform_etch();

    // Hold in the etch
    game_arm_hold_etch();

    // Draw
    draw_IEEE();

    // Release the etch
    game_arm_release();

    // Reset the platform
    platform_etch_undo();
}

///////////////////////////////////////////////
//
// Etch Primitives
//
///////////////////////////////////////////////

void turn_servo_vertical_etch_distance(float distance) {
    uint16_t u16_degrees;

    if (distance > 0) {
        // Turn clockwise to go right
        u16_degrees = ((distance * VERTICAL_ADJUSTMENT)/FULL_ROTATION_VERT_DIST) * 360;
        turn_servo_CW_degrees(ETCH_VERTICAL, u16_degrees);
    } else {
        // Turn counter to go left
        u16_degrees = ((distance * VERTICAL_ADJUSTMENT)/FULL_ROTATION_VERT_DIST) * -360;
        turn_servo_CCW_degrees(ETCH_VERTICAL, u16_degrees);
    }
}

void turn_servo_horizontal_etch_distance(float distance) {
    uint16_t u16_degrees;

    if (distance > 0) {
        // Turn clockwise to go right
        u16_degrees = ((distance * HORIZ_ADJUSTMENT)/FULL_ROTATION_HORIZ_DIST) * 360;
        turn_servo_CW_degrees(ETCH_HORIZ, u16_degrees);
    } else {
        // Turn counter to go left
        u16_degrees = ((distance * HORIZ_ADJUSTMENT)/FULL_ROTATION_HORIZ_DIST) * -360;
        turn_servo_CCW_degrees(ETCH_HORIZ, u16_degrees);
    }
}

///////////////////////////////////////////////
//
// Etch Characters/words
//
///////////////////////////////////////////////

void draw_I_character() {
    // Top
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(-1.5 * ETCH_UNIT);

    // Length
    turn_servo_vertical_etch_distance(-4 * ETCH_UNIT);

    // Bottom
    turn_servo_horizontal_etch_distance(-1.5 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);
}

void draw_E_character_from_bottom() {
    // Bottom
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(-3 * ETCH_UNIT);

    // Middle
    turn_servo_vertical_etch_distance(2 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(2 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(-2 * ETCH_UNIT);

    // Top
    turn_servo_vertical_etch_distance(2 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);
}

void draw_E_character_from_top() {
    // Top
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(-3 * ETCH_UNIT);

    // Middle
    turn_servo_vertical_etch_distance(-2 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(2 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(-2 * ETCH_UNIT);

    // Bottom
    turn_servo_vertical_etch_distance(-2 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);

}

void draw_IEEE() {
    display_draw_number(ETCH_I_NUMBER);
    draw_I_character();
    turn_servo_horizontal_etch_distance(0.568*ETCH_UNIT);

    display_draw_number(ETCH_E1_NUMBER);
    draw_E_character_from_bottom();
    turn_servo_horizontal_etch_distance(0.568*ETCH_UNIT);

    display_draw_number(ETCH_E2_NUMBER);
    draw_E_character_from_top();
    turn_servo_horizontal_etch_distance(0.568*ETCH_UNIT);

    display_draw_number(ETCH_E3_NUMBER);
    draw_E_character_from_bottom();
}

void underline_to_reset() {
    // Get to bottom right
    turn_servo_horizontal_etch_distance(-3 * ETCH_UNIT);
    turn_servo_vertical_etch_distance(-4 * ETCH_UNIT);
    turn_servo_horizontal_etch_distance(3 * ETCH_UNIT);

    // Move down under the letters
    turn_servo_vertical_etch_distance(-1 * ETCH_UNIT);

    // Underline
    turn_servo_horizontal_etch_distance((-12 * ETCH_UNIT) - 20);

    // Move back up to starting point
    turn_servo_vertical_etch_distance(5 * ETCH_UNIT);
}
