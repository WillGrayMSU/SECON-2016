/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_2.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control sensor array 2
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        2/11/2015               SECON 2015
*********************************************************************/

#include "sensor_array_2.h"

uint16_t u16_maxValueLine2 = 0x7FFF;

///////////////////////////////////////////////
//
// Sensor config
//
///////////////////////////////////////////////

// Initializes sensor array
void sensor_array_init_line2() {
    CONFIG_RF5_AS_DIG_OUTPUT();
}

// Configures pins as dig outputs
void config_outputs_line2() {
    CONFIG_RC1_AS_DIG_OUTPUT();
    CONFIG_RB9_AS_DIG_OUTPUT();
    CONFIG_RB10_AS_DIG_OUTPUT();
    CONFIG_RB11_AS_DIG_OUTPUT();
    CONFIG_RB12_AS_DIG_OUTPUT();
    CONFIG_RB13_AS_DIG_OUTPUT();
    CONFIG_RB14_AS_DIG_OUTPUT();
    CONFIG_RF4_AS_DIG_OUTPUT();
}

// Configures pins as dig inputs
void config_inputs_line2() {
    CONFIG_RC1_AS_DIG_INPUT();
    DISABLE_RC1_PULLUP();

    CONFIG_RB9_AS_DIG_INPUT();
    DISABLE_RB9_PULLUP();

    CONFIG_RB10_AS_DIG_INPUT();
    DISABLE_RB10_PULLUP();

    CONFIG_RB11_AS_DIG_INPUT();
    DISABLE_RB11_PULLUP();

    CONFIG_RB12_AS_DIG_INPUT();
    DISABLE_RB12_PULLUP();

    CONFIG_RB13_AS_DIG_INPUT();
    DISABLE_RB13_PULLUP();

    CONFIG_RB14_AS_DIG_INPUT();
    DISABLE_RB14_PULLUP();

    CONFIG_RF4_AS_DIG_INPUT();
    DISABLE_RF4_PULLUP();
}

///////////////////////////////////////////////
//
// Sensor primitives
//
///////////////////////////////////////////////

// Sets all the sensors high and times as they discharge
void set_sensors_high_line2() {
    SENSOR0_LINE2_OUT = 1;
    SENSOR1_LINE2_OUT = 1;
    SENSOR2_LINE2_OUT = 1;
    SENSOR3_LINE2_OUT = 1;
    SENSOR4_LINE2_OUT = 1;
    SENSOR5_LINE2_OUT = 1;
    SENSOR6_LINE2_OUT = 1;
    SENSOR7_LINE2_OUT = 1;
}

// Check each sensor on this array
void check_sensor_values_line2(uint16_t* pau16_sensorValues, uint16_t u16_delta) {
    if((SENSOR0_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[0])) {
        pau16_sensorValues[0] = u16_delta;
    }
    if((SENSOR1_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[1])) {
        pau16_sensorValues[1] = u16_delta;
    }
    if((SENSOR2_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[2])) {
        pau16_sensorValues[2] = u16_delta;
    }
    if((SENSOR3_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[3])) {
        pau16_sensorValues[3] = u16_delta;
    }
    if((SENSOR4_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[4])) {
        pau16_sensorValues[4] = u16_delta;
    }
    if((SENSOR5_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[5])) {
        pau16_sensorValues[5] = u16_delta;
    }
    if((SENSOR6_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[6])) {
        pau16_sensorValues[6] = u16_delta;
    }
    if((SENSOR7_LINE2_IN == 0) && (u16_delta < pau16_sensorValues[7])) {
        pau16_sensorValues[7] = u16_delta;
    }
}

// Disable LEDs
void emitters_off_line2() {
    EMITTER_LINE2 = 0;
    DELAY_US(EMITTER_DELAY);
}

// Enable LEDs
void emitters_on_line2() {
    EMITTER_LINE2 = 1;
    DELAY_US(EMITTER_DELAY);
}

// Get the max value from the calibration step
uint16_t get_line_max_value_line2() {
    return u16_maxValueLine2;
}

// Set the max value during calibration
void set_line_max_value_line2(uint16_t u16_maxValue) {
    u16_maxValueLine2 = u16_maxValue;
}
