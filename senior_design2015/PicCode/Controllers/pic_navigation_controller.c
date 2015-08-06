/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: pic_navigation_controller.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: PIC code that will give a user control over all
* the navigation functions via a serial menu
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        12/5/2014              SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "line_follower_API.h"
#include "navigation_port_mapping.h"
#include "secon_robot_configuration.h"
#include "SSDisplayAPI.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Variable to hold user input
char u8_c;
char u8_c2;

// Game order
uint8_t pu8_gameOrder[4];
char pu8_branchList[4];

extern queue_t navigationRoutineQueue;
extern queue_t navigationMoveDistanceQueue;
extern uint8_t u8_routineBlock;
extern uint8_t u8_currentRoutine;

uint8_t u8_staticTurnLayoutNumber;
uint8_t u8_staticTurnLayoutAvailable;
uint8_t pau8_turnList[128] = {0};
uint8_t u8_turnLayoutNumber;

// Function declarations
void pic_navigation_init();
void navigation_serial_command(uint8_t u8_motor);
void motor_control(uint8_t u8_motor, uint8_t u8_function);
void navigation_serial_menu(void);
void single_motor_function_menu(void);
void double_motor_function_menu(void);
void sensor_array_menu(void);
void navigation_queue_menu(void);
void pid_menu(void);
void sensor_array_print(uint8_t u8_sensorArray);
void print_get_line(void);
void navigate_course(void);
void handle_navigation_queue_command(uint8_t u8_function);
void handle_pid_command(uint8_t u8_function);
void follow_line_pid(float f_maxSpeed, uint8_t u8_direction);
void print_pid_info(float f_maxSpeed);

// Main loop for the navigation PIC controller using serial commands
int main (void) {
    // Initialize pic and print out serial menu
    configBasic(HELLO_MSG);
    while(1) {
        doHeartbeat();
    }
    // pic_navigation_init();

    // pu8_gameOrder[0] = SIMON;
    // pu8_gameOrder[1] = RUBIKS;
    // pu8_gameOrder[2] = ETCH;
    // pu8_gameOrder[3] = CARD;
    //
    // pu8_branchList[0] = 'L';
    // pu8_branchList[1] = 'R';
    // pu8_branchList[2] = 'L';
    // pu8_branchList[3] = 'R';

    // navigation_serial_menu();

    // Game playing loop to check serial commands and I2C commands
    while(1) {
        // if(isCharReady()) {
        //     // Handle serial command
        //     u8_c = inChar();
        //     navigation_serial_command(u8_c);
        //     navigation_serial_menu();
        // }
        doHeartbeat();
    }
}

// Initialization for the navigation PIC
void pic_navigation_init() {
    // Initialize everything to follow a line
    line_follower_init();
}

///////////////////////////////////////////////
//
// Serial menus for the navigation PIC
//
///////////////////////////////////////////////

