/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: simon_code.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to record lights coming from the simon
* then push the correct buttons to play the game
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        10/5/2014               SECON 2015
*********************************************************************/

#include "servos_API.h"
#include "photo_cell_API.h"
#include "game_arm_control.h"
#include "game_player_port_mapping.h"
#include "platform_control.h"
#include "rubiks_code.h"
#include "SSDisplayAPI.h"

#ifndef SIMON_CODE_H_
#define SIMON_CODE_H_

// Retract pulse width constants
#define YELLOW_RETRACT  2520
#define BLUE_RETRACT    2330 // 2450
#define RED_RETRACT     2000
#define GREEN_RETRACT   1630

// Hover pulse width constants
#define YELLOW_HOVER    925
#define BLUE_HOVER      900 // 1020
#define RED_HOVER       970
#define GREEN_HOVER     910
#define START_HOVER     YELLOW_HOVER

// Push pulse width constants
#define YELLOW_PUSH     825
#define BLUE_PUSH       870 // 990
#define RED_PUSH        920
#define GREEN_PUSH      900
#define START_PUSH      YELLOW_PUSH

#define YELLOW_MAX_PUSH     (YELLOW_PUSH - 100)
#define BLUE_MAX_PUSH       (BLUE_PUSH - MAX_PUSH_INCREASE)
#define RED_MAX_PUSH        (RED_PUSH - MAX_PUSH_INCREASE)
#define GREEN_MAX_PUSH      (GREEN_PUSH - MAX_PUSH_INCREASE)
#define START_MAX_PUSH      YELLOW_MAX_PUSH

#define MAX_PUSH_INCREASE 400

// Light thresholds
#define YELLOW_LIGHT_THRESH_HOLD    55
#define BLUE_LIGHT_THRESH_HOLD      90 // 125
#define RED_LIGHT_THRESH_HOLD       90 // 125
#define GREEN_LIGHT_THRESH_HOLD     90
#define OFF_THRESH_HOLD             20

// Amount to increase pulses when a push has not been successful
#define PULSE_INCREASE  25

// Various delays
#define PLATFORM_WAIT   1000
#define PUSH_WAIT       250
#define RESPONSE_WAIT   10

// The length of time in seconds to play Simon for
#define SIMON_DURATION  15

typedef enum {
    YELLOW_BUTTON = 0,
    BLUE_BUTTON =   1,
    RED_BUTTON =    2,
    GREEN_BUTTON =  3,
    START_SIMON_BUTTON =  4
} buttonID;


///////////////////////////////////////////////
//
// Simon Usage
//
///////////////////////////////////////////////

/**
 * @brief Initialize simon arms to the retracted postion
 */
void simon_init(void);

/**
 * @brief Plays simon using servos and photo transistors
 */
void play_simon(void);

/**
 * @brief Plays simon using servos and photo transistors infinitely
 */
void play_simon_infinitely();

///////////////////////////////////////////////
//
// Simon primitives
//
///////////////////////////////////////////////

/**
 * @brief Calibrates the photo transistors for the Simon.
 * Stores the readings whene there is only ambient light
 * as the lowest value for that sensor/
 */
void calibrate_sensors(void);

/**
 * @brief Detects the Simon colors using the photo transistors
 *
 * @param u8_numberOfButtons The number of buttons to expect
 */
void record_colors(uint8_t u8_numberOfButtons);

/**
 * @brief Using the servo arms and an array of recorded buttons,
 * Simon buttons are pushed
 *
 * @param u8_numRounds The number of buttons to push
 */
void play_buttons(uint8_t u8_numRounds);

/**
 * @brief Blocking function that blocks until the passed color turns on
 *
 * @param u8_color The color to look for
 */
void confirm_color(uint8_t u8_color);

/**
 * @brief Blocking function that blocks until the passed color turns off
 *
 * @param u8_color The color to confirm that is off
 */
void confirm_color_off(uint8_t u8_color);

/**
 * Retract the arm for the given button
 *
 * @param button arm to be retracted
 */
void simon_retract_button(buttonID button);

/**
 * Hover the arm for the given button
 *
 * @param button arm to be hovered
 */
void simon_hover_button(buttonID button);

/**
 * Push the given Simon button
 *
 * @param button to be pushed
 */
void simon_push_button(buttonID button);

/**
 * Push the given Simon button then return to hover position
 *
 * @param button to be pushed then hovered
 */
void simon_push_and_hover_button(buttonID button);

/**
 * @brief Push all simon buttons
 */
void simon_push_buttons(void);

/**
 * @brief Position all servo arms to the hover position
 */
void simon_hover_buttons(void);

/**
 * @brief Position all servo arms in the retract position
 */
void simon_retract_buttons(void);

/**
 * @brief Push and hover all simon buttons
 */
void simon_push_and_hover_buttons(void);

void increase_button_push(buttonID button);

/**
 * @brief Config a timer to count to 15 seconds
 */
void  configTimer5(void);


#endif
