/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: pic_game_player_controller.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: PIC code that will give a user control over all
* the game player functions via a serial menu
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        12/5/2014               SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "motors_API.h"
#include "SSDisplayAPI.h"
#include "etch_code.h"
#include "rubiks_code.h"
#include "simon_code.h"
#include "card_code.h"
#include "platform_control.h"
#include "game_arm_control.h"
#include <string.h>
#include <stdio.h>

// Varibles for input and to hold the current status of the robot
char u8_c;
char u8_c2;
uint8_t u8_platformPos;
uint8_t u8_twistPos;
uint8_t u8_platformFlipped;
uint8_t u8_armSlide;
uint8_t u8_armPivot;
uint16_t u16_pwm;

// Function Declarations
void pic_game_player_init(void);
void game_player_serial_command(uint8_t u8_c);
void game_player_serial_menu(void);
void game_player_servo_menu(void);
void simon_menu(void);
void game_player_set_servo(char u8_servo);
void start_light_print(void);
void block_until_start_light_off();

// Main loop for the game player pic using I2C command
int main(void) {
    // Initialize pic and print out serial menu
    configBasic(HELLO_MSG);
    pic_game_player_init();

    u8_twistPos = 0;
    u8_platformPos = 0;
    u8_platformFlipped = 0;
    u8_armSlide = 0;
    u8_armPivot = 0;

    game_player_serial_menu();

    // Game playing loop to check serial commands and I2C commands
    while(1) {
        if(isCharReady()) {
            // Handle serial command
            u8_c = inChar();
            game_player_serial_command(u8_c);
            game_player_serial_menu();
        }
        doHeartbeat();
    }
}

// Controller initialization
void pic_game_player_init() {
    // Initialize all the timers and comparators for the servos
    servo_init();

    // Delay to let the configurations to take place
    DELAY_MS(500);

    // Initialize the game arm
    game_arm_init();

    // Initialize all the servos to their starting position
    rubik_init();
    etch_init();
    simon_init();

    // Photo cell init
    photo_cell_init();

    // SSD init
    ssd_init();
}

///////////////////////////////////////////////
//
// Serial menus for the game player PIC
//
///////////////////////////////////////////////

