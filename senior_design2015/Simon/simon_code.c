/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: simon_code.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to record lights coming from the simon
* then push the correct buttons to play the game
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        10/5/2014               SECON 2015
*********************************************************************/

#include "simon_code.h"

#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

uint8_t u8_yellowLightPin = 3;
uint8_t u8_blueLightPin = 2;
uint8_t u8_redLightPin = 0;
uint8_t u8_greenLightPin = 1;

int16_t i16_ambientYellowValue = 0;
int16_t i16_ambientBlueValue = 0;
int16_t i16_ambientRedValue = 0;
int16_t i16_ambientGreenValue = 0;

int16_t i16_currentYellowValue = 0;
int16_t i16_currentBlueValue = 0;
int16_t i16_currentRedValue = 0;
int16_t i16_currentGreenValue = 0;

int16_t i16_yellowDifference = 0;
int16_t i16_blueDifference = 0;
int16_t i16_redDifference = 0;
int16_t i16_greenDifference = 0;

int16_t i16_yellowOnValue = 0;
int16_t i16_blueOnValue = 0;
int16_t i16_redOnValue = 0;
int16_t i16_greenOnValue = 0;

uint16_t u16_currentYellowPushPulse = 0;
uint16_t u16_currentBluePushPulse = 0;
uint16_t u16_currentRedPushPulse = 0;
uint16_t u16_currentGreenPushPulse = 0;
uint16_t u16_currentStartPushPulse = 0;

uint8_t au8_buttonArray[50];
uint8_t u8_roundNum = 1;
uint8_t u8_button;
uint8_t u8_started = 0;

volatile uint8_t u8_simonFinished = 0;
volatile uint16_t u16_milliSeconds = 0;

///////////////////////////////////////////////
//
// Simon usage
//
///////////////////////////////////////////////

void simon_init() {
    simon_retract_buttons();
    configTimer5();

    u16_currentYellowPushPulse = YELLOW_PUSH;
    u16_currentBluePushPulse = BLUE_PUSH;
    u16_currentRedPushPulse = RED_PUSH;
    u16_currentGreenPushPulse = GREEN_PUSH;
    u16_currentStartPushPulse = START_PUSH;
}

void play_simon() {
    display_draw_number(SIMON_NUMBER);
    #ifdef DEBUG_BUILD
    printf("\n*** Playing Simon ***\n");
    #endif

    // Grab the Simon and calibrate the sensors
    game_arm_pull_simon();
    simon_hover_buttons();
    DELAY_MS(PLATFORM_WAIT);
    platform_down();
    twist_rubiks_clock();
    DELAY_MS(PLATFORM_WAIT);
    calibrate_sensors(); // I don't think we need calibration with the new transistors

    // Stop playing if we exceed 5 rounds
    while (u8_roundNum < 5) {
        #ifdef DEBUG_BUILD
        printf("\nROUND %i\n", u8_roundNum);
        #endif

        // If it's the first round, hit the start button
        if (u8_roundNum == 1) {
            // Push the start button and record the colors
            u8_started = 1;
            simon_push_button(START_SIMON_BUTTON);
            record_colors(u8_roundNum);
            u8_started = 0;

            if (u8_simonFinished == 1) {
                #ifdef DEBUG_BUILD
                printf("Giving up\n");
                #endif

                twist_rubiks_counter();
                platform_up();
                DELAY_MS(PLATFORM_WAIT);
                simon_retract_buttons();
                return;
            }

            #ifdef DEBUG_BUILD
            printf("Starting Simon time\n");
            #endif

            u8_simonFinished = 0;
            u16_milliSeconds = 0;
            T5CONbits.TON = 1;     // Turn on the timer
            simon_hover_button(YELLOW_BUTTON);
        } else {
            // Record a certain number of colors
            record_colors(u8_roundNum);
        }
        // If we've surpassed 15 seconds, leave Simon
        if (u8_simonFinished) {
            #ifdef DEBUG_BUILD
            printf("Stopping Simon Time\n");
            #endif
            u8_simonFinished = 0;
            u16_milliSeconds = 0;
            u8_roundNum = 1;

            break;
        }
        // Play back the buttons
        DELAY_MS(500);
        play_buttons(u8_roundNum);
        u8_roundNum++;
    }

    // Let go of the simon and pull the arms back
    T5CONbits.TON = 0;
    twist_rubiks_counter();
    platform_up();
    DELAY_MS(PLATFORM_WAIT);
    simon_retract_buttons();
}

