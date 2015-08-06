/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_common.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control sensor array 1
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        2/11/2015               SECON 2015
*********************************************************************/

#include "pic24_all.h"
#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef SENSOR_ARRAY_COMMON_H_
#define SENSOR_ARRAY_COMMON_H_

#define SENSOR_NUM              8
#define QTR_EMITTERS_OFF        0
#define QTR_EMITTERS_ON         1
#define QTR_EMITTERS_ON_AND_OFF 2

#define EMITTER_DELAY           200

#define MAX_VALUE               0x7FFF

/**
 * @brief Configures timer 4 for sensor usage
 */
void config_timer4(void);

#endif