void game_player_serial_command(uint8_t u8_c) {
    char u8_servo;
    char sz_buf[32];
    uint8_t u8_percentage;

    switch(u8_c) {
        case 'e':
            play_etch();
            break;
        case 'c':
            play_rubiks();
            break;
        case 's':
            printf("\nSimon play mode\n");
            printf("   s) for 15 seconds\n");
            printf("   i) infinitely\n");
            u8_c = inChar();
            if (u8_c == 's') {
                play_simon();
            } else if (u8_c == 'i') {
                play_simon_infinitely();
            }
            break;
        case 'd':
            play_card();
            break;
        case 'p':
            if (u8_platformPos == 0) {
                u8_platformPos = 1;
                printf("\n*** Lowering platform ***\n");
                platform_rubiks();
            } else {
                u8_platformPos = 0;
                printf("\n*** Raising platform ***\n");
                platform_up();
            }
            break;
        case 't':
            printf("\n*** Twisting rubiks twist ***\n");
            if (u8_twistPos == 0) {
                u8_twistPos = 1;
                twist_rubiks_clock();
            } else {
                u8_twistPos = 0;
                twist_rubiks_counter();
            }
            break;
        case 'f':
            if (u8_platformFlipped == 0) {
                u8_platformFlipped = 1;
                printf("\n*** Flipping back platform for Etch ***\n");
                platform_etch();
            } else {
                u8_platformFlipped = 0;
                printf("\n*** Reseting platform to original position ***\n");
                platform_etch_undo();
            }
            break;
        case 'r':
            simon_menu();
            u8_servo = inChar();

            switch(u8_servo) {
                case 'a':
                    printf("\n*** Retracting all buttons ***\n");
                    simon_retract_buttons();
                    break;
                case 'y':
                    printf("\n*** Retracting yellow button ***\n");
                    simon_retract_button(YELLOW_BUTTON);
                    break;
                case 'b':
                    printf("\n*** Retracting blue button ***\n");
                    simon_retract_button(BLUE_BUTTON);
                    break;
                case 'r':
                    printf("\n*** Retracting red button ***\n");
                    simon_retract_button(RED_BUTTON);
                    break;
                case 'g':
                    printf("\n*** Retracting green button ***\n");
                    simon_retract_button(GREEN_BUTTON);
                    break;
                case 's':
                    printf("\n*** Retracting start button ***\n");
                    simon_retract_button(START_BUTTON);
                    break;
                default:
                    printf("Invalid command");
                    break;
            }
            break;
        case 'h':
            simon_menu();
            u8_servo = inChar();

            switch(u8_servo) {
                case 'a':
                    printf("\n*** Hovering all buttons ***\n");
                    simon_hover_buttons();
                    break;
                case 'y':
                    printf("\n*** Hovering yellow button ***\n");
                    simon_hover_button(YELLOW_BUTTON);
                    break;
                case 'b':
                    printf("\n*** Hovering blue button ***\n");
                    simon_hover_button(BLUE_BUTTON);
                    break;
                case 'r':
                    printf("\n*** Hovering red button ***\n");
                    simon_hover_button(RED_BUTTON);
                    break;
                case 'g':
                    printf("\n*** Hovering green button ***\n");
                    simon_hover_button(GREEN_BUTTON);
                    break;
                case 's':
                    printf("\n*** Hovering start button ***\n");
                    simon_hover_button(START_BUTTON);
                    break;
                default:
                    printf("Invalid command");
                    break;
            }
            break;
        case 'b':
            simon_menu();
            u8_servo = inChar();

            switch(u8_servo) {
                case 'a':
                    printf("\n*** Pushing all buttons ***\n");
                    simon_push_buttons();
                    break;
                case 'y':
                    printf("\n*** Pushing yellow button ***\n");
                    simon_push_button(YELLOW_BUTTON);
                    break;
                case 'b':
                    printf("\n*** Pushing blue button ***\n");
                    simon_push_button(BLUE_BUTTON);
                    break;
                case 'r':
                    printf("\n*** Pushing red button ***\n");
                    simon_push_button(RED_BUTTON);
                    break;
                case 'g':
                    printf("\n*** Pushing green button ***\n");
                    simon_push_button(GREEN_BUTTON);
                    break;
                case 's':
                    printf("\n*** Pushing start button ***\n");
                    simon_push_button(START_BUTTON);
                    break;
                default:
                    printf("Invalid command");
                    break;
            }
            break;
        case 'a':
            if (u8_armSlide == 0) {
                // Slide arm forward
                game_arm_slide_forward();
                u8_armSlide = 1;
            } else {
                // Slide arm back
                game_arm_slide_back();
                u8_armSlide = 0;
            }
            break;
        case 'o':
            if (u8_armPivot == 0) {
                // Lower the arm
                game_arm_lower();
                u8_armPivot = 1;
            } else {
                // Raise the arm
                game_arm_raise();
                u8_armPivot = 0;
            }
            break;
        case 'y':
            printf("\nEnter percentage to slide arm: ");
            inStringEcho(sz_buf,31);
            sscanf(sz_buf,"%hhu",(uint8_t *) &u8_percentage);

            game_arm_slide(u8_percentage);
            break;
        case '/':
            printf("\nShimmy (c)ube or (e)tch\n");
            u8_c2= inChar();

            if (u8_c2 == 'c') {
                game_arm_pull_rubiks();
            }
            else if (u8_c2 == 'e') {
                game_arm_pull_etch();
            }
            break;
        case 'z':
            while (isCharReady() == 0) {
                photo_trans_print();
                doHeartbeat();
            }
            break;
        case 'l':
            start_light_print();
            break;
        case 'w':
            block_until_start_light_off();
            break;
        case 'v':
            test_ss_displays();
            break;
        case 'x':
            // Set a sepcific servo
            game_player_servo_menu();
            u8_servo = inChar();
            game_player_set_servo(u8_servo);
            break;
        default:
            printf("Invalid command");
            break;
    }
}

void game_player_serial_menu(void) {
    printf("\nChoose a command\n");
    printf("   e) play Etch-a-Sketch\n");
    printf("   c) spin Rubiks\n");
    printf("   s) play Simon\n");
    printf("   d) pick up a card from the deck\n");
    if (u8_platformPos == 0) {
        printf("   p) raise platform\n");
    } else {
        printf("   p) lower platform\n");
    }
    printf("   t) spin rubiks twist servo\n");
    if (u8_platformFlipped == 0) {
        printf("   f) prepare for etch\n");
    } else {
        printf("   f) reset platform to init\n");
    }
    printf("   r)retract Simon arms\n");
    printf("   h)hover Simon arms\n");
    printf("   b)push Simon buttons\n");
    if (u8_armSlide == 0) {
        printf("   a) slide arm forward\n");
    } else {
        printf("   a) slide arm back\n");
    }
    if (u8_armPivot == 0) {
        printf("   o) lower arm\n");
    } else {
        printf("   o) raise arm\n");
    }
    printf("   y) slide the arm by a percentage\n");
    printf("   /) to shimmy the arm\n");
    printf("   z) read photo transistors\n");
    printf("   l) read start light resistor\n");
    printf("   w) block until start light off\n");
    printf("   v) test the displays\n");
    printf("   x) set a servo\n");
}

void game_player_servo_menu(void) {
    printf("\nChoose a servo\n");
    printf("   v) Etch vertical\n");
    printf("   h) Etch horizontal\n");
    printf("   p) Platform\n");
    printf("   t) Rubiks twist\n");
    printf("   y) Simon yellow\n");
    printf("   b) Simon blue\n");
    printf("   r) Simon red\n");
    printf("   g) Simon green\n");
    printf("   l) Arm slide left\n");
    printf("   ;) Arm slide right\n");
    printf("   a) Arm position\n");
}

