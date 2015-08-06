/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: motors_API.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control DC motors
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/20/2014               SECON 2015
*********************************************************************/

#include "motors_API.h"

// Current encoder count for each motor
volatile int16_t i16_leftCounterROT = 0;
volatile int16_t i16_rightCounterROT = 0;

// Direction of each motor
uint8_t u8_leftMotorDirection = 0;
uint8_t u8_rightMotorDirection = 0;

// Current revolution count for each motor
volatile int16_t i16_rightRevolutionCount = 5000;
volatile int16_t i16_leftRevolutionCount = 5000;

// Target locations for each motor
float f_rightTargetPosition = 5000.0;
float f_leftTargetPosition = 5000.0;

// Whether or not the motors have reached their target location
volatile uint8_t u8_rightAtTarget = 1;
volatile uint8_t u8_leftAtTarget = 1;

// Flag for routine blocking
volatile uint8_t u8_routineBlock = 0;
uint8_t u8_currentRoutine = 0;
queue_t navigationRoutineQueue;
queue_t navigationMoveDistanceQueue;

float BASE_SPEED = BASE_SPEED_DEFAULT;

///////////////////////////////////////////////
//
// Motor config
//
///////////////////////////////////////////////

void motors_init(void)
{
    // Configure timers
    config_motor_timer2();
    config_motor_timer3();

    // Configure output comparators
    motor_config_output_compare2();
    motor_config_output_compare3();
    motor_config_output_compare4();
    motor_config_output_compare5();

    // Turn on timers
    T2CONbits.TON = 1;
    T3CONbits.TON = 1;

    // Configure the pins for the motors
    CONFIG_RIGHT_MOTOR_IN1();
    CONFIG_RIGHT_MOTOR_IN2();
    CONFIG_LEFT_MOTOR_IN1();
    CONFIG_LEFT_MOTOR_IN2();

    // Config the encoders
    config_encoder_interrupts();

    // Stop both motors
    motors_stop();

    init_queue(&navigationRoutineQueue);
    init_queue(&navigationMoveDistanceQueue);
}

void config_motor_timer2(void) {
    T2CON = T2_OFF
            | T2_IDLE_CON
            | T2_GATE_OFF
            | T2_32BIT_MODE_OFF
            | T2_SOURCE_INT
            | T2_PS_1_64;
    PR2 = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T2CONbits)) - 1;
    //PR2 = PULSEWIDTH1;
    TMR2  = 0;       //clear timer2 value
    _T2IF = 0;
    _T2IP = 1;
    _T2IE = 1;    //enable the Timer2 interrupt
}

void config_motor_timer3(void) {
    T3CON = T3_OFF
            | T3_IDLE_CON
            | T3_GATE_OFF
            | T2_32BIT_MODE_OFF
            | T3_SOURCE_INT
            | T3_PS_1_64;
    PR3 = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T3CONbits)) - 1;
    //PR3 = PULSEWIDTH1;
    TMR3  = 0;       //clear timer3 value
    _T3IF = 0;
    _T3IP = 1;
    _T3IE = 1;    //enable the Timer3 interrupt
}

void config_encoder_interrupts(void) {
    // Right encoder
    _INT0IF = 0;     //Clear the interrupt flag
    _INT0IP = 2;     //Choose a priority
    _INT0EP = 0;     //positive edge triggerred
    _INT0IE = 1;     //enable INT0 interrupt

    // Left encoder
    _INT1IF = 0;     //Clear the interrupt flag
    _INT1IP = 2;     //Choose a priority
    _INT1EP = 0;     //positive edge triggerred
    _INT1IE = 1;     //enable INT1 interrupt

    // Enable the input pins
    CONFIG_RF6_AS_DIG_INPUT();
    CONFIG_RD8_AS_DIG_INPUT();
}

