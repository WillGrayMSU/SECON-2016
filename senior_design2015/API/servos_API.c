/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: servos_API.c
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

#include "servos_API.h"

static volatile uint16_t    au16_servoPWidths1[NUM_SERVOS1];      // Array to hold a servo pulse widths
volatile uint8_t            u8_currentServo1 = 0;                // Servo currently being controlled
volatile uint8_t            u8_servoEdge1 = 1;                   // 1 = RISING, 0 = FALLING

static volatile uint16_t    au16_servoPWidths2[NUM_SERVOS2];    // Array to hold a servo pulse widths
volatile uint8_t            u8_currentServo2 = 0;                // Servo currently being controlled
volatile uint8_t            u8_servoEdge2 = 1;                   // 1 = RISING, 0 = FALLING

volatile uint16_t           u16_slotWidthTicks = 0;             // Slot width in ticks

///////////////////////////////////////////////
//
// Servo config
//
///////////////////////////////////////////////

void servo_init() {
    // All servo outputs low initially
    RUBIKS_PLATFORM_PIN = 0;
    RUBIKS_TWIST_PIN = 0;

    SIMON_YELLOW_PIN = 0;
    SIMON_BLUE_PIN = 0;
    SIMON_RED_PIN = 0;
    SIMON_GREEN_PIN = 0;

    ARM_POSITION_PIN = 0;
    ARM_SLIDE_LEFT_PIN = 0;
    ARM_SLIDE_RIGHT_PIN = 0;

    ETCH_VERTICAL_PIN = 0;
    ETCH_HORIZ_PIN = 0;

    // Configure timers and output comparators
    config_servo_timer3();
    config_output_capture1();

    config_servo_timer2();
    config_output_capture2();

    u16_slotWidthTicks = usToU16Ticks(SLOT_WIDTH, getTimerPrescale(T3CONbits));

    DELAY_MS(500);

    // Config all the servo outputs
    CONFIG_RD2_AS_DIG_OUTPUT();
    CONFIG_RD3_AS_DIG_OUTPUT();
    CONFIG_RD4_AS_DIG_OUTPUT();
    CONFIG_RD5_AS_DIG_OUTPUT();
    CONFIG_RD6_AS_DIG_OUTPUT();
    CONFIG_RD7_AS_DIG_OUTPUT();
    CONFIG_RD8_AS_DIG_OUTPUT();
    CONFIG_RD9_AS_DIG_OUTPUT();
    CONFIG_RD10_AS_DIG_OUTPUT();
    CONFIG_RD11_AS_DIG_OUTPUT();
    CONFIG_RB13_AS_DIG_OUTPUT();

    // Turn on the timers
    T3CONbits.TON = 1;
    T2CONbits.TON = 1;
}

void config_servo_timer3(void) {
    // Config bits
    T3CON = T3_OFF
            | T3_IDLE_CON
            | T2_32BIT_MODE_OFF
            | T3_GATE_OFF
            | T3_SOURCE_INT
            | T3_PS_1_256 ;  //1 tick = 1.6 us at FCY=40 MHz
    PR3 = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T3CONbits)) - 1;
    TMR3  = 0;       //clear timer3 value
}

void _ISR _OC1Interrupt(void) {
    _OC1IF = 0;

    // Change the servo's value
    set_servo_output(u8_currentServo1, u8_servoEdge1);

    // Schedule next interrupt
    if (u8_servoEdge1 == 1) {  // Rising edge
       // Next interrupt occurs after pulse width has elapsed
       OC1R = OC1R + au16_servoPWidths1[u8_currentServo1];
       u8_servoEdge1 = 0;     // Change to falling edge

    } else { // Falling edge
        // Next interrupt occurs at beginning of next slot
        if (u8_currentServo1 != NUM_SERVOS1 - 1) {
            OC1R = u16_slotWidthTicks*(u8_currentServo1+1);
        } else { // Last servo
            OC1R = 0;
        }
        u8_servoEdge1 = 1;     // Change to rising edge
        u8_currentServo1++;
        if (u8_currentServo1 == NUM_SERVOS1)
            u8_currentServo1 = 0;
    }
}

void config_output_capture1(void) {
    T3CONbits.TON = 0;       //disable Timer when configuring Output compare
    OC1RS = 0;  //clear both registers
    OC1R  =  0;  //initialize to 0
    //turn on the compare toggle mode using Timer3
    OC1CON =    OC_TIMER3_SRC |      //Timer3 source
                OC_TOGGLE_PULSE;     //single compare toggle, just care about compare event
    _OC1IF = 0;
    _OC1IP = 1;
    _OC1IE = 1;    //enable the OC1 interrupt
}

void config_servo_timer2(void) {
    // Config bits
    T2CON = T2_OFF
            | T2_IDLE_CON
            | T2_32BIT_MODE_OFF
            | T2_GATE_OFF
            | T2_SOURCE_INT
            | T2_PS_1_256 ;  //1 tick = 1.6 us at FCY=40 MHz
    PR2 = usToU16Ticks(MOTOR_PWM_PERIOD, getTimerPrescale(T2CONbits)) - 1;
    TMR2 = 0;       //clear timer2 value
}