void play_simon_infinitely() {
    display_draw_number(SIMON_NUMBER);
    #ifdef DEBUG_BUILD
    printf("\n*** Playing Simon ***\n");
    #endif

    // Grab the Simon and calibrate the sensors
    game_arm_pull_simon();
    simon_hover_buttons();
    DELAY_MS(PLATFORM_WAIT);
    platform_rubiks();
    twist_rubiks_clock();
    DELAY_MS(PLATFORM_WAIT);
    calibrate_sensors(); // I don't think we need calibration with the new transistors

    // Stop playing if we exceed 5 rounds
    while (1) {
        #ifdef DEBUG_BUILD
        printf("\nROUND %i\n", u8_roundNum);
        #endif

        // If it's the first round, hit the start button
        if (u8_roundNum == 1) {
            // Push the start button and record the colors
            simon_push_button(START_SIMON_BUTTON);
            record_colors(u8_roundNum);

            u8_simonFinished = 0;
            u16_milliSeconds = 0;
            simon_hover_button(YELLOW_BUTTON);
        } else {
            // Record a certain number of colors
            record_colors(u8_roundNum);
        }
        // Play back the buttons
        DELAY_MS(500);
        play_buttons(u8_roundNum);
        u8_roundNum++;
    }

    // Let go of the simon and pull the arms back
    twist_rubiks_counter();
    DELAY_MS(PLATFORM_WAIT);
    platform_up();
    DELAY_MS(PLATFORM_WAIT);
    simon_retract_buttons();
}

///////////////////////////////////////////////
//
// Simon primitives
//
///////////////////////////////////////////////

void calibrate_sensors() {
    uint8_t i;
    int16_t i16_tempYellowValue = read_photo_cell(YELLOW_TRANS);
    int16_t i16_tempBlueValue = read_photo_cell(BLUE_TRANS);
    int16_t i16_tempRedValue = read_photo_cell(RED_TRANS);
    int16_t i16_tempGreenValue = read_photo_cell(GREEN_LIGHT);

    // Find the lowest value for each sensor over a short period of time
    for (i = 0; i < 50; i++) {
        i16_tempYellowValue = read_photo_cell(YELLOW_TRANS);
        i16_tempBlueValue = read_photo_cell(BLUE_TRANS);
        i16_tempRedValue = read_photo_cell(RED_TRANS);
        i16_tempGreenValue = read_photo_cell(GREEN_TRANS);

        if (i16_tempYellowValue > i16_ambientYellowValue) {
            i16_ambientYellowValue = i16_tempYellowValue;
        }
        if (i16_tempBlueValue > i16_ambientBlueValue) {
            i16_ambientBlueValue = i16_tempBlueValue;
        }
        if (i16_tempRedValue > i16_ambientRedValue) {
            i16_ambientRedValue = i16_tempRedValue;
        }
        if (i16_tempGreenValue > i16_ambientGreenValue) {
            i16_ambientGreenValue = i16_tempGreenValue;
        }
        DELAY_MS(RESPONSE_WAIT);
    }
    #ifdef DEBUG_BUILD
    printf("Calibrated\n");
    printf("Yellow: %i\n", i16_ambientYellowValue);
    printf("Blue: %i\n", i16_ambientBlueValue);
    printf("Red: %i\n", i16_ambientRedValue);
    printf("Green: %i\n", i16_ambientGreenValue);
    printf("\n");
    #endif
}

