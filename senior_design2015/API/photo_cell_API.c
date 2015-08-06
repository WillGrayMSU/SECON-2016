/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: photo_cell_API.c
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

#include "photo_cell_API.h"

///////////////////////////////////////////////
//
// Photocell usage
//
///////////////////////////////////////////////

// Inititalize analog pins
void  photo_cell_init() {
    CONFIG_RB0_AS_ANALOG();
    CONFIG_RB1_AS_ANALOG();
    CONFIG_RB2_AS_ANALOG();
    CONFIG_RB3_AS_ANALOG();

    adc_init(0);
}

// Read a given color's phototransistor
uint16_t read_photo_cell(photoTrans color) {
    switch (color) {
        case YELLOW_TRANS:
            return adc_read(0);
            break;
        case BLUE_TRANS:
            return adc_read(1);
            break;
        case RED_TRANS:
            return adc_read(2);
            break;
        case GREEN_TRANS:
            return adc_read(3);
            break;
        case START_CELL:
            return adc_read(4);
            break;
        default:
            return 0;
            break;
    }

    // I think this should work gotta test it
    // the enum in photoCellAPI.h would have to be changed
    // return adc_read(color);
}

#ifdef DEBUG_BUILD
// Continuously print out phototransistor values REQUIRES PIC RESET
void photo_trans_print() {
    printf("\nYellow %i", read_photo_cell(YELLOW_TRANS));
    printf("    Blue %i", read_photo_cell(BLUE_TRANS));
    printf("    Red %i", read_photo_cell(RED_TRANS));
    printf("    Green %i", read_photo_cell(GREEN_TRANS));
}
#endif