void motor_config_output_compare2(void) {
    T2CONbits.TON = 0;          //disable Timer when configuring Output compare
    //CONFIG_OC2_TO_RP(RB4_RP);   //map OC2 to RB4
    // OC2CONbits.OCM2 = 0b110;
    OC2CONbits.OCM = 0b110; // This is possibly incorrect and may need the line above it to work, but OCM2 is 1 bit, OCM is the 3 bit register

    OC2RS = 0;  //clear both registers
    OC2R = 0;
    #ifdef OC2CON1
        //turn on the compare toggle mode using Timer2
        OC2CON1 = OC_TIMER2_SRC |     //Timer2 source
                    OC_PWM_CENTER_ALIGN;  //PWM
        OC2CON2 = OC_SYNCSEL_TIMER2;   //synchronize to timer2
    #else
        //older families, this PWM mode is compatible with center-aligned, OC2R=0
        //as writes to OC2RS sets the pulse widith.
        OC2CON = OC_TIMER2_SRC //Timer3 source
                    | OC_PWM_FAULT_PIN_DISABLE;  //PWM, no fault detection
    #endif
}

void motor_config_output_compare3(void) {
    T2CONbits.TON = 0;          //disable Timer when configuring Output compare
    //CONFIG_OC1_TO_RP(RB4_RP);   //map OC1 to RB4
    // OC3CONbits.OCM2 = 0b110;
    OC3CONbits.OCM = 0b110; // This is possibly incorrect and may need the line above it to work, but OCM2 is 1 bit, OCM is the 3 bit register

    OC3RS = 0;  //clear both registers
    OC3R = 0;
    #ifdef OC3CON2
        //turn on the compare toggle mode using Timer3
        OC3CON1 = OC_TIMER2_SRC |     //Timer3 source
                    OC_PWM_CENTER_ALIGN;  //PWM
        OC3CON2 = OC_SYNCSEL_TIMER2;   //synchronize to timer3
    #else
        //older families, this PWM mode is compatible with center-aligned, OC1R=0
        //as writes to OC1RS sets the pulse widith.
        OC3CON = OC_TIMER2_SRC      //Timer2 source
                | OC_PWM_FAULT_PIN_DISABLE;  //PWM, no fault detection
    #endif
}

void motor_config_output_compare4(void) {
    T3CONbits.TON = 0;          //disable Timer when configuring Output compare
    //CONFIG_OC1_TO_RP(RB4_RP);   //map OC1 to RB4
    // OC4CONbits.OCM2 = 0b110;
    OC4CONbits.OCM = 0b110; // This is possibly incorrect and may need the line above it to work, but OCM2 is 1 bit, OCM is the 3 bit register

    OC4RS = 0;  //clear both registers
    OC4R = 0;
    #ifdef OC4CON2
        //turn on the compare toggle mode using Timer3
        OC4CON1 = OC_TIMER3_SRC     //Timer3 source
                   | OC_PWM_CENTER_ALIGN;  //PWM
        OC4CON2 = OC_SYNCSEL_TIMER3;   //synchronize to timer3
    #else
        //older families, this PWM mode is compatible with center-aligned, OC1R=0
        //as writes to OC1RS sets the pulse widith.
        OC4CON = OC_TIMER3_SRC     //Timer3 source
                | OC_PWM_FAULT_PIN_DISABLE;  //PWM, no fault detection
    #endif
}

void motor_config_output_compare5(void) {
    T3CONbits.TON = 0;          //disable Timer when configuring Output compare
    //CONFIG_OC1_TO_RP(RB4_RP);   //map OC1 to RB4
    // OC5CONbits.OCM2 = 0b110;
    OC5CONbits.OCM = 0b110; // This is possibly incorrect and may need the line above it to work, but OCM2 is 1 bit, OCM is the 3 bit register

    OC5RS = 0;  //clear both registers
    OC5R = 0;
    #ifdef OC5CON2
        //turn on the compare toggle mode using Timer3
        OC5CON1 = OC_TIMER3_SRC    //Timer3 source
                    | OC_PWM_CENTER_ALIGN;  //PWM
        OC5CON2 = OC_SYNCSEL_TIMER3;   //synchronize to timer3
    #else
        //older families, this PWM mode is compatible with center-aligned, OC1R=0
        //as writes to OC1RS sets the pulse widith.
        OC5CON = OC_TIMER3_SRC     //Timer3 source
               | OC_PWM_FAULT_PIN_DISABLE;  //PWM, no fault detection
    #endif
}

