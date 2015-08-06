/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: ADC_API.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help read from the ADC
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        10/13/2014              SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "game_player_port_mapping.h"

#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef ADC_API_H_
#define ADC_API_H_

#define YELLOW_LIGHT    0
#define BLUE_LIGHT      1
#define RED_LIGHT       2
#define GREEN_LIGHT     3

///////////////////////////////////////////////
//
// ADC usage
//
///////////////////////////////////////////////

/**
 * @brief Initialize ADC1
 * @details 12 Bit mode, auto clock, auto-sampling off, internal clock, internal reference
 *
 * @param u8_channel Pin to start the ADC on
 */
void adc_init(uint8_t u8_channel);

/**
 * @brief Read the ADC on the given pin
 *
 * @param u8_channel The pin to be read
 * @return The ADC value
 */
uint16_t adc_read(uint8_t u8_channel);

///////////////////////////////////////////////
//
// ADC primitives
//
///////////////////////////////////////////////

/**
 * @brief Change the ADC to read on the given pin
 *
 * @param u8_channel Pin to be read
 */
void adc_change_channel(uint8_t u8_channel);

#endif
