/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_1.c
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

#include "sensor_array_1.h"

uint16_t u16_maxValueLine1 = 0x7FFF;

///////////////////////////////////////////////
//
// Sensor config
//
///////////////////////////////////////////////

// Initializes sensor array
void sensor_array_init_line1() {
    CONFIG_RB8_AS_DIG_OUTPUT();
}

// Configures pins as dig outputs
void config_outputs_line1() {
    CONFIG_RB0_AS_DIG_OUTPUT();
    CONFIG_RB1_AS_DIG_OUTPUT();
    CONFIG_RB2_AS_DIG_OUTPUT();
    CONFIG_RB3_AS_DIG_OUTPUT();
    CONFIG_RB4_AS_DIG_OUTPUT();
    CONFIG_RB5_AS_DIG_OUTPUT();
    CONFIG_RB6_AS_DIG_OUTPUT();
    CONFIG_RB7_AS_DIG_OUTPUT();
}

// Configures pins as dig inputs
void config_inputs_line1() {
    CONFIG_RB0_AS_DIG_INPUT();
    DISABLE_RB0_PULLUP();

    CONFIG_RB1_AS_DIG_INPUT();
    DISABLE_RB1_PULLUP();

    CONFIG_RB2_AS_DIG_INPUT();
    DISABLE_RB2_PULLUP();

    CONFIG_RB3_AS_DIG_INPUT();
    DISABLE_RB3_PULLUP();

    CONFIG_RB4_AS_DIG_INPUT();
    DISABLE_RB4_PULLUP();

    CONFIG_RB5_AS_DIG_INPUT();
    DISABLE_RB5_PULLUP();

    CONFIG_RB6_AS_DIG_INPUT();
    DISABLE_RB6_PULLUP();

    CONFIG_RB7_AS_DIG_INPUT();
    DISABLE_RB7_PULLUP();
}

///////////////////////////////////////////////
//
// Sensor primitives
//
///////////////////////////////////////////////

// Sets all the sensors high and times as they discharge
void set_sensors_high_line1() {
    SENSOR0_LINE1_OUT = 1;
    SENSOR1_LINE1_OUT = 1;
    SENSOR2_LINE1_OUT = 1;
    SENSOR3_LINE1_OUT = 1;
    SENSOR4_LINE1_OUT = 1;
    SENSOR5_LINE1_OUT = 1;
    SENSOR6_LINE1_OUT = 1;
    SENSOR7_LINE1_OUT = 1;
}

// Check each sensor on this array
void check_sensor_values_line1(uint16_t* pau16_sensorValues, uint16_t u16_delta) {
    if((SENSOR0_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[0])) {
        pau16_sensorValues[0] = u16_delta;
    }
    if((SENSOR1_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[1])) {
        pau16_sensorValues[1] = u16_delta;
    }
    if((SENSOR2_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[2])) {
        pau16_sensorValues[2] = u16_delta;
    }
    if((SENSOR3_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[3])) {
        pau16_sensorValues[3] = u16_delta;
    }
    if((SENSOR4_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[4])) {
        pau16_sensorValues[4] = u16_delta;
    }
    if((SENSOR5_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[5])) {
        pau16_sensorValues[5] = u16_delta;
    }
    if((SENSOR6_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[6])) {
        pau16_sensorValues[6] = u16_delta;
    }
    if((SENSOR7_LINE1_IN == 0) && (u16_delta < pau16_sensorValues[7])) {
        pau16_sensorValues[7] = u16_delta;
    }
}

// Disable LEDs
void emitters_off_line1() {
    EMITTER_LINE1 = 0;
    DELAY_US(EMITTER_DELAY);
}

// Enable LEDs
void emitters_on_line1() {
    EMITTER_LINE1 = 1;
    DELAY_US(EMITTER_DELAY);
}

// Get the max value from the calibration step
uint16_t get_line_max_value_line1() {
    return u16_maxValueLine1;
}

// Set the max value during calibration
void set_line_max_value_line1(uint16_t u16_maxValue) {
    u16_maxValueLine1 = u16_maxValue;
}