void simon_menu(void) {
    printf("\nChoose Simon arm\n");
    printf("   a) All arms\n");
    printf("   y) Yellow arm\n");
    printf("   b) Blue arm\n");
    printf("   r) Red arm\n");
    printf("   g) Green arm\n");
    printf("   s) Start arm\n");
}

void game_player_set_servo(char u8_servo) {
    uint16_t u16_pwm;
    char sz_buf[32];
    char u8_c;
    u8_c = 0;

    // Get pulse width for non-continuous servos
    if (u8_servo != '0' && u8_servo != '1') {
        printf("\nEnter pulse width: ");
        inStringEcho(sz_buf,31);
        sscanf(sz_buf,"%d",(uint16_t *) &u16_pwm);
    }

    // Set servo appropriately
    switch(u8_servo) {
        case 'v':
            printf("\nUse 'a' and 'd' to move servo clockwise and counter clockwise and 'q' to quit");
            while (u8_c != 'q') {
                u8_c = inChar();
                if (u8_c == 'a') {
                    step_servo(0, ETCH_VERTICAL);
                } else if (u8_c == 'd') {
                    step_servo(1, ETCH_VERTICAL);
                } else {
                    stop_servo(ETCH_VERTICAL);
                }
            }
            break;
        case 'h':
            printf("\nUse 'a' and 'd' to move servo and 'q' to quit");
            while (u8_c != 'q') {
                u8_c = inChar();
                if (u8_c == 'a') {
                    step_servo(0, ETCH_HORIZ);
                } else if (u8_c == 'd') {
                    step_servo(1, ETCH_HORIZ);
                } else {
                    stop_servo(ETCH_HORIZ);
                }
            }
            break;
        case 'p':
            turn_servo_by_pulse(RUBIKS_PLATFORM, u16_pwm);
            printf("\n*** Setting platform to %u ***\n", u16_pwm);
            break;
        case 't':
            turn_servo_by_pulse(RUBIKS_TWIST, u16_pwm);
            printf("\n*** Setting rubiks twist to %u ***\n", u16_pwm);
            break;
        case 'y':
            turn_servo_by_pulse(SIMON_YELLOW, u16_pwm);
            printf("\n*** Setting simon yellow to %u ***\n", u16_pwm);
            break;
        case 'b':
            turn_servo_by_pulse(SIMON_BLUE, u16_pwm);
            printf("\n*** Setting simon blue to %u ***\n", u16_pwm);
            break;
        case 'r':
            turn_servo_by_pulse(SIMON_RED, u16_pwm);
            printf("\n*** Setting simon red to %u ***\n", u16_pwm);
            break;
        case 'g':
            turn_servo_by_pulse(SIMON_GREEN, u16_pwm);
            printf("\n*** Setting simon green to %u ***\n", u16_pwm);
            break;
        case 's':
            turn_servo_by_pulse(ARM_SLIDE_LEFT, u16_pwm);
            printf("\n*** Setting arm extend to %u ***\n", u16_pwm);
            break;
        case '2':
            turn_servo_by_pulse(ARM_SLIDE_RIGHT, u16_pwm);
            printf("\n*** Setting arm extend 2 to %u ***\n", u16_pwm);
            break;
        case 'a':
            turn_servo_by_pulse(ARM_POSITION, u16_pwm);
            printf("\n*** Setting arm pivot to %u ***\n", u16_pwm);
            break;
        default:
            printf("Invalid choice\n");
            break;
    }
}

void start_light_print() {
    uint16_t u16_startLightvalue;

    while (1) {
        u16_startLightvalue = read_photo_cell(START_CELL);
        printf("Start light: %d\n", u16_startLightvalue);

        doHeartbeat();
    }
}

void block_until_start_light_off() {
    uint16_t i16_ledMaxOnvalue;
    uint16_t i16_ledMinOnvalue;
    uint16_t i16_ledThreshold;
    uint16_t u16_tempLedvalue;
    uint8_t i;

    i16_ledMaxOnvalue = 0;
    i16_ledMinOnvalue = 65535;
    i16_ledThreshold = 0;
    u16_tempLedvalue = 0;

    printf("Calibrating\n");

    // Sample a few values from the on LED
    for (i = 0; i < 100; ++i) {
        u16_tempLedvalue = read_photo_cell(START_CELL);
        if (u16_tempLedvalue < i16_ledMinOnvalue) {
            i16_ledMinOnvalue = u16_tempLedvalue;
        }
        if (u16_tempLedvalue > i16_ledMaxOnvalue) {
            i16_ledMaxOnvalue = u16_tempLedvalue;
        }
        DELAY_MS(10);
    }

    // Calculate the threshold
    i16_ledThreshold = i16_ledMaxOnvalue - i16_ledMinOnvalue;

    // Wait until the start light turns off
    printf("Waiting for start signal\n");
    u16_tempLedvalue = read_photo_cell(START_CELL);
    while(u16_tempLedvalue >= (i16_ledMinOnvalue - i16_ledThreshold)) {
        u16_tempLedvalue = read_photo_cell(START_CELL);
        doHeartbeat();
    }

    printf("Start signal detected\n");
}