void _ISR _OC2Interrupt(void) {
    _OC2IF = 0;

    // Change the servo's value
    set_servo_output(u8_currentServo2 + 5, u8_servoEdge2);

    // Schedule next interrupt
    if (u8_servoEdge2 == 1) {  // Rising edge
       // Next interrupt occurs after pulse width has elapsed
       OC2R = OC2R + au16_servoPWidths2[u8_currentServo2];
       u8_servoEdge2 = 0;     // Change to falling edge

    } else { // Falling edge
        // Next interrupt occurs at beginning of next slot
        if (u8_currentServo2 != NUM_SERVOS2 - 1) {
            OC2R = u16_slotWidthTicks*(u8_currentServo2+1);
        } else { // Last servo
            OC2R = 0;
        }
        u8_servoEdge2 = 1;     // Change to rising edge
        u8_currentServo2++;
        if (u8_currentServo2 == NUM_SERVOS2)
            u8_currentServo2 = 0;
    }
}

void config_output_capture2(void) {
    T2CONbits.TON = 0;       //disable Timer when configuring Output compare
    OC2RS = 0;  //clear both registers
    OC2R  =  0;  //initialize to 0

    //turn on the compare toggle mode using Timer2
    OC2CON =    OC_TIMER2_SRC |      //Timer2 source
                OC_TOGGLE_PULSE;     //single compare toggle, just care about compare event
    _OC2IF = 0;
    _OC2IP = 1;
    _OC2IE = 1;    //enable the OC2 interrupt
}

///////////////////////////////////////////////
//
// Servo primatives
//
///////////////////////////////////////////////

void set_servo_output (uint8_t u8_servo, uint8_t u8_val) {
    switch (u8_servo) {
        case 0:
            ETCH_VERTICAL_PIN = u8_val;
            break;
        case 1:
            ETCH_HORIZ_PIN = u8_val;
            break;
        case 2:
            RUBIKS_PLATFORM_PIN = u8_val;
            break;
        case 3:
            RUBIKS_TWIST_PIN = u8_val;
            break;
        case 4:
            ARM_POSITION_PIN = u8_val;
            break;
        case 5:
            SIMON_YELLOW_PIN = u8_val;
            break;
        case 6:
            SIMON_BLUE_PIN = u8_val;
            break;
        case 7:
            SIMON_RED_PIN = u8_val;
            break;
        case 8:
            SIMON_GREEN_PIN = u8_val;
            break;
        case 9:
            ARM_SLIDE_LEFT_PIN = u8_val;
            break;
        case 10:
            ARM_SLIDE_RIGHT_PIN = u8_val;
        default:
            break;
    }
}

///////////////////////////////////////////////
//
// Servo usage
//
///////////////////////////////////////////////

void turn_servo_by_pulse(servoIDs id, uint16_t pulseWidth) {
    _OC1IE = 0; //disable the interrupt while changing
    _OC2IE = 0;

    if (id == ETCH_VERTICAL || id == ETCH_HORIZ || id == RUBIKS_PLATFORM || id == RUBIKS_TWIST || id == ARM_POSITION) {
        au16_servoPWidths1[id] = usToU16Ticks(pulseWidth, getTimerPrescale(T3CONbits));
    } else {
        au16_servoPWidths2[id-5] = usToU16Ticks(pulseWidth, getTimerPrescale(T2CONbits));
    }

    _OC1IE = 1;
    _OC2IE = 1;
    DELAY_MS(100);
}

void stop_servo (servoIDs id) {
    turn_servo_by_pulse(id, NEUTRAL_PULSE);
}

void step_servo (uint8_t direction, servoIDs id) {
    if (direction == 0) {
        turn_servo_CW_degrees(id, STEP_SIZE);
    } else {
        turn_servo_CCW_degrees(id, STEP_SIZE);
    }
}

void turn_servo_CW_degrees(servoIDs id, float degrees) {
    uint16_t u16_delayTime = (degrees/360) * CONT_CLOCK_FULL_ROTATION_TIME;

    turn_servo_by_pulse(id, CLOCKWISE_PULSE);
    DELAY_MS(u16_delayTime);
    turn_servo_by_pulse(id, NEUTRAL_PULSE);
}

void turn_servo_CCW_degrees(servoIDs id, float degrees) {
    uint16_t u16_delayTime = (degrees/360) * CONT_COUNTER_FULL_ROTATION_TIME;

    turn_servo_by_pulse(id, COUNTER_PULSE);
    DELAY_MS(u16_delayTime);
    turn_servo_by_pulse(id, NEUTRAL_PULSE);
}

void turn_servo_to_degree(servoIDs id, float degree) {
    if (degree == 0) {
        turn_servo_by_pulse(id, 1000);
    } else {
        turn_servo_by_pulse(id, ((degree/180) * 1000) + 1000);
    }
}