void _ISR _T2Interrupt(void) {
    _T2IF = 0;    //clear the timer interrupt bit
}

void _ISR _T3Interrupt(void) {
      _T3IF = 0;    //clear the timer interrupt bit
}

// Interrupt Service Routine for INT0 for right encoder
void _ISRFAST _INT0Interrupt (void) {
    _INT0IF = 0;    //clear the interrupt bit
    process_right_rotary_data();
}

// Interrupt Service Routine for INT1 for left encoder
void _ISRFAST _INT1Interrupt (void) {
    _INT1IF = 0;    //clear the interrupt bit
    process_left_rotary_data();
}

///////////////////////////////////////////////
//
// Motor primitives
//
///////////////////////////////////////////////

// Left motor primitive movements
void left_motor_reverse (float f_speed) {
    float f_duty = (f_speed/100.0) * LEFT_MOTOR_ADJUSTMENT;

    LIN1_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T2CONbits));
    LIN2_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T2CONbits)) * (1-f_duty);

    u8_leftMotorDirection = BACKWARD_MOVEMENT;
}

void left_motor_fwd (float f_speed) {
    float f_duty = (f_speed/100.0) * LEFT_MOTOR_ADJUSTMENT;

    LIN1_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T2CONbits)) * (1-f_duty);
    LIN2_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T2CONbits));

    u8_leftMotorDirection = FORWARD_MOVEMENT;
}

void left_motor_stop() {
    LIN1_PULSE = 0;
    LIN2_PULSE = 0;

    u8_leftMotorDirection = STOPPED;
}

// Right motor primitive movements
void right_motor_reverse (float f_speed) {
    float f_duty = (f_speed/100.0) * RIGHT_MOTOR_ADJUSTMENT;

    RIN1_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T3CONbits));
    RIN2_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T3CONbits)) * (1-f_duty);

    u8_rightMotorDirection = BACKWARD_MOVEMENT;
}

void right_motor_fwd (float f_speed) {
    float f_duty = (f_speed/100.0) * RIGHT_MOTOR_ADJUSTMENT;

    RIN1_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T3CONbits)) * (1-f_duty);
    RIN2_PULSE = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T3CONbits));

    u8_rightMotorDirection = FORWARD_MOVEMENT;
}

void right_motor_stop() {
    RIN1_PULSE = 0;
    RIN2_PULSE = 0;

    u8_rightMotorDirection = STOPPED;
}

// Process the right encoder's data
void process_right_rotary_data() {
    // Increment if moving forward
    if (u8_rightMotorDirection == FORWARD_MOVEMENT) {
        i16_rightCounterROT++;
        if ((get_right_motor_location() >= f_rightTargetPosition) && (u8_rightAtTarget == 0)){
            u8_rightAtTarget = 1;
            right_motor_stop();
            if ((u8_routineBlock == 1) && (u8_leftAtTarget == 1)) {
                u8_routineBlock = 0;
            }
        }
    }

    // Decrement if moving backwards
    if (u8_rightMotorDirection == BACKWARD_MOVEMENT) {
        i16_rightCounterROT--;
        if ((get_right_motor_location() <= f_rightTargetPosition) && (u8_rightAtTarget == 0)){
            u8_rightAtTarget = 1;
            right_motor_stop();
            if ((u8_routineBlock == 1) && (u8_leftAtTarget == 1)) {
                u8_routineBlock = 0;
            }
        }
    }

    // Reset counter to 0 we've reached max and increment revolution count
    if (i16_rightCounterROT > ROT_MAX) {
        i16_rightCounterROT = 0;
        i16_rightRevolutionCount++;
    }

    // Reset counter to max we've reached 0 and decrement revolution count
    if (i16_rightCounterROT < 0 ) {
        i16_rightCounterROT = ROT_MAX;
        i16_rightRevolutionCount--;
    }

    // Check for a routine
    if (u8_routineBlock == 0) {
        check_for_routine();
    }
}

