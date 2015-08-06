/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: photo_cell_API.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help read from photo cells
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        10/13/2014              SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "game_player_port_mapping.h"
#include "ADC_API.h"

#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef PHOTO_CELL_API_H_
#define PHOTO_CELL_API_H_

typedef enum {
    YELLOW_TRANS =  0,
    BLUE_TRANS =    1,
    RED_TRANS =     2,
    GREEN_TRANS =   3,
    START_CELL =    4
} photoTrans;

///////////////////////////////////////////////
//
// Photocell usage
//
///////////////////////////////////////////////

/**
 * @brief Configure the analog channels and ADC
 */
void  photo_cell_init();

/**
 * @brief Read the value from a photo transistor
 *
 * @param color Photo transistor to be read
 * @return the value of the photo transistor
 */
uint16_t read_photo_cell(photoTrans color);

/**
 * @brief Continuously print out all the the photo transistors
 * @details THIS REQUIRES A PIC RESET, WOULDN'T RECCOMMEND
 */
void photo_trans_print();

#endif
