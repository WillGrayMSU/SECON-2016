/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: etch_code.h
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

#include "servos_API.h"
#include "game_player_port_mapping.h"
#include "platform_control.h"
#include "game_arm_control.h"
#include "SSDisplayAPI.h"

#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef ETCH_CODE_H_
#define ETCH_CODE_H_

#define FULL_ROTATION_HORIZ_DIST    18.5039     // in mm
#define FULL_ROTATION_VERT_DIST     19.3421     // in mm
#define ETCH_UNIT                   2.2         // 4.4 mm unit for simplicity of code
#define VERTICAL_ADJUSTMENT         1.0         // Vertical adjustment factor
#define HORIZ_ADJUSTMENT            1.0         // Horizontal adjustment factor

///////////////////////////////////////////////
//
// Etch Usage
//
///////////////////////////////////////////////
/**
 * @brief Initialize servos to not move at all
 */
void etch_init(void);

/**
 * @brief Lower etch knobs on to the etch and draw IEEE
 */
void play_etch(void);

///////////////////////////////////////////////
//
// Etch primitives
//
///////////////////////////////////////////////
/**
 * @brief Turn the vertical nob of the etch-a-sketch
 * @details This function is custom tuned to the etch-a-sketch nobs
 *
 * @param distance Distance in millimeters to move the cursor in the vertical direction
 */
void turn_servo_vertical_etch_distance(float distance);

/**
 * @brief Turn the horizontal nob of the etch-a-sketch
 * @details This function is custom tuned to the etch-a-sketch nobs
 *
 * @param distance Distance in millimeters to move the cursor in the horizontal direction
 */
void turn_servo_horizontal_etch_distance(float distance);

/**
 * @brief I-Character on etch
 */
void draw_I_character(void);

/**
 * @brief E-Character on etch starting from bottom
 */
void draw_E_character_from_bottom(void);

/**
 * @brief E-Character on etch from top
 */
void draw_E_character_from_top(void);

/**
 * @brief Full IEEE word
 */
void draw_IEEE(void);

/**
 * @brief Underlines IEEE and goes back to beginning
 *
 * @description Sort of test code, probably won't ever use this
 */
void underline_to_reset(void);

#endif