// Process the left encoder's data
void process_left_rotary_data() {
    // Increment if moving forward
    if (u8_leftMotorDirection == FORWARD_MOVEMENT) {
        i16_leftCounterROT++;

        if ((get_left_motor_location() >= f_leftTargetPosition) && (u8_leftAtTarget == 0)) {
            u8_leftAtTarget = 1;
            left_motor_stop();
            if ((u8_routineBlock == 1) && (u8_rightAtTarget == 1)) {
                u8_routineBlock = 0;
                #ifdef DEBUG_BUILD
                printf("Finished routine\n");
                #endif
            }
        }
    }

    // Decrement if moving backwards
    if (u8_leftMotorDirection == BACKWARD_MOVEMENT) {
        i16_leftCounterROT--;

        if ((get_left_motor_location() <= f_leftTargetPosition) && (u8_leftAtTarget == 0)) {
            u8_leftAtTarget = 1;
            left_motor_stop();
            if ((u8_routineBlock == 1) && (u8_rightAtTarget == 1)) {
                u8_routineBlock = 0;
                #ifdef DEBUG_BUILD
                printf("Finished routine\n");
                #endif
            }
        }
    }

    // Reset counter to 0 we've reached max and increment revolution count
    if (i16_leftCounterROT > ROT_MAX) {
        i16_leftCounterROT = 0;
        i16_leftRevolutionCount++;
    }

    // Reset counter to max we've reached 0 and decrement revolution count
    if (i16_leftCounterROT < 0 ) {
        i16_leftCounterROT = ROT_MAX;
        i16_leftRevolutionCount--;
    }

    // Check for a routine
    if (u8_routineBlock == 0) {
        check_for_routine();
    }
}

// Get the current location of the right motor
float get_right_motor_location() {
    float f_fractionPart;
    float f_total;

    f_fractionPart = (1.0 * i16_rightCounterROT)/(1.0 * ROT_MAX);

    // Located in the positive direction
    if (i16_rightRevolutionCount > 0) {
        f_total = i16_rightRevolutionCount + f_fractionPart;
    }
    // Located in the negative direction
    else if (i16_rightRevolutionCount < 0){
        f_total = i16_rightRevolutionCount - f_fractionPart;
    }
    // At zero so we have to look at our movement
    else {
        if (u8_rightMotorDirection == FORWARD_MOVEMENT) {
            f_total = 0 + f_fractionPart;
        }
        else {
            f_total = 0 - f_fractionPart;
        }
    }
    return f_total;
}

// Get the current location of the left motor
float get_left_motor_location() {
    float f_fractionPart;
    float f_total;

    f_fractionPart = (1.0 * i16_leftCounterROT)/(1.0 * ROT_MAX);

    // Located in the positive direction
    if (i16_leftRevolutionCount > 0) {
        f_total = i16_leftRevolutionCount + f_fractionPart;
    }
    // Located in the negative direction
    else if (i16_leftRevolutionCount < 0){
        f_total = i16_leftRevolutionCount - f_fractionPart;
    }
    // At zero so we have to look at our movement
    else {
        if (u8_leftMotorDirection == FORWARD_MOVEMENT) {
            f_total = 0 + f_fractionPart;
        }
        else {
            f_total = 0 - f_fractionPart;
        }
    }
    return f_total;
}

///////////////////////////////////////////////
//
// Motor usage
//
///////////////////////////////////////////////

// Stop robot
void motors_stop(void) {
    left_motor_stop();
    right_motor_stop();
}

// Turn robot right
void motors_turn_right(float f_speed) {
    right_motor_reverse(f_speed);
    left_motor_fwd(f_speed);
}

// Turn robot left
void motors_turn_left(float f_speed) {
    right_motor_fwd(f_speed);
    left_motor_reverse(f_speed);
}

