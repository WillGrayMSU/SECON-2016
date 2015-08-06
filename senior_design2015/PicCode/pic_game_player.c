/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: pic_game_player.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Controls all the game playing mechanisms for the
* SECON 2015 robot.
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/20/2014               SECON 2015
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
#include "secon_robot_configuration.h"
#include <string.h>

extern char sz_dispString[BUFFSIZE];

#ifdef DEBUG_BUILD
#include <stdio.h>
#warning "Game Player: DEBUG BUILD"
#endif

// States for the game player PIC
typedef enum {
    PLAY_SIMON,
    PLAY_ETCH,
    PLAY_RUBIK,
    PLAY_CARDS,
    IDLE,
    WAIT,
    CONF
} picGamePlayerState;

// States for the I2C interrupt
typedef enum  {
    STATE_WAIT_FOR_ADDR,
    STATE_WAIT_FOR_WRITE_DATA,
    STATE_SEND_READ_DATA,
    STATE_SEND_READ_LAST
} I2C_STATE;

// Variables to keep up with I2C messages coming in
volatile char  sz_i2cInString[BUFFSIZE+1];
volatile char sz_currentStateString[BUFFSIZE];
volatile uint16_t u16_index;
volatile picGamePlayerState e_picState;
volatile I2C_STATE e_mystate = STATE_WAIT_FOR_ADDR;

// Function declarations
void pic_game_player_init(void);
void I2C_check_command(volatile char *psz_s1);
void wait_for_start_signal(void);

// Main loop for the game player PIC using I2C commands
int main(void) {
    // Start off in wait state, waiting for the LED to turn off
    strncpy((char *) sz_currentStateString, sz_confString, BUFFSIZE);
    e_picState = CONF;

    // Initialize pic and print out serial menu
    configBasic(HELLO_MSG);
    pic_game_player_init();

    // Show that we've booted up
    display_draw_number(00);

    #ifdef DEBUG_BUILD
    printf("Waiting for configuration\n");
    #endif

    // Wait until configuration is done
    while(e_picState == CONF) {
        doHeartbeat();
    }

    #ifdef DEBUG_BUILD
    printf("Finished configuration\n");
    #endif

    // Wait for the start light
    if (SKIP_START_LIGHT == 0) {
        strncpy((char *) sz_currentStateString, sz_waitString, BUFFSIZE);
        e_picState = WAIT;

        // Wait for the start signal
        display_draw_number(START_LIGHT_NUMBER);
        wait_for_start_signal();
    } else {
        DELAY_MS(5000);
    }

    // Move to the idle state, letting the motor controller know that it's time to move
    strncpy((char *) sz_currentStateString, sz_idleString, BUFFSIZE);
    e_picState = IDLE;

    // Game playing loop to check serial commands and I2C commands
    while(1) {
        // If we're out of the Idle state then there's something to be done
        if (e_picState != IDLE) {
            if (e_picState == PLAY_ETCH) {
                if (SKIP_ETCH == 0) {
                    play_etch();
                } else {
                    DELAY_MS(2000);
                }
            } else if (e_picState == PLAY_RUBIK) {
                if (SKIP_RUBIKS == 0) {
                    play_rubiks();
                } else {
                    DELAY_MS(2000);
                }
            } else if (e_picState == PLAY_SIMON) {
                if (SKIP_SIMON == 0) {
                    play_simon();
                } else {
                    DELAY_MS(2000);
                }
            } else if (e_picState == PLAY_CARDS) {
                if (SKIP_CARD == 0) {
                    play_card();
                } else {
                    DELAY_MS(2000);
                }
            }

            // Go back to the idle state after playing a game
            e_picState = IDLE;
            strncpy((char *) sz_currentStateString, sz_idleString, BUFFSIZE);
            #ifdef DEBUG_BUILD
            printf("Waiting for a new game command\n");
            #endif
        }

        // Static number while navigating
        doHeartbeat();
    }
}

