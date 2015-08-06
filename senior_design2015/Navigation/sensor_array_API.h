/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_API.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control sensor arrays
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/20/2014              SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "sensor_array_1.h"
#include "sensor_array_2.h"
#include "sensor_array_3.h"
#include "sensor_array_4.h"
#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef SENSOR_ARRAY_API_H_
#define SENSOR_ARRAY_API_H_

#define NUM_ARRAYS      5

#define MAIN_LINE               1
#define HI_RES_LINE             2
#define TRIPLE_LEFT_LINE        3
#define TRIPLE_RIGHT_LINE       4
#define BACK_LINE               5

#define TRIPLE_SENSOR_NUM               SENSOR_NUM * 3
#define TRIPLE_HI_RES_SENSOR_NUM        SENSOR_NUM * 4

#define PID_SENSOR_NUM                  SENSOR_NUM * 2


///////////////////////////////////////////////
//
// Sensor config
//
///////////////////////////////////////////////
/**
 * @brief Initializes and calibrates the sensor lines
 */
void sensor_array_init(void);

/**
 * @brief Calibrates a sensor in a given mode
 * @param u8_readMode Mode to calibrate in
 * @param u8_line Line to calibrate
 */
void calibrate(char u8_readMode, uint8_t u8_line);

/**
 * @brief Calibrate all the sensor arrays
 */
void calibrateAllSensorArrays();

///////////////////////////////////////////////
//
// Sensor primitives
//
///////////////////////////////////////////////
/**
 * @brief Configures all the sensor arrays pins as outputs
 * @param u8_line Line to set sensors as outputs
 */
void config_outputs(uint8_t u8_line);

/**
 * @brief Configures all the sensor arrays pins as inputs
 * @param u8_line Line to set sensors as inputs
 */
void config_inputs(uint8_t u8_line);

/**
 * @brief Sets all the sensors high and times as they discharge
 * @param u8_line Line to set its emitters high
 */
void set_sensors_high(uint8_t u8_line);

/**
 * @brief Check each sensor on this array
 * @param pau16_sensorValues the current sensor values
 * @param u16_delta the time that has elapsed
 * @param u8_line Line to check
**/
void check_sensor_values(uint16_t* pau16_sensorValues, uint16_t u16_delta, uint8_t u8_line);

/**
 * @brief Turns on the emitters
 * @param u8_line Line to turn its emitter on
 */
void emitters_on(uint8_t u8_line);

/**
 * @brief Turns off the emitters
 * @param u8_line Line to turn its emitter off
 */
void emitters_off(uint8_t u8_line);

/**
 * @brief Get the max value from the calibration step
 * @param u8_line Line to get the max value from
 * @return the max value
**/
uint16_t get_line_max_value(uint8_t u8_line);

/**
 * Set the max value during calibration
 * @param u16_maxValue the new max value
 * @param u8_line Line to set
**/
void set_line_max_value(uint16_t u16_maxValue, uint8_t u8_line);

/**
 * @brief Reads the sensor values using a specific read mode
 * @param pau16_sensorValues Values from the sensor
 * @param u8_readMode The mode to read in
 * @param u8_line The line to read
**/
void read(uint16_t* pau16_sensorValues, char u8_readMode, uint8_t u8_line);

/**
 * @brief Reads the sensor values
 * @param pau16_sensorValues Values from the sensor
 * @param u8_line The line to read
**/
void read_values(uint16_t* pau16_sensorValues, uint8_t u8_line);

///////////////////////////////////////////////
//
// Sensor usage
//
///////////////////////////////////////////////
/**
 * @brief Read an entire sensor line
 *
 * @param pau16_sensorValues the values from the sensor array
 * @param u8_readMode whether the emitters are on or not
 * @param u8_line sensor line to read
 */
void read_sensor_array(uint16_t* pau16_sensorValues, char u8_readMode, uint8_t u8_line);

/**
 * @brief Read all of the sensor lines
 *
 * @param pau16_sensorValues the values from the sensor array
 * @param u8_readMode whether the emitters are on or not
 */
void read_all_sensor_arrays(uint16_t* pau16_allSensorValues, char u8_readMode);

/**
 * @brief Read the consecutive line of three lines
 * @param pau16_tripleSensorValues the values from the sensor array
 *
**/
void read_sensor_triple(uint16_t* pau16_tripleSensorValues, char u8_readMode);

/**
 * @brief Read the high resolution double sensor line
 * @param pau16_hiResSensorValues the values from the sensor array
 *
**/
void read_sensor_hi_res(uint16_t* pau16_hiResSensorValues, char u8_readMode);

/**
 * @brief Read the consecutive line of three lines plus the high resolution double sensor line
 * @param pau16_tripleHiResSensorValues the values from the sensor array
 *
**/
void read_sensor_triple_plus_hi_res(uint16_t* pau16_tripleHiResSensorValues, char u8_readMode);
int16_t read_line(char u8_readMode);

///////////////////////////////////////////////
//
// Sensor printing (self explanatory)
//
///////////////////////////////////////////////
#ifdef DEBUG_BUILD
void print_sensor_array(uint8_t u8_line);
void print_all_sensor_arrays(void);
void print_sensor_triple(void);
void print_sensor_hi_res(void);
void print_sensor_triple_plus_hi_res(void);
#endif
#endif