// Move robot forward
void motors_move_forward(float f_speed) {
    right_motor_fwd(f_speed);
    left_motor_fwd(f_speed);
}

// Move robot in reverse
void motors_move_reverse(float f_speed) {
    right_motor_reverse(f_speed);
    left_motor_reverse(f_speed);
}

// Move right motor by revolutions
void move_right_motor_by_revolutions(float f_revolutions, float f_speed) {
    float f_currentPosition;

    f_currentPosition = get_right_motor_location();
    f_rightTargetPosition = f_currentPosition + (f_revolutions * RIGHT_ENCODER_ADJUSTMENT);
    u8_rightAtTarget = 0;

    if (f_revolutions > 0) {
        right_motor_fwd(f_speed);
    }
    else if (f_revolutions < 0) {
        right_motor_reverse(f_speed);
    }
}

// Move left motor by revolutions
void move_left_motor_by_revolutions(float f_revolutions, float f_speed) {
    float f_currentPosition;

    f_currentPosition = get_left_motor_location();
    f_leftTargetPosition = f_currentPosition + (f_revolutions * LEFT_ENCODER_ADJUSTMENT);
    u8_leftAtTarget = 0;

    if (f_revolutions > 0) {
        left_motor_fwd(f_speed);
    }
    else if (f_revolutions < 0) {
        left_motor_reverse(f_speed);
    }
}

// Move right motor by mm
void move_right_motor_by_distance(float f_distance, float f_speed) {
    move_right_motor_by_revolutions(f_distance/WHEEL_CIRCUMFERENCE, f_speed);
}

// Move left motor by mm
void move_left_motor_by_distance(float f_distance, float f_speed) {
    move_left_motor_by_revolutions(f_distance/WHEEL_CIRCUMFERENCE, f_speed);
}

// Move entire robot by revolutions
void move_by_revolutions(float f_revolutions, float f_speed) {
    move_right_motor_by_revolutions(f_revolutions, f_speed);
    move_left_motor_by_revolutions(f_revolutions, f_speed);
}

// Move entire robot by mm
void move_by_distance(float f_distance, float f_speed) {
    move_right_motor_by_distance(f_distance, f_speed);
    move_left_motor_by_distance(f_distance, f_speed);
}

void set_motors_pid(int16_t i16_leftSpeed, int16_t i16_rightSpeed, uint8_t u8_direction) {
    float f_leftPercentage;
    float f_rightPercentage;

    f_leftPercentage = (i16_leftSpeed*100.0/MOTOR_PWM_PERIOD);
    f_rightPercentage = (i16_rightSpeed*100.0/MOTOR_PWM_PERIOD);

    if (f_leftPercentage > LEFT_MAX_SPEED ) {
        f_leftPercentage = LEFT_MAX_SPEED;
    }
    if (f_rightPercentage > RIGHT_MAX_SPEED ) {
        f_rightPercentage = RIGHT_MAX_SPEED;
    }
    if (f_leftPercentage < 0) {
        f_leftPercentage = 0;
    }
    if (f_rightPercentage < 0) {
        f_rightPercentage = 0;
    }

    if (u8_direction == FORWARD_MOVEMENT) {
        left_motor_fwd(f_leftPercentage);
        right_motor_fwd(f_rightPercentage);
    } else if (u8_direction == BACKWARD_MOVEMENT) {
        left_motor_reverse(f_leftPercentage);
        right_motor_reverse(f_rightPercentage);
    }
}

///////////////////////////////////////////////
//
// Motor routines
//
///////////////////////////////////////////////

// Prepare robot for 90 degree turn by time
void prepare_for_90_degree_turn(float f_speed) {
    move_by_distance(PREPARE_90_TURN_DISTANCE, f_speed);
}