// Handle top level serial commands
void navigation_serial_command(uint8_t u8_command) {
    uint8_t u8_function;
    uint8_t u8_sensorArray;
    uint8_t u8_newBase;
    char sz_buf[32];

    switch(u8_command) {
        case 'l':
            single_motor_function_menu();
            u8_function = inChar();
            motor_control(u8_command, u8_function);
            break;
        case 'r':
            single_motor_function_menu();
            u8_function = inChar();
            motor_control(u8_command, u8_function);
            break;
        case 'b':
            double_motor_function_menu();
            u8_function = inChar();
            motor_control(u8_command, u8_function);
            break;
        case 'a':
            sensor_array_menu();
            u8_sensorArray = inChar();
            sensor_array_print(u8_sensorArray);
            break;
        case 'c':
            calibrate(QTR_EMITTERS_ON, 1);
            calibrate(QTR_EMITTERS_ON, 2);
            calibrate(QTR_EMITTERS_ON, 3);
            calibrate(QTR_EMITTERS_ON, 4);
            break;
        case 'n':
            follow_line_to_box(BASE_SPEED, 0, 0);
            break;
        case 'p':
            pid_menu();
            u8_function = inChar();
            handle_pid_command(u8_function);
            break;
        case 'm':
            follow_line_back_to_main_line(BASE_SPEED);
            break;
        case 'h':
            printf("\nChoose a direction to turn\n");
            printf("   Press 'l' turn left\n");
            printf("   Press 'r' turn right\n");
            printf("   Press 'e' turn left curve\n");
            printf("   Press 'k' turn right curve\n");
            u8_c = inChar();

            if (u8_c == 'r') {
                printf("Turning 90 degrees right\n");
                enqueue(&navigationRoutineQueue, PREPARE_TURN);
                enqueue(&navigationRoutineQueue, RIGHT_TURN);
                check_for_routine();
            }
            else if (u8_c == 'l') {
                printf("Turning 90 degrees left\n");
                enqueue(&navigationRoutineQueue, PREPARE_TURN);
                enqueue(&navigationRoutineQueue, LEFT_TURN);
                check_for_routine();
            }
            else if (u8_c == 'e') {
                printf("Turning 90 degrees right in a curve\n");
                enqueue(&navigationRoutineQueue, PREPARE_TURN_CURVE);
                enqueue(&navigationRoutineQueue, RIGHT_CURVE_TURN);
                check_for_routine();
            }
            else if (u8_c == 'k') {
                printf("Turning 90 degrees left in a curve\n");
                enqueue(&navigationRoutineQueue, PREPARE_TURN_CURVE);
                enqueue(&navigationRoutineQueue, LEFT_CURVE_TURN);
                check_for_routine();
            }
            else {
                printf("Invalid Choice\n");
            }
            break;
        case 't':
            printf("\nChoose a direction to turn\n");
            printf("   Press 'l' turn left\n");
            printf("   Press 'r' turn right\n");
            u8_c = inChar();

            if (u8_c == 'r') {
                printf("Turning 180 degrees right\n");
                turn_180_degrees(BASE_SPEED, RIGHT_DIRECTION);
            }
            else if (u8_c == 'l') {
                printf("Turning 180 degrees left\n");
                turn_180_degrees(BASE_SPEED, LEFT_DIRECTION);
            }
        case 'q':
            navigation_queue_menu();
            u8_function = inChar();
            handle_navigation_queue_command(u8_function);
            break;
        case 'w':
            navigate_course();
            break;
        case 's':
            printf("\nEnter new base speed\n");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%hhu", (uint8_t *) &u8_newBase);
            u8_c2 = inChar();

            set_base_speed(u8_newBase*1.0);
            break;
        case 'k':
            printf("\nEnter which static turn layout to load\n");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%hhu", (uint8_t *) &u8_turnLayoutNumber);
            u8_c2 = inChar();

            u8_staticTurnLayoutAvailable = prepare_static_course_turn_info(u8_turnLayoutNumber, pau8_turnList);
            load_turn_layout_to_line_follower(u8_staticTurnLayoutAvailable, pau8_turnList);
            break;
        case 'g':
            printf("\nEnter direction for game 1\n");
            u8_c2 = inChar();
            printf("Game 1: %c\n", u8_c2);
            pu8_branchList[0] = u8_c2;

            printf("\nEnter direction for game 2\n");
            u8_c2 = inChar();
            printf("Game 2: %c\n", u8_c2);
            pu8_branchList[1] = u8_c2;

            printf("\nEnter direction for game 3\n");
            u8_c2 = inChar();
            printf("Game 3: %c\n", u8_c2);
            pu8_branchList[2] = u8_c2;

            printf("\nEnter direction for game 4\n");
            u8_c2 = inChar();
            printf("Game 4: %c\n", u8_c2);
            pu8_branchList[3] = u8_c2;

            break;
        default:
            printf("Invalid Choice\n");
            break;
    }
}

