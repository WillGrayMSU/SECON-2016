/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: card_code.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to pick up a card using the arm
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        11/19/2014              SECON 2015
*********************************************************************/

#include "servos_API.h"
#include "game_player_port_mapping.h"
#include "platform_control.h"
#include "game_arm_control.h"
#include "SSDisplayAPI.h"

#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef CARD_CODE_H_
#define CARD_CODE_H_

///////////////////////////////////////////////
//
// Card Usage
//
///////////////////////////////////////////////
/**
 * @brief Grab a card from a deck with the arm
 */
void play_card(void);

#endif