// Turn robot 90 degrees
void turn_90_degrees(float f_speed, uint8_t u8_direction) {
    if (u8_direction == RIGHT_DIRECTION) {
        move_right_motor_by_revolutions((-1.0 * DEGREE_90_TURN_REVS), f_speed);
        move_left_motor_by_revolutions(DEGREE_90_TURN_REVS, f_speed);
    }
    if (u8_direction == LEFT_DIRECTION) {
        move_right_motor_by_revolutions(DEGREE_90_TURN_REVS, f_speed);
        move_left_motor_by_revolutions((-1.0 * DEGREE_90_TURN_REVS), f_speed);
    }
}

// Prepare for a curved turn
void prepare_for_curve_turn(float f_speed) {
    move_by_distance(PREPARE_CURVE_TURN_DISTANCE, f_speed);
}

// Turn robot 90 degrees in a curve
void turn_90_degree_curve(float f_speed, uint8_t u8_direction) {
    if (u8_direction == RIGHT_DIRECTION) {
        move_left_motor_by_revolutions((DEGREE_90_CURVE_TURN_REVS), f_speed*1.5);
    }
    if (u8_direction == LEFT_DIRECTION) {
        move_right_motor_by_revolutions((DEGREE_90_CURVE_TURN_REVS), f_speed*1.5);
    }
}
// Reverse after turning 90 degrees to compensate for the preparing movement
void finish_90_degree_turn(float f_speed) {
    move_by_distance((-1.0 * PREPARE_90_TURN_DISTANCE*0.8), f_speed);
}

// Prepare for a reverse 90 degree turn by moving forward
void prepare_for_reverse_90_degree_turn(float f_speed) {
    move_by_distance(PREPARE_90_TURN_DISTANCE, f_speed);
}

// Reverse after reverse turning 90 degrees to compensate for the preparing movement
void finish_reverse_90_degree_turn(float f_speed) {
    move_by_distance((-1.0 * PREPARE_90_TURN_DISTANCE), f_speed);
}

void turn_180_degrees(float f_speed, uint8_t u8_direction) {
    if (u8_direction == RIGHT_DIRECTION) {
        move_right_motor_by_revolutions((-2.0 * DEGREE_90_TURN_REVS), f_speed);
        move_left_motor_by_revolutions((2.0 * DEGREE_90_TURN_REVS), f_speed);
    }
    if (u8_direction == LEFT_DIRECTION) {
        move_right_motor_by_revolutions((2.0 * DEGREE_90_TURN_REVS), f_speed);
        move_left_motor_by_revolutions((-2.0 * DEGREE_90_TURN_REVS), f_speed);
    }
}

void finish_180_degree_turn(float f_speed) {
    move_by_distance((-1.0 * FINISH_180_TURN_DISTANCE), f_speed);
}

void back_away_from_box(float f_speed) {
    move_by_distance((-2.0 * LINE_WIDTH), f_speed);
}

void move_past_start_box(float f_speed) {
    move_by_distance(START_BOX_DRIVE_DISTANCE, f_speed);
}

void move_past_branch(float f_speed) {
    move_by_distance(LINE_WIDTH, f_speed);
}

void move_into_box(float f_speed) {
    move_by_distance(MOVE_INTO_BOX_DISTANCE, f_speed);
}

void final_game_preparations(uint8_t u8_game) {
    // Move forward different distances based on which game it is
    switch(u8_game) {
        case SIMON:
            enqueue(&navigationRoutineQueue, PREPARE_SIMON);
            break;
        case RUBIKS:
            enqueue(&navigationRoutineQueue, PREPARE_RUBIKS);
            break;
        case ETCH:
            enqueue(&navigationRoutineQueue, PREPARE_ETCH);
            break;
        case CARD:
            break;
    }
    // Initiate this
    check_for_routine();

    // Wait until this finishes
    block_until_all_routines_done();
}