// Push buttons based on an array of recorded buttons
void play_buttons(uint8_t u8_numRounds) {
    uint8_t i;

    // Push buttons until we've pushed enough buttons
    for(i = 0; i < u8_numRounds; i++) {
        // If we've surpassed 15 seconds, leave Simon
        if (u8_simonFinished)
            return;

        #ifdef DEBUG_BUILD
        printf ("Button: %i\n", i+1);
        #endif
        DELAY_MS(500);

        // For each  button, push -> confirm on -> hover -> confirm off
        if (au8_buttonArray[i] == YELLOW_BUTTON) {
            #ifdef DEBUG_BUILD
            printf("Pressing Yellow\n");
            #endif

            simon_push_button(YELLOW_BUTTON);
            confirm_color(YELLOW_BUTTON);
            DELAY_MS(120);
            simon_hover_button(YELLOW_BUTTON);
            confirm_color_off(YELLOW_BUTTON);
        }
        else if (au8_buttonArray[i] == BLUE_BUTTON) {
            #ifdef DEBUG_BUILD
            printf("Pressing Blue\n");
            #endif

            simon_push_button(BLUE_BUTTON);
            confirm_color(BLUE_BUTTON);
            DELAY_MS(120);
            simon_hover_button(BLUE_BUTTON);
            confirm_color_off(BLUE_BUTTON);
        }
        else if (au8_buttonArray[i] == RED_BUTTON) {
            #ifdef DEBUG_BUILD
            printf("Pressing Red\n");
            #endif

            simon_push_button(RED_BUTTON);
            confirm_color(RED_BUTTON);
            DELAY_MS(120);
            simon_hover_button(RED_BUTTON);
            confirm_color_off(RED_BUTTON);
        }
        else if (au8_buttonArray[i] == GREEN_BUTTON) {
            #ifdef DEBUG_BUILD
            printf("Pressing Green\n");
            #endif

            simon_push_button(GREEN_BUTTON);
            confirm_color(GREEN_BUTTON);
            DELAY_MS(120);
            simon_hover_button(GREEN_BUTTON);
            confirm_color_off(GREEN_BUTTON);
        }
    }
}

// Records colors so that buttons can be pushed in the correct order
void record_colors(uint8_t u8_numberOfButtons) {
    // Record colors, adding the button to our array of buttons we'll push later
    uint8_t u8_detectedButtons;
    uint8_t u16_giveUpCounter = 0;

    #ifdef DEBUG_BUILD
    uint8_t i;
    #endif
    u8_detectedButtons = 0;

    // Keep looking for lights until we've seen enough
    while(u8_detectedButtons < u8_numberOfButtons) {
        // If we've surpassed 15 seconds, leave Simon
        if (u8_simonFinished) {
            return;
        }

        if (u8_started == 1) {
            u16_giveUpCounter++;

            if (u16_giveUpCounter >= 30) {
                u8_simonFinished = 1;
                return;
            }
        }

        // For each light, get the transistor value, and compare it to the light threshold
        i16_currentYellowValue = read_photo_cell(YELLOW_TRANS);
        i16_currentBlueValue = read_photo_cell(BLUE_TRANS);
        i16_currentRedValue = read_photo_cell(RED_TRANS);
        i16_currentGreenValue = read_photo_cell(GREEN_TRANS);

        i16_yellowDifference = i16_currentYellowValue - i16_ambientYellowValue;
        i16_blueDifference = i16_currentBlueValue - i16_ambientBlueValue;
        i16_redDifference = i16_currentRedValue - i16_ambientRedValue;
        i16_greenDifference = i16_currentGreenValue - i16_ambientGreenValue;

        // If we encounter a difference larger than the threshold, add the button to our array and wait until it's off
        if (i16_yellowDifference > YELLOW_LIGHT_THRESH_HOLD)
        {
            #ifdef DEBUG_BUILD
            printf("Detected Yellow: %i\n", i16_currentYellowValue);
            #endif

            au8_buttonArray[u8_detectedButtons] = YELLOW_BUTTON;
            u8_detectedButtons++;
            DELAY_MS(50);
            confirm_color_off(YELLOW_BUTTON);
        }
        else if (i16_blueDifference > BLUE_LIGHT_THRESH_HOLD)
        {
            #ifdef DEBUG_BUILD
            printf("Detected Blue: %i\n", i16_currentBlueValue);
            #endif

            au8_buttonArray[u8_detectedButtons] = BLUE_BUTTON;
            u8_detectedButtons++;
            DELAY_MS(50);
            confirm_color_off(BLUE_BUTTON);
        }
        else if (i16_redDifference > RED_LIGHT_THRESH_HOLD)
        {
            #ifdef DEBUG_BUILD
            printf("Detected Red: %i\n", i16_currentRedValue);
            #endif

            au8_buttonArray[u8_detectedButtons] = RED_BUTTON;
            u8_detectedButtons++;
            DELAY_MS(50);
            confirm_color_off(RED_BUTTON);
        }
        else if (i16_greenDifference > GREEN_LIGHT_THRESH_HOLD)
        {
            #ifdef DEBUG_BUILD
            printf("Detected Green: %i\n", i16_currentGreenValue);
            #endif

            au8_buttonArray[u8_detectedButtons] = GREEN_BUTTON;
            u8_detectedButtons++;
            DELAY_MS(50);
            confirm_color_off(GREEN_BUTTON);
        }
        //short DELAY_MS for faster response to light.
        DELAY_MS(RESPONSE_WAIT);
        if (u8_roundNum == 1) {
            u16_currentStartPushPulse -= PULSE_INCREASE;
            simon_push_button(START_SIMON_BUTTON);
        }
    }

    #ifdef DEBUG_BUILD
    printf ("\nOur buttons\n");
    for (i = 0; i < u8_numberOfButtons; i++) {
        if (au8_buttonArray[i] == YELLOW_BUTTON){
            printf("Button %i: Yellow\n", i+1);
        }
        else if (au8_buttonArray[i] == BLUE_BUTTON){
            printf("Button %i: Blue\n", i+1);
        }
        else if (au8_buttonArray[i] == RED_BUTTON){
            printf("Button %i: Red\n", i+1);
        }
        else if (au8_buttonArray[i] == GREEN_BUTTON){
            printf("Button %i: Green\n", i+1);
        }
    }
    printf("\n");
    #endif
}