// Handle motor control functions
void motor_control(uint8_t u8_motor, uint8_t u8_function) {
    uint16_t u16_revolutions;
    uint16_t u16_distance;

    int16_t i16_revolutions;
    int16_t i16_distance;

    int16_t i16_leftSpeed;
    int16_t i16_rightSpeed;

    char sz_buf[32];

    // Perform the given function on the given motor
    switch(u8_function) {
        case 'f':
            if (u8_motor == 'l') {
                left_motor_fwd(BASE_SPEED);
            } else if (u8_motor == 'r') {
                right_motor_fwd(BASE_SPEED);
            } else if (u8_motor == 'b') {
                motors_move_forward(BASE_SPEED);
            }
            break;
        case 'b':
            if (u8_motor == 'l') {
                left_motor_reverse(BASE_SPEED);
            } else if (u8_motor == 'r') {
                right_motor_reverse(BASE_SPEED);
            } else if (u8_motor == 'b') {
                motors_move_reverse(BASE_SPEED);
            }
            break;
        case 'r':
            motors_turn_right(BASE_SPEED);
            break;
        case 'l':
            motors_turn_left(BASE_SPEED);
            break;
        case 'o':
            printf("\nEnter number of tenths of revolutions\n");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%u", (uint16_t *) &u16_revolutions);
            u8_c2 = inChar();

            printf("\n(f)orward or (b)ack\n");
            u8_c = inChar();

            if (u8_c == 'b') {
                i16_revolutions = 0 - u16_revolutions;
            }
            else {
                i16_revolutions = u16_revolutions;
            }

            if (u8_motor == 'l') {
                printf("Turning left motor by %u tenths of revolutions\n", u16_revolutions);
                move_left_motor_by_revolutions(i16_revolutions/10.0, BASE_SPEED);
            } else if (u8_motor == 'r') {
                printf("Turning left motor by %u tenths of revolutions\n", u16_revolutions);
                move_right_motor_by_revolutions(i16_revolutions/10.0, BASE_SPEED);
            } else if (u8_motor == 'b') {
                printf("Turning motors by %u tenths of revolutions\n", u16_revolutions);
                move_by_revolutions(i16_revolutions/10.0, BASE_SPEED);
            }
            break;
        case 'd':
            printf("\nEnter distance in mm\n");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%u", (uint16_t *) &u16_distance);
            u8_c2 = inChar();

            printf("\n(f)orward or (b)ack\n");
            u8_c = inChar();

            if (u8_c == 'b') {
                i16_distance = 0 - u16_distance;
            }
            else {
                i16_distance = u16_distance;
            }

            if (u8_motor == 'l') {
                printf("Turning left motor by %u mm\n", u16_distance);
                move_left_motor_by_distance(i16_distance*1.0, BASE_SPEED);
            } else if (u8_motor == 'r') {
                printf("Turning right motor by %u mm\n", u16_distance);
                move_right_motor_by_distance(i16_distance*1.0, BASE_SPEED);
            } else if (u8_motor == 'b') {
                printf("Turning motors by %u mm\n", u16_distance);
                move_by_distance(i16_distance*1.0, BASE_SPEED);
            }
            break;
        case 's':
            if (u8_motor == 'l') {
                left_motor_stop();
            } else if (u8_motor == 'r') {
                right_motor_stop();
            } else if (u8_motor == 'b') {
                motors_stop();
            }
            break;
        case 'p':
            printf("\nEnter left speed\n");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%i", (int16_t *) &i16_leftSpeed);
            u8_c2 = inChar();
            printf("\nEnter right speed\n");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%i", (int16_t *) &i16_rightSpeed);
            u8_c2 = inChar();

            set_motors_pid(i16_leftSpeed, i16_rightSpeed, FORWARD_MOVEMENT);

            break;
        default:
            break;
    }
}

// Main serial menu for navigation pic
void navigation_serial_menu() {
    printf("\nChoose a command\n");
    printf("   l) control the left motor\n");
    printf("   r) control the right motor\n");
    printf("   b) control both motors\n");
    printf("   a) print out the sensor array values\n");
    printf("   c) recalibrate all the sensor arrays\n");
    printf("   n) navigate to a box\n");
    printf("   p) pid menu\n");
    printf("   m) navigate backwards to a mainline\n");
    printf("   h) turn 90 degrees\n");
    printf("   t) turn 180 degrees\n");
    printf("   q) use the navigation queue\n");
    printf("   w) navigate the whole course (skips the game stuff)\n");
    printf("   k) load up a static turn layout\n");
    printf("   g) load branch list\n");
}

// Menu for contorlling a single motor
void single_motor_function_menu() {
    printf("\nChoose a motor function\n");
    printf("   f) forward\n");
    printf("   b) back\n");
    printf("   o) move by revolution\n");
    printf("   d) move by distance\n");
    printf("   s) stop\n");
}

// Menu for contorlling both motors
void double_motor_function_menu() {
    printf("\nChoose a motor function\n");
    printf("   f) forward\n");
    printf("   b) back\n");
    printf("   r) right turn\n");
    printf("   l) left turn\n");
    printf("   o) move by revolution\n");
    printf("   d) move by distance\n");
    printf("   s) stop\n");
    printf("   p) set motors pid\n");
}

