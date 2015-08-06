/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_4.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control sensor array 4
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        2/11/2015               SECON 2015
*********************************************************************/

#include "sensor_array_4.h"

uint16_t u16_maxValueLine4 = 0x7FFF;

///////////////////////////////////////////////
//
// Sensor config
//
///////////////////////////////////////////////

// Initializes sensor array
void sensor_array_init_line4() {
    CONFIG_RC12_AS_DIG_OUTPUT();
}

// Configures pins as dig outputs
void config_outputs_line4() {
    CONFIG_RD9_AS_DIG_OUTPUT();
    CONFIG_RD10_AS_DIG_OUTPUT();
    CONFIG_RD11_AS_DIG_OUTPUT();
    CONFIG_RD0_AS_DIG_OUTPUT();
    CONFIG_RC13_AS_DIG_OUTPUT();
    CONFIG_RC14_AS_DIG_OUTPUT();
    CONFIG_RD5_AS_DIG_OUTPUT();
    CONFIG_RD6_AS_DIG_OUTPUT();
}

// Configures pins as dig inputs
void config_inputs_line4() {
    CONFIG_RD9_AS_DIG_INPUT();
    DISABLE_RD9_PULLUP();

    CONFIG_RD10_AS_DIG_INPUT();
    DISABLE_RD10_PULLUP();

    CONFIG_RD11_AS_DIG_INPUT();
    DISABLE_RD11_PULLUP();

    CONFIG_RD0_AS_DIG_INPUT();
    DISABLE_RD0_PULLUP();

    CONFIG_RC13_AS_DIG_INPUT();
    DISABLE_RC13_PULLUP();

    CONFIG_RC14_AS_DIG_INPUT();
    DISABLE_RC14_PULLUP();

    CONFIG_RD5_AS_DIG_INPUT();
    DISABLE_RD5_PULLUP();

    CONFIG_RD6_AS_DIG_INPUT();
    DISABLE_RD6_PULLUP();
}

///////////////////////////////////////////////
//
// Sensor primitives
//
///////////////////////////////////////////////

// Sets all the sensors high and times as they discharge
void set_sensors_high_line4() {
    SENSOR0_LINE4_OUT = 1;
    SENSOR1_LINE4_OUT = 1;
    SENSOR2_LINE4_OUT = 1;
    SENSOR3_LINE4_OUT = 1;
    SENSOR4_LINE4_OUT = 1;
    SENSOR5_LINE4_OUT = 1;
    SENSOR6_LINE4_OUT = 1;
    SENSOR7_LINE4_OUT = 1;
}

// Check each sensor on this array
void check_sensor_values_line4(uint16_t* pau16_sensorValues, uint16_t u16_delta) {
    if((SENSOR0_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[0])) {
        pau16_sensorValues[0] = u16_delta;
    }
    if((SENSOR1_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[1])) {
        pau16_sensorValues[1] = u16_delta;
    }
    if((SENSOR2_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[2])) {
        pau16_sensorValues[2] = u16_delta;
    }
    if((SENSOR3_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[3])) {
        pau16_sensorValues[3] = u16_delta;
    }
    if((SENSOR4_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[4])) {
        pau16_sensorValues[4] = u16_delta;
    }
    if((SENSOR5_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[5])) {
        pau16_sensorValues[5] = u16_delta;
    }
    if((SENSOR6_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[6])) {
        pau16_sensorValues[6] = u16_delta;
    }
    if((SENSOR7_LINE4_IN == 0) && (u16_delta < pau16_sensorValues[7])) {
        pau16_sensorValues[7] = u16_delta;
    }
}

// Disable LEDs
void emitters_off_line4() {
    EMITTER_LINE4 = 0;
    DELAY_US(EMITTER_DELAY);
}

// Enable LEDs
void emitters_on_line4() {
    EMITTER_LINE4 = 1;
    DELAY_US(EMITTER_DELAY);
}

// Get the max value from the calibration step
uint16_t get_line_max_value_line4() {
    return u16_maxValueLine4;
}

// Set the max value during calibration
void set_line_max_value_line4(uint16_t u16_maxValue) {
    u16_maxValueLine4 = u16_maxValue;
}