// Blocking function until color comes on
void confirm_color(uint8_t u8_color) {
    // Confirm that the given color turns on
    #ifdef DEBUG_BUILD
    printf("Waiting to turn on\n");
    #endif

    // Depending on which color we're looking for, enter a loop, where we continue to read adc until the threshold is exceeded
    if (u8_color == YELLOW_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentYellowValue = read_photo_cell(YELLOW_TRANS);
            i16_yellowDifference = i16_currentYellowValue - i16_ambientYellowValue;

            if (i16_yellowDifference > YELLOW_LIGHT_THRESH_HOLD) {
                #ifdef DEBUG_BUILD
                printf("Yellow confirmed on: %i\n", i16_currentYellowValue);
                #endif

                return;
            }

            // Try and push harder
            // u16_currentYellowPushPulse -= PULSE_INCREASE;
            increase_button_push(YELLOW_BUTTON);
            simon_push_button(YELLOW_BUTTON);

            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    } else if (u8_color == BLUE_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentBlueValue = read_photo_cell(BLUE_TRANS);
            i16_blueDifference = i16_currentBlueValue - i16_ambientBlueValue;

            if (i16_blueDifference > BLUE_LIGHT_THRESH_HOLD) {
                #ifdef DEBUG_BUILD
                printf("Blue confirmed on: %i\n", i16_currentBlueValue);
                #endif

                return;
            }

            // Try and push harder
            // u16_currentBluePushPulse -= PULSE_INCREASE;
            increase_button_push(BLUE_BUTTON);
            simon_push_button(BLUE_BUTTON);

            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    } else if (u8_color == RED_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentRedValue = read_photo_cell(RED_TRANS);
            i16_redDifference = i16_currentRedValue - i16_ambientRedValue;

            if (i16_redDifference > RED_LIGHT_THRESH_HOLD) {
                #ifdef DEBUG_BUILD
                printf("Red confirmed on: %i\n", i16_currentRedValue);
                #endif

                return;
            }

            // Try and push harder
            // u16_currentRedPushPulse -= PULSE_INCREASE;
            increase_button_push(RED_BUTTON);
            simon_push_button(RED_BUTTON);

            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    } else if (u8_color == GREEN_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentGreenValue = read_photo_cell(GREEN_TRANS);
            i16_greenDifference = i16_currentGreenValue - i16_ambientGreenValue;

            if (i16_greenDifference > GREEN_LIGHT_THRESH_HOLD) {
                #ifdef DEBUG_BUILD
                printf("Green confirmed on: %i\n", i16_currentGreenValue);
                #endif

                return;
            }

            // Try and push harder
            // u16_currentGreenPushPulse -= PULSE_INCREASE;
            increase_button_push(GREEN_BUTTON);
            simon_push_button(GREEN_BUTTON);

            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    }
}

