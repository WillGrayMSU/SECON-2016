/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: ADC_API.c
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

#include "ADC_API.h"

/////////////////////////////////////////////// 
//
// ADC usage
//
///////////////////////////////////////////////

void adc_init(uint8_t u8_channel) {
    // Turn off ADC while configing
    AD1CON1bits.ADON = 0;

    // Disable auto sampling
    AD1CON1 = ADC_CLK_AUTO | ADC_AUTO_SAMPLING_OFF;

    // Twelve bit mode and internal vref
    AD1CON1bits.AD12B = 1;
    AD1CON2 = ADC_VREF_AVDD_AVSS;

    // Select the pin we are reading from based on passed in channel
    if (u8_channel == 0) {
        AD1CHS0 = ADC_CH0_NEG_SAMPLEA_VREFN | RB0_AN;
    } else if (u8_channel == 1) {
        AD1CHS0 = ADC_CH0_NEG_SAMPLEA_VREFN | RB1_AN;
    } else if (u8_channel == 2) {
        AD1CHS0 = ADC_CH0_NEG_SAMPLEA_VREFN | RB2_AN;
    } else if (u8_channel == 3) {
        AD1CHS0 = ADC_CH0_NEG_SAMPLEA_VREFN | RB3_AN;
    }
    // Use internal clock and turn the ADC back on
    AD1CON3 = ADC_CONV_CLK_INTERNAL_RC;
    AD1CON1bits.ADON = 1;

}

uint16_t adc_read(uint8_t u8_channel) {
    static uint8_t u8_wdState;
    uint16_t u16_data;

    adc_change_channel(u8_channel);

    DELAY_MS(10);

    u8_wdState = _SWDTEN;
    _SWDTEN = 1;
    AD1CON1bits.SAMP=1;
    while (!IS_CONVERSION_COMPLETE_ADC1());
    _SWDTEN = u8_wdState;
    u16_data = ADC1BUF0;
    return u16_data;
}

/////////////////////////////////////////////// 
//
// ADC primitives
//
///////////////////////////////////////////////

void adc_change_channel(uint8_t u8_channel) {
    // Turn off ADC while configing
    AD1CON1bits.ADON = 0;

    AD1CHS0 = ADC_CH0_NEG_SAMPLEA_VREFN | u8_channel;

    AD1CON1bits.ADON = 1;
}