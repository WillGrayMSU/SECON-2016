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

#include "sensor_array_common.h"

// Configures timer to measure discharge time of the capacitor
void  config_timer4() {
    T4CON = T4_OFF
            | T4_IDLE_CON
            | T4_GATE_OFF
            | T4_32BIT_MODE_OFF
            | T4_SOURCE_INT
            | T4_PS_1_8;  //1 tick = 0.2 us at FCY=40 MHz
    PR4 = 0xFFFF;                    //maximum period
    TMR4  = 0;                       //clear timer2 value
    _T4IF = 0;                       //clear interrupt flag
    T4CONbits.TON = 1;               //turn on the timer
}