void prepare_to_leave_game(uint8_t u8_game) {
    // Get out of the box and turn around, different for each game
    switch(u8_game) {
        case SIMON:
            #ifdef DEBUG_BUILD
            printf("Leaving Simon\n");
            #endif
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, BACK_AWAY_FROM_GAME_DISTANCE);
            enqueue(&navigationRoutineQueue, TURN_180);
            enqueue(&navigationRoutineQueue, FINISH_180_TURN);
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, FINAL_LEAVING_SIMON);
            break;
        case RUBIKS:
            #ifdef DEBUG_BUILD
            printf("Leaving Rubiks\n");
            #endif
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, BACK_AWAY_FROM_GAME_DISTANCE);
            enqueue(&navigationRoutineQueue, TURN_180);
            enqueue(&navigationRoutineQueue, FINISH_180_TURN);
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, FINAL_LEAVING_RUBIKS);
            break;
        case ETCH:
            #ifdef DEBUG_BUILD
            printf("Leaving Etch\n");
            #endif
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, BACK_AWAY_FROM_GAME_DISTANCE);
            enqueue(&navigationRoutineQueue, TURN_180);
            enqueue(&navigationRoutineQueue, FINISH_180_TURN);
            enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
            enqueue(&navigationMoveDistanceQueue, FINAL_LEAVING_ETCH);
            break;
        case CARD:
            #ifdef DEBUG_BUILD
            printf("Leaving Card\n");
            #endif
            enqueue(&navigationRoutineQueue, TURN_180);
            enqueue(&navigationRoutineQueue, FINISH_180_TURN);
            break;
    }
    // Initiate this
    check_for_routine();

    // Wait until this finishes
    block_until_all_routines_done();
}