// Blocking function until color confirmed off
void confirm_color_off(uint8_t u8_color) {
    // Confirm that the given color turns on
    #ifdef DEBUG_BUILD
    printf("Waiting to turn off\n");
    #endif

    // Depending on which color we're looking for, enter a loop, where we continue to read adc until the adc reads below the lowest level
    if (u8_color == YELLOW_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentYellowValue = read_photo_cell(YELLOW_TRANS);

            if (i16_currentYellowValue < (i16_ambientYellowValue + OFF_THRESH_HOLD)) {
                #ifdef DEBUG_BUILD
                printf("Yellow confirmed off: %i\n", i16_currentYellowValue);
                #endif

                return;
            }
            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    } else if (u8_color == BLUE_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentBlueValue = read_photo_cell(BLUE_TRANS);

            if (i16_currentBlueValue < (i16_ambientBlueValue + OFF_THRESH_HOLD)) {
                #ifdef DEBUG_BUILD
                printf("Blue confirmed off: %i\n", i16_currentBlueValue);
                #endif

                return;
            }
            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    } else if (u8_color == RED_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentRedValue = read_photo_cell(RED_TRANS);

            if (i16_currentRedValue < (i16_ambientRedValue + OFF_THRESH_HOLD)) {
                #ifdef DEBUG_BUILD
                printf("Red confirmed off: %i\n", i16_currentBlueValue);
                #endif

                return;
            }
            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    } else if (u8_color == GREEN_BUTTON) {
        while (1) {
            // If we've surpassed 15 seconds, leave Simon
            if (u8_simonFinished)
                return;

            i16_currentGreenValue = read_photo_cell(GREEN_TRANS);

            if (i16_currentGreenValue < (i16_ambientGreenValue + OFF_THRESH_HOLD)) {
                #ifdef DEBUG_BUILD
                printf("Green confirmed off: %i\n", i16_currentGreenValue);
                #endif

                return;
            }
            // Short DELAY_MS for faster response to light.
            DELAY_MS(RESPONSE_WAIT);
        }
    }
}

// Retract function
void simon_retract_button(buttonID button) {
    switch (button) {
        case YELLOW_BUTTON:
            turn_servo_by_pulse(SIMON_YELLOW, YELLOW_RETRACT);
            break;
        case BLUE_BUTTON:
            turn_servo_by_pulse(SIMON_BLUE, BLUE_RETRACT);
            break;
        case RED_BUTTON:
            turn_servo_by_pulse(SIMON_RED, RED_RETRACT);
            break;
        case GREEN_BUTTON:
            turn_servo_by_pulse(SIMON_GREEN, GREEN_RETRACT);
            break;
        case START_SIMON_BUTTON:
            turn_servo_by_pulse(SIMON_YELLOW, YELLOW_RETRACT);
            break;
        default:
            break;
    }
}

// Hover function
void simon_hover_button(buttonID button) {
    switch (button) {
        case YELLOW_BUTTON:
            turn_servo_by_pulse(SIMON_YELLOW, YELLOW_HOVER);
            break;
        case BLUE_BUTTON:
            turn_servo_by_pulse(SIMON_BLUE, BLUE_HOVER);
            break;
        case RED_BUTTON:
            turn_servo_by_pulse(SIMON_RED, RED_HOVER);
            break;
        case GREEN_BUTTON:
            turn_servo_by_pulse(SIMON_GREEN, GREEN_HOVER);
            break;
        case START_SIMON_BUTTON:
            turn_servo_by_pulse(SIMON_YELLOW, YELLOW_HOVER);
            break;
        default:
            break;
    }
}

// Push function
void simon_push_button(buttonID button) {
    switch (button) {
        case YELLOW_BUTTON:
            turn_servo_by_pulse(SIMON_YELLOW, u16_currentYellowPushPulse);
            break;
        case BLUE_BUTTON:
            turn_servo_by_pulse(SIMON_BLUE, u16_currentBluePushPulse);
            break;
        case RED_BUTTON:
            turn_servo_by_pulse(SIMON_RED, u16_currentRedPushPulse);
            break;
        case GREEN_BUTTON:
            turn_servo_by_pulse(SIMON_GREEN, u16_currentGreenPushPulse);
            break;
        case START_SIMON_BUTTON:
            turn_servo_by_pulse(SIMON_YELLOW, u16_currentStartPushPulse);
            break;
        default:
            break;
    }
}

