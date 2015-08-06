/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_3.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control sensor array 3
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        2/11/2015               SECON 2015
*********************************************************************/

#include "sensor_array_3.h"

uint16_t u16_maxValueLine3 = 0x7FFF;

///////////////////////////////////////////////
//
// Sensor config
//
///////////////////////////////////////////////

// Initializes sensor array
void sensor_array_init_line3() {
    CONFIG_RC15_AS_DIG_OUTPUT();
}

// Configures pins as dig outputs
void config_outputs_line3() {
    CONFIG_RD7_AS_DIG_OUTPUT();
    CONFIG_RF0_AS_DIG_OUTPUT();
    CONFIG_RF1_AS_DIG_OUTPUT();
    CONFIG_RG1_AS_DIG_OUTPUT();
    CONFIG_RG0_AS_DIG_OUTPUT();
    CONFIG_RG14_AS_DIG_OUTPUT();
    CONFIG_RG12_AS_DIG_OUTPUT();
    CONFIG_RG13_AS_DIG_OUTPUT();
}

// Configures pins as dig inputs
void config_inputs_line3() {
    CONFIG_RD7_AS_DIG_INPUT();
    DISABLE_RD7_PULLUP();

    CONFIG_RF0_AS_DIG_INPUT();
    DISABLE_RF0_PULLUP();

    CONFIG_RF1_AS_DIG_INPUT();
    DISABLE_RF1_PULLUP();

    CONFIG_RG1_AS_DIG_INPUT();
    DISABLE_RG1_PULLUP();

    CONFIG_RG0_AS_DIG_INPUT();
    DISABLE_RG0_PULLUP();

    CONFIG_RG14_AS_DIG_INPUT();
    DISABLE_RG14_PULLUP();

    CONFIG_RG12_AS_DIG_INPUT();
    DISABLE_RG12_PULLUP();

    CONFIG_RG13_AS_DIG_INPUT();
    DISABLE_RG13_PULLUP();
}

///////////////////////////////////////////////
//
// Sensor primitives
//
///////////////////////////////////////////////

// Sets all the sensors high and times as they discharge
void set_sensors_high_line3() {
    SENSOR0_LINE3_OUT = 1;
    SENSOR1_LINE3_OUT = 1;
    SENSOR2_LINE3_OUT = 1;
    SENSOR3_LINE3_OUT = 1;
    SENSOR4_LINE3_OUT = 1;
    SENSOR5_LINE3_OUT = 1;
    SENSOR6_LINE3_OUT = 1;
    SENSOR7_LINE3_OUT = 1;
}

// Check each sensor on this array
void check_sensor_values_line3(uint16_t* pau16_sensorValues, uint16_t u16_delta) {
    if((SENSOR0_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[0])) {
        pau16_sensorValues[0] = u16_delta;
    }
    if((SENSOR1_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[1])) {
        pau16_sensorValues[1] = u16_delta;
    }
    if((SENSOR2_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[2])) {
        pau16_sensorValues[2] = u16_delta;
    }
    if((SENSOR3_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[3])) {
        pau16_sensorValues[3] = u16_delta;
    }
    if((SENSOR4_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[4])) {
        pau16_sensorValues[4] = u16_delta;
    }
    if((SENSOR5_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[5])) {
        pau16_sensorValues[5] = u16_delta;
    }
    if((SENSOR6_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[6])) {
        pau16_sensorValues[6] = u16_delta;
    }
    if((SENSOR7_LINE3_IN == 0) && (u16_delta < pau16_sensorValues[7])) {
        pau16_sensorValues[7] = u16_delta;
    }
}

// Disable LEDs
void emitters_off_line3() {
    EMITTER_LINE3 = 0;
    DELAY_US(EMITTER_DELAY);
}

// Enable LEDs
void emitters_on_line3() {
    EMITTER_LINE3 = 1;
    DELAY_US(EMITTER_DELAY);
}

// Get the max value from the calibration step
uint16_t get_line_max_value_line3() {
    return u16_maxValueLine3;
}

// Set the max value during calibration
void set_line_max_value_line3(uint16_t u16_maxValue) {
    u16_maxValueLine3 = u16_maxValue;
}