// Menu for all the sensor arrays
void sensor_array_menu() {
    printf("\nChoose a sensor array to print\n");
    printf("   a) all\n");
    printf("   1) line 1\n");
    printf("   2) line 2\n");
    printf("   3) line 3\n");
    printf("   4) line 4\n");
    printf("   5) line 5\n");
    printf("   t) the triple\n");
    printf("   h) the hi-res\n");
    printf("   p) the triple plus the hi-res\n");
}

// Menu for navigation queue stuff
void navigation_queue_menu() {
    printf("\nChoose a queue function\n");
    printf("   i) initiate routines\n");
    printf("   s) queue up static course routines\n");
    printf("   r) load right turn\n");
    printf("   l) load left turn\n");
    printf("   p) load prepare turn\n");
    printf("   f) load finish turn\n");
    printf("   b) load back away from box\n");
    printf("   m) load move past branch\n");
    printf("   e) load move forward distance\n");
    printf("   d) load move back distance\n");
}

// Menu for pid specific stuff
void pid_menu() {
    printf("\nChoose pid function\n");
    printf("   w) navigate whole course\n");
    printf("   b) follow line to box\n");
    printf("   r) follow line in reverse\n");
    printf("   c) follow line continuously\n");
    printf("   g) get line continuously\n");
    printf("   p) set KP\n");
    printf("   d) set KD\n");
}
// Print a certain line sensor array over and over
void sensor_array_print(uint8_t u8_sensorArray) {
    while(isCharReady() == 0) {
        switch(u8_sensorArray) {
            case 'a':
                print_all_sensor_arrays();
                break;
            case '1':
                print_sensor_array(1);
                break;
            case '2':
                print_sensor_array(2);
                break;
            case '3':
                print_sensor_array(3);
                break;
            case '4':
                print_sensor_array(4);
                break;
            case '5':
                print_sensor_array(5);
                break;
            case 't':
                print_sensor_triple();
                break;
            case 'h':
                print_sensor_hi_res();
                break;
            case 'p':
                print_sensor_triple_plus_hi_res();
                break;
        }
        doHeartbeat();
    }
}

void handle_navigation_queue_command(uint8_t u8_function) {
    uint8_t u8_c2;
    uint16_t u16_distance;
    uint8_t u8_courseNumber;
    char sz_buf[32];

    if ((u8_function == 'e') || (u8_function == 'd')) {
        printf("\nEnter distance in mm\n");
        inStringEcho(sz_buf,31);
        sscanf(sz_buf,"%u", (uint16_t *) &u16_distance);
        u8_c2 = inChar();
    }
    if (u8_function == 's') {
        printf("\nEnter course number\n");
        inStringEcho(sz_buf,31);
        sscanf(sz_buf,"%hhu", (uint8_t *) &u8_courseNumber);
        u8_c2 = inChar();
    }

    switch (u8_function) {
        case 'i':
            check_for_routine();
            break;
        case 's':
            if (prepare_course_routines(u8_courseNumber) == 1) {
                printf("Running course %u\n", u8_courseNumber);
                check_for_routine();
                while(1) {
                    if (u8_currentRoutine == PLAY_GAME_PAUSE) {
                        DELAY_MS(2500);
                        check_for_routine();
                    }
                }
            }
            else {
                printf("Invalid course number\n");
            }
        case 'r':
            enqueue(&navigationRoutineQueue, RIGHT_TURN);
            break;
        case 'l':
            enqueue(&navigationRoutineQueue, LEFT_TURN);
            break;
        case 'p':
            enqueue(&navigationRoutineQueue, PREPARE_TURN);
            break;
        case 'f':
            enqueue(&navigationRoutineQueue, FINISH_TURN);
            break;
        case 'b':
            enqueue(&navigationRoutineQueue, BACK_AWAY_FROM_BOX);
            break;
        case 'm':
            enqueue(&navigationRoutineQueue, MOVE_PAST_START_BOX);
            break;
        case 'e':
            enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, u16_distance);
            break;
        case 'd':
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, u16_distance);
            break;

    }
}