void handle_routine(uint8_t routine) {
    uint16_t u16_distance;
    int16_t i16_distance;

    switch(routine) {
        case RIGHT_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: turning right\n");
            #endif
            turn_90_degrees(BASE_SPEED, RIGHT_DIRECTION);
            break;
        case LEFT_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: turning left\n");
            #endif
            turn_90_degrees(BASE_SPEED, LEFT_DIRECTION);
            break;
        case PREPARE_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: preparing for normal turn\n");
            #endif
            prepare_for_90_degree_turn(BASE_SPEED);
            break;
        case FINISH_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: finishing normal turn\n");
            #endif
            finish_90_degree_turn(BASE_SPEED);
            break;
        case PREPARE_REVERSE_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: preparing for reverse turn\n");
            #endif
            prepare_for_reverse_90_degree_turn(BASE_SPEED);
            break;
        case FINISH_REVERSE_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: finishing reverse turn\n");
            #endif
            finish_reverse_90_degree_turn(BASE_SPEED);
            break;
        case MOVE_INTO_BOX:
            #ifdef DEBUG_BUILD
            printf("Routine: moving into box\n");
            #endif
            move_into_box(BASE_SPEED);
            break;
        case TURN_180:
            #ifdef DEBUG_BUILD
            printf("Routine: 180 turn\n");
            #endif
            turn_180_degrees(BASE_SPEED, RIGHT_DIRECTION);
            break;
        case FINISH_180_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: finishing 180 turn\n");
            #endif
            finish_180_degree_turn(BASE_SPEED);
            break;
        case BACK_AWAY_FROM_BOX:
            #ifdef DEBUG_BUILD
            printf("Routine: backing away from game box\n");
            #endif
            back_away_from_box(BASE_SPEED);
            break;
        case MOVE_PAST_START_BOX:
            #ifdef DEBUG_BUILD
            printf("Routine: moving past start box\n");
            #endif
            move_past_start_box(BASE_SPEED);
            break;
        case MOVE_PAST_BRANCH:
            #ifdef DEBUG_BUILD
            printf("Routine: moving past branch\n");
            #endif
            move_past_branch(BASE_SPEED);
            break;
        case MOVE_FORWARD_DISTANCE:
            u16_distance = dequeue(&navigationMoveDistanceQueue);
            i16_distance = u16_distance;
            #ifdef DEBUG_BUILD
            printf("Routine: moving forward %u mm\n", u16_distance);
            #endif
            move_by_distance(i16_distance*1.0, BASE_SPEED);
            break;
        case MOVE_REVERSE_DISTANCE:
            u16_distance = dequeue(&navigationMoveDistanceQueue);
            i16_distance = 0 - u16_distance;
            #ifdef DEBUG_BUILD
            printf("Routine: moving reverse %u mm\n", u16_distance);
            #endif
            move_by_distance(i16_distance*1.0, BASE_SPEED);
            break;
        case RIGHT_CURVE_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: turning right curve\n");
            #endif
            turn_90_degree_curve(BASE_SPEED, RIGHT_DIRECTION);
            break;
        case LEFT_CURVE_TURN:
            #ifdef DEBUG_BUILD
            printf("Routine: turning left curve\n");
            #endif
            turn_90_degree_curve(BASE_SPEED, LEFT_DIRECTION);
            break;
        case PREPARE_TURN_CURVE:
            #ifdef DEBUG_BUILD
            printf("Routine: prepare for curve turn\n");
            #endif
            prepare_for_curve_turn(BASE_SPEED);
            break;
        case PLAY_GAME_PAUSE:
            #ifdef DEBUG_BUILD
            printf("Routine: game pause\n");
            #endif
            break;
        case PREPARE_SIMON:
            #ifdef DEBUG_BUILD
            printf("Routine: prepare for Simon\n");
            #endif
            move_by_distance(MOVE_INTO_SIMON_DISTANCE*1.0, BASE_SPEED);
            break;
        case PREPARE_ETCH:
            #ifdef DEBUG_BUILD
            printf("Routine: prepare for Etch\n");
            #endif
            move_by_distance(MOVE_INTO_ETCH_DISTANCE*1.0, BASE_SPEED);
            break;
        case PREPARE_RUBIKS:
            #ifdef DEBUG_BUILD
            printf("Routine: prepare for Rubiks\n");
            #endif
            move_by_distance(MOVE_INTO_RUBIKS_DISTANCE*1.0, BASE_SPEED);
            break;
        case CROSS_FINISH_LINE:
            #ifdef DEBUG_BUILD
            printf("Routine: crossing finish line\n");
            #endif
            move_by_distance(CROSS_FINISH_LINE_DISTANCE*1.0, BASE_SPEED);
            break;
        case SHORT_BACKUP:
            #ifdef DEBUG_BUILD
            printf("Routine: moving back short\n");
            #endif
            move_by_distance(SHORT_BACKUP_DISTANCE*-1.0, BASE_SPEED);
            break;
        case MEDIUM_BACKUP:
            #ifdef DEBUG_BUILD
            printf("Routine: moving back medium\n");
            #endif
            move_by_distance(MEDIUM_BACKUP_DISTANCE*-1.0, BASE_SPEED);
            break;
        case LONG_BACKUP:
            #ifdef DEBUG_BUILD
            printf("Routine: moving back long\n");
            #endif
            move_by_distance(LONG_BACKUP_DISTANCE*-1.0, BASE_SPEED);
            break;
        default:
            break;
    }
}

uint8_t check_for_routine() {
    static uint8_t u8_sent99;

    if (queue_is_empty(navigationRoutineQueue)) {
        if (u8_sent99 != 1) {
            send_display_number(NAVIGATING_NUMBER);
            u8_sent99 = 1;
        }
        return 0;
    } else {
        u8_sent99 = 0;
        u8_routineBlock = 1;
        u8_currentRoutine = dequeue(&navigationRoutineQueue);
        handle_routine(u8_currentRoutine);
        send_navigation_routine_number(u8_currentRoutine);
        return u8_currentRoutine;
    }
}

void clear_routines() {
    // Stop the robot
    motors_stop();

    // Clear out the nav queue
    clear_queue(&navigationRoutineQueue);
    clear_queue(&navigationMoveDistanceQueue);

    // Reset targets
    u8_leftAtTarget = 1;
    u8_rightAtTarget = 1;
    u8_routineBlock = 0;
}

void set_base_speed(float f_newBase) {
    BASE_SPEED = f_newBase;
}

void block_until_all_routines_done() {
    while(1) {
        if (u8_routineBlock == 0) {
            return;
        }
    }
}
