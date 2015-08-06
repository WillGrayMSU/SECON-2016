/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: servos_API.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help setup and control servos
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/20/2014               SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "game_player_port_mapping.h"

#ifndef SERVOS_API_H_
#define SERVOS_API_H_

#define NUM_SERVOS1 5
#define NUM_SERVOS2 6

#define CONT_COUNTER_FULL_ROTATION_TIME  905    // Full counter rotation time for a continuous servo in seconds
#define CONT_CLOCK_FULL_ROTATION_TIME    915    // Full clock rotation time for a continuous servo in seconds
#define STEP_SIZE 1

#define NEUTRAL_PULSE       1500
#define CLOCKWISE_PULSE     2000
#define COUNTER_PULSE       1000

#define MIN_PW              600     // Minimum pulse width, in us
#define MAX_PW              2400    // Maximum pulse width, in us
#define SLOT_WIDTH          2800    // Slot width, in us
#define MOTOR_PWM_PERIOD    20000   // Full pulse width in ms

typedef enum {
    ETCH_VERTICAL =     0,
    ETCH_HORIZ =        1,
    RUBIKS_PLATFORM =   2,
    RUBIKS_TWIST =      3,
    ARM_POSITION =      4,
    SIMON_YELLOW =      5,
    SIMON_BLUE =        6,
    SIMON_RED =         7,
    SIMON_GREEN =       8,
    ARM_SLIDE_LEFT =    9,
    ARM_SLIDE_RIGHT =   10
} servoIDs;

///////////////////////////////////////////////
//
// Servo config
//
///////////////////////////////////////////////
/**
 * @brief Initializes every thing for all the servos in the servoID enum
 */
void servo_init(void);

/**
 * @brief Configs timer 3 to drive servos
 */
void config_servo_timer3(void);

/**
 * @brief Configure the output comparator for servo usage
 * @details Uses OC1
 */
void config_output_capture1(void);

/**
 * @brief Configs timer 2 to drive servos
 */
void config_servo_timer2(void);

/**
 * @brief Configure the output comparator for servo usage
 * @details Uses OC2
 */
void config_output_capture2(void);

///////////////////////////////////////////////
//
// Servo primitives
//
///////////////////////////////////////////////
/**
 * @brief Set a new pusle width to a servo, called by the interrupt ISR
 *
 * @param u8_servo servo number to change
 * @param u8_val new value to assign
 */
void set_servo_output (uint8_t u8_servo, uint8_t u8_val);

///////////////////////////////////////////////
//
// Servo usage
//
///////////////////////////////////////////////
/**
 * @brief Provides the given servo a pulse width in order to turn it
 *
 * @param id the servoID of the servo to turn
 * @param pulseWidth The pulse width to send the servo
 */
void turn_servo_by_pulse(servoIDs id, uint16_t pulseWidth);

/**
 * @brief Stops a servo from turning
 *
 * @param id the servoID of the servo to stop
 */
void stop_servo (servoIDs id);

/**
 * @brief Stops a servo from turning
 *
 * @param direction clockwise or counter
 * @param id the servoID of the servo to step
 */
void step_servo (uint8_t direction, servoIDs id);

/**
 * @brief Turn a servo clockwise by a certain degree amount
 * @details This turns the servo counter clockwise when looking at
 * the back of the servo
 *
 * @param id the servoID of the servo to turn
 * @param degrees number of degrees to turn the servo
 */
void turn_servo_CW_degrees(servoIDs id, float degrees);

/**
 * @brief Turn a servo counter-clockwise by a certain degree amount
 * @details This turns the servo counter clockwise when looking at
 * the back of the servo
 *
 * @param id the servoID of the servo to turn
 * @param degrees number of degrees to turn the servo
 */
void turn_servo_CCW_degrees(servoIDs id, float degrees);

/**
 * @brief Turn a servo to a certain degree
 *
 * @param id the servoID of the servo to turn
 * @param degrees number of degrees to turn the servo
 */
void turn_servo_to_degree(servoIDs id, float degree);

#endif