void handle_pid_command(uint8_t u8_function) {
    float f_newValue;
    char sz_buf[32];

    switch(u8_function) {
        case 'w':
            break;
        case 'b':
            follow_line_to_box(BASE_SPEED, 0, 0);
            break;
        case 'r':
            follow_line_pid(BASE_SPEED, BACKWARD_MOVEMENT);
            break;
        case 'c':
            follow_line_pid(BASE_SPEED, FORWARD_MOVEMENT);
            break;
        case 'g':
            print_get_line();
            break;
        case 'i':
            print_pid_info(BASE_SPEED);
            break;
    //     case 'p':
    //         printf("\nEnter new KP\n");
    //         inStringEcho(sz_buf,31);
    //         sscanf(sz_buf,"%f", &f_newValue);
    //         u8_c2 = inChar();
    //         // printf("New value: %f\n", (double)f_newValue);
    //
    //         set_KP(f_newValue);
    //         break;
    //     case 'd':
    //         printf("\nEnter new KP\n");
    //         inStringEcho(sz_buf,31);
    //         sscanf(sz_buf,"%f", &f_newValue);
    //         u8_c2 = inChar();
    //         // printf("New value: %f\n", (double)f_newValue);
    //
    //         set_KD(f_newValue);
    //         break;
    }
}

// Print the calculated line position
void print_get_line() {
    int16_t i16_position;
    uint16_t pau16_sensorValues[TRIPLE_HI_RES_SENSOR_NUM];

    while(isCharReady() == 0) {
        read_sensor_triple_plus_hi_res(pau16_sensorValues, QTR_EMITTERS_ON);
        i16_position = read_line(QTR_EMITTERS_ON);

        if (i16_position == 0) {
            if (check_left_for_line(pau16_sensorValues) == 1) {
                i16_position = 0;
            }
            else if (check_right_for_line(pau16_sensorValues) == 1) {
                i16_position = 15000;
            }
        }

        printf("Line position: %u\n", i16_position);
    }
}

// Navigate an entire course to the finish line
void navigate_course() {
    // Game counter
    uint8_t u8_currentGame;

    u8_currentGame = 0;

    enqueue(&navigationRoutineQueue, MOVE_PAST_START_BOX);
    check_for_routine();

    // Play Rubiks, Etch, and Simon then stop
    while(u8_currentGame <= 3) {
        // Find a box
        follow_line_to_box(BASE_SPEED, pu8_branchList[u8_currentGame], 1);

        // Make our final preperations
        final_game_preparations(pu8_gameOrder[u8_currentGame]);

        // Tell the game player to play a game
        #ifdef DEBUG_BUILD
        printf("Reached game %u\n", u8_currentGame);
        #endif
        DELAY_MS(2500);

        // Leave the game box in preperation to follow the line back
        prepare_to_leave_game(pu8_gameOrder[u8_currentGame]);

        // Get back on the line after spining around
        reverse_until_line();

        // Get back to the main line
        follow_line_back_to_main_line(BASE_SPEED);

        // Increment to the next game
        u8_currentGame++;

    }

    // Get to the finish line
    follow_line_to_box(BASE_SPEED, 1, 0);
}

void follow_line_pid(float f_maxSpeed, uint8_t u8_direction) {
    uint16_t pau16_sensorValues[TRIPLE_HI_RES_SENSOR_NUM];

    while(isCharReady() == 0) {
        read_sensor_triple_plus_hi_res(pau16_sensorValues, QTR_EMITTERS_ON);
        correct_line_error_pid(f_maxSpeed, u8_direction, pau16_sensorValues);
    }
    motors_stop();
}

void print_pid_info(float f_maxSpeed) {
    int16_t i16_lineCenter;

    int16_t i16_position;
    int16_t i16_error;
    int16_t i16_motorSpeed;
    int16_t i16_leftMotorSpeed;
    int16_t i16_rightMotorSpeed;

    static int16_t i16_lastError;

    i16_lineCenter = 7000;

    while(isCharReady() == 0) {
        i16_position = read_line(QTR_EMITTERS_ON);
        i16_error = i16_position - i16_lineCenter;

        i16_motorSpeed = KP * i16_error + KD * (i16_error - i16_lastError);
        i16_lastError = i16_error;

        i16_leftMotorSpeed = ((f_maxSpeed/100)*MOTOR_PWM_PERIOD) + i16_motorSpeed;
        i16_rightMotorSpeed = ((f_maxSpeed/100)*MOTOR_PWM_PERIOD) - i16_motorSpeed;

        #ifdef DEBUG_BUILD
        printf("Position %i, Error: %i, Motor Speed: %i, Left: %i, Right %i\n", i16_position, i16_error, i16_motorSpeed, i16_leftMotorSpeed, i16_rightMotorSpeed);
        #endif
    }
}