// Push and hover
void simon_push_and_hover_button(buttonID button) {
    simon_push_button(button);
    DELAY_MS(PUSH_WAIT);
    simon_hover_button(button);
}

// Retract all
void simon_retract_buttons() {
    simon_retract_button(YELLOW_BUTTON);
    simon_retract_button(BLUE_BUTTON);
    simon_retract_button(RED_BUTTON);
    simon_retract_button(GREEN_BUTTON);
}

// Hover all
void simon_hover_buttons() {
    simon_hover_button(YELLOW_BUTTON);
    simon_hover_button(BLUE_BUTTON);
    simon_hover_button(RED_BUTTON);
    simon_hover_button(GREEN_BUTTON);
}

// Push all
void simon_push_buttons() {
    simon_push_button(YELLOW_BUTTON);
    simon_push_button(BLUE_BUTTON);
    simon_push_button(RED_BUTTON);
    simon_push_button(GREEN_BUTTON);
}

// Push and hover all
void simon_push_and_hover_buttons() {
    simon_push_and_hover_button(YELLOW_BUTTON);
    simon_push_and_hover_button(BLUE_BUTTON);
    simon_push_and_hover_button(RED_BUTTON);
    simon_push_and_hover_button(GREEN_BUTTON);
}

void increase_button_push(buttonID button) {
    switch (button) {
        case YELLOW_BUTTON:
            u16_currentYellowPushPulse -= PULSE_INCREASE;
            if (u16_currentYellowPushPulse < YELLOW_MAX_PUSH) {
                u16_currentYellowPushPulse = YELLOW_MAX_PUSH;
                simon_hover_button(YELLOW_BUTTON);
                DELAY_MS(100);
            }
            break;
        case BLUE_BUTTON:
            u16_currentBluePushPulse -= PULSE_INCREASE;
            if (u16_currentBluePushPulse < BLUE_MAX_PUSH) {
                u16_currentBluePushPulse = BLUE_MAX_PUSH;
                simon_hover_button(BLUE_BUTTON);
                DELAY_MS(100);
            }
            break;
        case RED_BUTTON:
            u16_currentRedPushPulse -= PULSE_INCREASE;
            if (u16_currentRedPushPulse < RED_MAX_PUSH) {
                u16_currentRedPushPulse = RED_MAX_PUSH;
                simon_hover_button(RED_BUTTON);
                DELAY_MS(100);
            }
            break;
        case GREEN_BUTTON:
            u16_currentGreenPushPulse -= PULSE_INCREASE;
            if (u16_currentGreenPushPulse < GREEN_MAX_PUSH) {
                u16_currentGreenPushPulse = GREEN_MAX_PUSH;
                simon_hover_button(GREEN_BUTTON);
                DELAY_MS(100);
            }
            break;
        default:
            break;
    }
}

// Count seconds for detrmining when to leave Simon
void _ISRFAST _T5Interrupt (void) {
    u16_milliSeconds++;
    if (u16_milliSeconds >= (SIMON_DURATION * 1000)) {
        u8_simonFinished = 1;
    }
    display_draw_number(u16_milliSeconds / 1000);
    _T5IF = 0;  //clear interrupt flag
}

// Configure a timer for detrmining when to leave Simon
void configTimer5() {
  T5CON = T5_OFF
          | T5_IDLE_CON
          | T5_SOURCE_INT
          | T5_GATE_OFF
          | T5_PS_1_1;  // prescaler of 1
  // PR5 = 0x3D09;          // For prescaler of 256, .1 seconds
  // PR5 = 0xF423;          // For prescaler of 64, .1 seconds
  PR5 = 0x9C39;          // For prescaler of 1, 1 milisecond
  _T5IF = 0;             //clear interrupt flag
  _T5IP = 1;             //choose a priority
  _T5IE = 1;             //enable the interrupt
}