void pic_game_player_init() {
    // SSD Init
    ssd_init();

    // Initialize all the timers and comparators for the servos
    servo_init();

    // Initialize the game arm
    game_arm_init();

    // Initialize all the servos to their starting position
    rubik_init();
    etch_init();
    simon_init();

    // Photo cell init
    photo_cell_init();

    // I2C init
    configI2C1(400);
    I2C1ADD = PIC_GAME_PLAYER_ADDR >> 1;
    _SI2C1IF = 0;
    _SI2C1IP = 1;
    _SI2C1IE = 1;
}

// Check incoming I2C messages
void I2C_check_command(volatile char *psz_s1) {
    char drawBuffer[2];

    // Etch
    if (strcmp((char*) psz_s1, sz_playEtchString) == 0) {
        e_picState = PLAY_ETCH;
    }
    // Rubiks
    else if(strcmp((char*) psz_s1, sz_playRubiksString) == 0) {
        e_picState = PLAY_RUBIK;
    }
    // Simon
    else if(strcmp((char*) psz_s1, sz_playSimonString) == 0) {
        e_picState = PLAY_SIMON;
    }
    // Cards
    else if(strcmp((char*) psz_s1, sz_playCardsString) == 0) {
        e_picState = PLAY_CARDS;
    }
    // Wait
    else if(strcmp((char*) psz_s1, sz_waitString) == 0) {
        e_picState = WAIT;
    }
    // Display command
    else if(strstr((char*) psz_s1, sz_dispString) != NULL) {
        drawBuffer[0] = psz_s1[3];
        drawBuffer[1] = psz_s1[4];

        display_draw_buffer(drawBuffer);
    }
    strncpy((char *) sz_currentStateString, (char *) psz_s1, BUFFSIZE);
}

// Function to wait on the start light to turn of
void wait_for_start_signal(void) {
    uint16_t i16_ledMaxOnvalue;
    uint16_t i16_ledMinOnvalue;
    uint16_t i16_ledThreshold;
    uint16_t u16_tempLedvalue;
    uint8_t i;

    i16_ledMaxOnvalue = 0;
    i16_ledMinOnvalue = 65535;
    i16_ledThreshold = 0;
    u16_tempLedvalue = 0;

    #ifdef DEBUG_BUILD
    printf("Waiting for start signal\n");
    #endif

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
    u16_tempLedvalue = read_photo_cell(START_CELL);
    while(u16_tempLedvalue >= (i16_ledMinOnvalue - 10)) {
        u16_tempLedvalue = read_photo_cell(START_CELL);
        doHeartbeat();
    }

    #ifdef DEBUG_BUILD
    printf("Start signal detected\n");
    #endif
}

// I2C intterrupt handler
void _ISRFAST _SI2C1Interrupt(void) {
    uint8_t u8_c;
    _SI2C1IF = 0;
    switch (e_mystate) {
    case STATE_WAIT_FOR_ADDR:
        u8_c = I2C1RCV; //clear RBF bit for address
        u16_index = 0;
        //check the R/W bit and see if read or write transaction
        if (I2C1STATbits.R_W) {
            I2C1TRN = sz_currentStateString[u16_index++];  //get first data byte
            I2C1CONbits.SCLREL = 1;     //release clock line so MASTER can drive it
            e_mystate = STATE_SEND_READ_DATA; //read transaction
        } else e_mystate = STATE_WAIT_FOR_WRITE_DATA;
        break;
    case STATE_WAIT_FOR_WRITE_DATA:
        //character arrived, place in buffer
        sz_i2cInString[u16_index++] = I2C1RCV;  //read the byte
        if (sz_i2cInString[u16_index-1] == 0) {
            //have a complete string, check the command
            I2C_check_command(sz_i2cInString);
            e_mystate = STATE_WAIT_FOR_ADDR; //wait for next transaction
        }
        break;
    case STATE_SEND_READ_DATA:
        I2C1TRN = sz_currentStateString[u16_index++];
        I2C1CONbits.SCLREL = 1;     //release clock line so MASTER can drive it
        if (sz_currentStateString[u16_index-1] == 0) e_mystate = STATE_SEND_READ_LAST;
        //this is the last character, after byte is shifted out, release the clock line again
        break;
    case STATE_SEND_READ_LAST:  //this is interrupt for last character finished shifting out
        e_mystate = STATE_WAIT_FOR_ADDR;
        break;
    default:
        e_mystate = STATE_WAIT_FOR_ADDR;
    }
}
