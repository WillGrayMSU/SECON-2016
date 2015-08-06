/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: platform_control.h
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

#include "servos_API.h"
#include "game_player_port_mapping.h"

#ifndef PLATFORM_CONTROL_H_
#define PLATFORM_CONTROL_H_

#define PLATFORM_UP_PULSE       2050
#define PLATFORM_RUBIK_PULSE    1500
#define PLATFORM_ETCH_PULSE     1000

#define TWIST_INITIAL_PULSE     2050
#define TWIST_DIAG_PULSE        1200

#define PLATFORM_WAIT           1000

///////////////////////////////////////////////
//
// Platform config
//
///////////////////////////////////////////////
/**
 * @brief Initialize the platform
 */
void platform_init(void);

///////////////////////////////////////////////
//
// Platform usage
//
///////////////////////////////////////////////
/**
 * @brief Raise the platform to it's intial position
**/
void platform_up(void);

/**
 * @brief Raise the platform to it's down position
**/
void platform_down(void);

/**
 * @brief Lower the platform to twist the cube
 */
void platform_rubiks(void);

/**
 * @brief Move twist and platform to position etch servos
 */
void platform_etch(void);

/**
 * @brief Put the platform and twist back to the inital positition after flipping
**/
void platform_etch_undo(void);

#endif
