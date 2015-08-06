/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: pic_navigation.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: PIC that will use a sensor array and motors to
* follow a line to a game then communicate to another PIC via I2C
* so that it can play the game
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        10/01/2014              SECON 2015
*********************************************************************/

#include "pic24_all.h"
#include "line_follower_API.h"
#include "navigation_port_mapping.h"
#include "secon_robot_configuration.h"
#include "SSDisplayAPI.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef DEBUG_BUILD
#warning "Navigation: DEBUG BUILD"
#endif

#define START_BUTTON_PUSHED     (_RG9 == 0)
#define START_BUTTON_RELEASED   (_RG9 == 1)

#define SIMON_BUTTON_PUSHED     (_RG6 == 0)
#define SIMON_BUTTON_RELEASED   (_RG6 == 1)

#define ETCH_BUTTON_PUSHED      (_RG7 == 0)
#define ETCH_BUTTON_RELEASED    (_RG7 == 1)

#define CARD_BUTTON_PUSHED      (_RG8 == 0)
#define CARD_BUTTON_RELEASED    (_RG8 == 1)

#define RUBIKS_BUTTON_PUSHED    (_RG9 == 0)
#define RUBIKS_BUTTON_RELEASED  (_RG9 == 1)

#define UP_BUTTON_PUSHED        (_RG7 == 0)
#define UP_BUTTON_RELEASED      (_RG7 == 1)

#define DOWN_BUTTON_PUSHED      (_RG6 == 0)
#define DOWN_BUTTON_RELEASED    (_RG6 == 1)

#define SET_BUTTON_PUSHED       (_RG9 == 0)
#define SET_BUTTON_RELEASED     (_RG9 == 1)

#define DEBOUNCE_DELAY          10
#define DISPLAY_DELAY           1500

// I2C buffer
char sz_recieveString[BUFFSIZE];

// Navigation externs
extern uint8_t u8_routineBlock;
extern queue_t navigationRoutineQueue;
extern uint8_t u8_currentRoutine;

// Game stuff
uint8_t u8_gameBlock;
uint8_t pu8_gameOrder[4];

// Info for static courses
uint8_t u8_staticCourseNumber;
uint8_t u8_staticTurnLayoutNumber;
char pu8_branchList[4];
uint8_t pu8_recenterList[4];

// Variable for speed configuration
uint8_t u8_newSpeed;

// Function declarations
void pic_navigation_init(void);
void run_static_course(uint8_t pu8_gameOrder[4]);
void navigate_course(uint8_t pu8_gameOrder[4]);
void play_game(gameID game);
void configure_robot(void);
void configure_game_order(void);
void configure_static_course_selection(void);
void configure_static_turn_layout_selection(void);
void configure_branch_order(void);
void configure_speed(void);
void configure_recenter_options(void);
void setup_start_button(void);
void wait_for_start_button_push(void);
void setup_game_buttons(void);

#ifdef DEBUG_BUILD
void print_order(uint8_t pu8_gameOrder[4]);
#endif

// Main loop for the navigation PIC using I2C commands
int main (void) {
    // Static turn layout Info
    uint8_t pau8_turnList[128] = {0};
    uint8_t u8_numberOfStaticTurns;

    // I2C send buffer
    char sz_sendString[BUFFSIZE];

    // Status flags
    u8_gameBlock = 0;
    u8_staticCourseNumber = 0;
    u8_staticTurnLayoutNumber = 0;
    u8_numberOfStaticTurns = 0;
    u8_newSpeed = BASE_SPEED;

    // Configure the motor controller PIC
    configBasic(HELLO_MSG);
    pic_navigation_init();
    setup_start_button();
    setup_game_buttons();

    // Start configuring
    send_display_value("CC");
    DELAY_MS(DISPLAY_DELAY);
    configure_robot();

    // Finished configuring
    send_display_value("CF");
    DELAY_MS(DISPLAY_DELAY);

    // Load up the turn layout, the return value will tell us if we have a turn layout for that course
    u8_numberOfStaticTurns = prepare_static_course_turn_info(u8_staticTurnLayoutNumber, pau8_turnList);
    load_turn_layout_to_line_follower(u8_numberOfStaticTurns, pau8_turnList);

    // Wait for the start button to be pushed
    if (SKIP_START_BUTTON == 0) {
        #ifdef DEBUG_BUILD
        printf("Waiting for start button\n");
        #endif
        send_display_number(START_BUTTON_NUMBER);
        wait_for_start_button_push();
    }

    // Tell the game player to wait on the start light
    strncpy(sz_sendString, sz_waitString, BUFFSIZE);
    writeNI2C1(PIC_GAME_PLAYER_ADDR, (uint8_t *)sz_sendString, 6);

    // Calibrate the sensor arrays
    calibrateAllSensorArrays();

    // Wait for the game player PIC to detect the start light to turn off
    #ifdef DEBUG_BUILD
    printf("Waiting for start signal\n");
    #endif
    while (strcmp((char*) sz_recieveString, "Idle.") != 0) {
        DELAY_MS(250);
        readNI2C1(PIC_GAME_PLAYER_ADDR, (uint8_t *) sz_recieveString, 6);
        doHeartbeat();
    }

    // Check to see if we're using a static course configuration
    if (prepare_course_routines(u8_staticCourseNumber) == 1) {
        // Run static routines
        run_static_course(pu8_gameOrder);
    }
    else {
        // Navigate the whole course
        navigate_course(pu8_gameOrder);
    }

    // After the finish line has been reached just sit and relax
    while(1) doHeartbeat();
}

// Initialization for the navigation PIC
void pic_navigation_init() {
    // Initialize everything to follow a line
    line_follower_init();

    // I2C Config
    configI2C1(400);

    // Wait just a bit for the game player to boot up
    DELAY_MS(1000);
}

// Navigate the whole course
void navigate_course(uint8_t pu8_gameOrder[4]) {
    // Game counter at 0
    uint8_t u8_currentGame;
    u8_currentGame = 0;

    // Get past our start box
    enqueue(&navigationRoutineQueue, MOVE_PAST_START_BOX);
    check_for_routine();

    // Play Simon, Rubiks, Etch, and Card then stop
    while(u8_currentGame <= 3) {
        // Find a box
        follow_line_to_box(BASE_SPEED, pu8_branchList[u8_currentGame], pu8_recenterList[u8_currentGame]);

        // Make our final preperations
        final_game_preparations(pu8_gameOrder[u8_currentGame]);

        // Tell the game player to play a game
        play_game(pu8_gameOrder[u8_currentGame]);
        #ifdef DEBUG_BUILD
        printf("Reached game %u\n", u8_currentGame);
        #endif

        // Leave the game box in preperation to follow the line back
        prepare_to_leave_game(pu8_gameOrder[u8_currentGame]);

        // Get back on the line after spining around
        // reverse_until_line();

        // Get back to the main line
        follow_line_back_to_main_line(BASE_SPEED);

        // Increment to the next game
        u8_currentGame++;
    }

    // Get to the finish line
    follow_line_to_box(BASE_SPEED, 1, 0);
}

void run_static_course(uint8_t pu8_gameOrder[4]) {
    // Game counter at 0
    uint8_t u8_currentGame;
    u8_currentGame = 0;

    // Start the routines
    check_for_routine();

    #ifdef DEBUG_BUILD
    printf("Running a static course\n");
    #endif

    while(u8_currentGame <= 3) {
        // If we're at a game play it otherwise keep navigating
        if (u8_currentRoutine == PLAY_GAME_PAUSE) {
            play_game(pu8_gameOrder[u8_currentGame]);
            u8_gameBlock = 0;
            check_for_routine();
        } else {
            doHeartbeat();
        }
    }
}

// Function to send I2C commands to play games
void play_game(gameID game) {
    char sz_sendString[BUFFSIZE];
    char sz_recieveString[BUFFSIZE];

    // Copy the correct string to send
    if (game == RUBIKS) {
        #ifdef DEBUG_BUILD
        printf("Playing Rubiks\n");
        #endif
        strncpy(sz_sendString, sz_playRubiksString, BUFFSIZE);
    } else if (game == ETCH) {
        #ifdef DEBUG_BUILD
        printf("Playing Etch\n");
        #endif
        strncpy(sz_sendString, sz_playEtchString, BUFFSIZE);
    } else if (game == SIMON) {
        #ifdef DEBUG_BUILD
        printf("Playing Simon\n");
        #endif
        strncpy(sz_sendString, sz_playSimonString, BUFFSIZE);
    } else if (game == CARD) {
        #ifdef DEBUG_BUILD
        printf("Playing Cards\n");
        #endif
        strncpy(sz_sendString, sz_playCardsString, BUFFSIZE);
    }

    // Send the game player the game to play
    writeNI2C1(PIC_GAME_PLAYER_ADDR, (uint8_t *)sz_sendString, 6);

    // Wait until the game is done being played
    do {
        DELAY_MS(1000);
        readNI2C1(PIC_GAME_PLAYER_ADDR, (uint8_t *) sz_recieveString, 6);
        doHeartbeat();
    } while (strcmp((char*) sz_recieveString, "Idle.") != 0);

    // Print out a success message
    #ifdef DEBUG_BUILD
    if (game == RUBIKS) {
        printf("Rubiks played\n");
    } else if (game == ETCH) {
        printf("Etch played\n");
    } else if (game == SIMON) {
        printf("Simon played\n");
    } else if (game == CARD) {
        printf("Cards played\n");
    }
    #endif
}

// Set up the start button
void setup_start_button() {
    CONFIG_RG15_AS_DIG_INPUT();
    // ENABLE_RG15_PULLUP();   /// THIS PIN CAN"T BE INTERNALL PULLED UP, NEEDS AN EXTERNAL ONE
    DELAY_US(1);
}

// Wait until the start button is pushed
void wait_for_start_button_push() {
    while (START_BUTTON_RELEASED) {
        doHeartbeat();
    }
}

// Configure the game selection buttons
void setup_game_buttons() {
    CONFIG_RG6_AS_DIG_INPUT();
    ENABLE_RG6_PULLUP();
    DELAY_US(1);

    CONFIG_RG7_AS_DIG_INPUT();
    ENABLE_RG7_PULLUP();
    DELAY_US(1);

    CONFIG_RG8_AS_DIG_INPUT();
    ENABLE_RG8_PULLUP();
    DELAY_US(1);

    CONFIG_RG9_AS_DIG_INPUT();
    ENABLE_RG9_PULLUP();
    DELAY_US(1);
}

void configure_robot(void) {
    #ifdef DEBUG_BUILD
    printf("Configuring robot\n");
    #endif

    // Configure the game order
    if (STATIC_ORDER == 0) {
        send_display_value("C1");
        DELAY_MS(DISPLAY_DELAY);
        configure_game_order();
    }
    // Default order of games
    else {
        pu8_gameOrder[0] = SIMON;
        pu8_gameOrder[1] = RUBIKS;
        pu8_gameOrder[2] = ETCH;
        pu8_gameOrder[3] = CARD;
    }

    // Configure a static course
    if (SKIP_STATIC_COURSE_SELECTION == 0) {
        send_display_value("C2");
        DELAY_MS(DISPLAY_DELAY);
        configure_static_course_selection();
    }

    // Configure the branch list
    if (SKIP_BRANCH_LIST_SETUP == 0) {
        send_display_value("C3");
        DELAY_MS(DISPLAY_DELAY);
        configure_branch_order();
    }
    // Default branch list
    else {
        pu8_branchList[0] = 'L';
        pu8_branchList[1] = 'R';
        pu8_branchList[2] = 'L';
        pu8_branchList[3] = 'R';
    }

    // Configure which static turn layout to use
    if (SKIP_STATIC_TURN_LAYOUT_SELECTION == 0) {
        send_display_value("C4");
        DELAY_MS(DISPLAY_DELAY);
        configure_static_turn_layout_selection();
    }

    // Configure the speed
    if (SKIP_SPEED_CONFIGURATION == 0) {
        send_display_value("C5");
        DELAY_MS(DISPLAY_DELAY);
        configure_speed();
    }

    // Configure recentering option
    if (SKIP_RECENTER_CONFIGURATION == 0) {
        send_display_value("C6");
        DELAY_MS(DISPLAY_DELAY);
        configure_recenter_options();
    }
}

void configure_game_order() {
    uint8_t u8_simonSet;
    uint8_t u8_cardSet;
    uint8_t u8_etchSet;
    uint8_t u8_rubiksSet;
    uint8_t u8_position;

    u8_simonSet = 0;
    u8_cardSet = 0;
    u8_etchSet = 0;
    u8_rubiksSet = 0;

    u8_position = 0;

    #ifdef DEBUG_BUILD
    printf("Waiting for game order\n");
    #endif

    // Keep waiting until all games are set
    while ((u8_simonSet == 0) || (u8_cardSet == 0) || (u8_etchSet == 0) || (u8_rubiksSet == 0)) {
        if (SIMON_BUTTON_PUSHED && (u8_simonSet == 0)) {
            #ifdef DEBUG_BUILD
            printf("Qeued Simon\n");
            #endif

            pu8_gameOrder[u8_position] = SIMON;
            u8_simonSet = 1;
            u8_position++;

            send_display_number(SIMON_NUMBER);
        }
        else if (CARD_BUTTON_PUSHED && (u8_cardSet == 0)) {
            #ifdef DEBUG_BUILD
            printf("Qeued Cards\n");
            #endif

            pu8_gameOrder[u8_position] = CARD;
            u8_cardSet = 1;
            u8_position++;

            send_display_number(CARD_NUMBER);
        }
        else if (ETCH_BUTTON_PUSHED && (u8_etchSet == 0)) {
            #ifdef DEBUG_BUILD
            printf("Qeued Etch\n");
            #endif

            pu8_gameOrder[u8_position] = ETCH;
            u8_etchSet = 1;
            u8_position++;

            send_display_number(ETCH_NUMBER);
        }
        else if (RUBIKS_BUTTON_PUSHED && (u8_rubiksSet == 0)) {
            #ifdef DEBUG_BUILD
            printf("Qeued Rubiks\n");
            #endif

            pu8_gameOrder[u8_position] = RUBIKS;
            u8_rubiksSet = 1;
            u8_position++;

            send_display_number(RUBIKS_NUMBER);
        }
        doHeartbeat();
    }

    DELAY_MS(DISPLAY_DELAY);
    #ifdef DEBUG_BUILD
    print_order(pu8_gameOrder);
    #endif
}

void configure_static_course_selection() {
    #ifdef DEBUG_BUILD
    printf("Selecting static course\n");
    #endif

    send_display_value("00");

    // Keep displaying the value and changing until the set button is pressed
    while (SET_BUTTON_RELEASED) {
        if (DOWN_BUTTON_PUSHED) {
            if (u8_staticCourseNumber == 0) {
                u8_staticCourseNumber = 99;
            } else {
                u8_staticCourseNumber = u8_staticCourseNumber - 1;
            }

            send_display_number(u8_staticCourseNumber);

            #ifdef DEBUG_BUILD
            printf("Static course %u\n", u8_staticCourseNumber);
            #endif

            DELAY_MS(DEBOUNCE_DELAY);
            while(DOWN_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
        if (UP_BUTTON_PUSHED) {
            if (u8_staticCourseNumber == 99) {
                u8_staticCourseNumber = 0;
            } else {
                u8_staticCourseNumber = u8_staticCourseNumber + 1;
            }

            send_display_number(u8_staticCourseNumber);

            #ifdef DEBUG_BUILD
            printf("Static course %u\n", u8_staticCourseNumber);
            #endif

            DELAY_MS(DEBOUNCE_DELAY);
            while(UP_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
    }
    DELAY_MS(DEBOUNCE_DELAY);
    while(SET_BUTTON_PUSHED);
    DELAY_MS(DEBOUNCE_DELAY);
}

void configure_static_turn_layout_selection() {
    #ifdef DEBUG_BUILD
    printf("Selecting static turn layout\n");
    #endif

    send_display_value("00");

    // Keep displaying the value and changing until the set button is pressed
    while (SET_BUTTON_RELEASED) {
        if (DOWN_BUTTON_PUSHED) {
            if (u8_staticTurnLayoutNumber == 0) {
                u8_staticTurnLayoutNumber = 99;
            } else {
                u8_staticTurnLayoutNumber = u8_staticTurnLayoutNumber - 1;
            }

            send_display_number(u8_staticTurnLayoutNumber);

            printf("Turn layout %u\n", u8_staticTurnLayoutNumber);

            DELAY_MS(DEBOUNCE_DELAY);
            while(DOWN_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
        if (UP_BUTTON_PUSHED) {
            if (u8_staticTurnLayoutNumber == 99) {
                u8_staticTurnLayoutNumber = 0;
            } else {
                u8_staticTurnLayoutNumber = u8_staticTurnLayoutNumber + 1;
            }

            send_display_number(u8_staticTurnLayoutNumber);

            printf("Turn layout %u\n", u8_staticTurnLayoutNumber);

            DELAY_MS(DEBOUNCE_DELAY);
            while(UP_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
    }
    DELAY_MS(DEBOUNCE_DELAY);
    while(SET_BUTTON_PUSHED);
    DELAY_MS(DEBOUNCE_DELAY);
}

void configure_branch_order() {
    char branchBuffer[2];
    char currentDirection;
    uint8_t u8_branchCount;

    send_display_value("1L");

    u8_branchCount = 0;
    currentDirection = 'L';

    #ifdef DEBUG_BUILD
    printf("Selecting branch order\n");
    #endif

    // Wait until all four branches have been set
    while (u8_branchCount <= 3) {
        // Down button is a left
        if (DOWN_BUTTON_PUSHED) {
            currentDirection = 'L';

            branchBuffer[0] = (char)(((int)'0') + u8_branchCount + 1);
            branchBuffer[1] = currentDirection;

            send_display_value(branchBuffer);

            #ifdef DEBUG_BUILD
            printf("Branch %u: %c\n", u8_branchCount+1, currentDirection);
            #endif

            DELAY_MS(DEBOUNCE_DELAY);
            while(DOWN_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }

        // Up button is a right
        if (UP_BUTTON_PUSHED) {
            currentDirection = 'R';

            branchBuffer[0] = (char)(((int)'0') + u8_branchCount + 1);
            branchBuffer[1] = currentDirection;

            send_display_value(branchBuffer);

            #ifdef DEBUG_BUILD
            printf("Branch %u: %c\n", u8_branchCount+1, currentDirection);
            #endif

            DELAY_MS(DEBOUNCE_DELAY);
            while(UP_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }

        // Set the turn to whatever is currently displayed
        if (SET_BUTTON_PUSHED) {
            pu8_branchList[u8_branchCount] = currentDirection;

            branchBuffer[0] = (char)(((int)'0') + u8_branchCount + 1 + 1);
            branchBuffer[1] = currentDirection;

            if (u8_branchCount < 3) {
                send_display_value(branchBuffer);
            }

            #ifdef DEBUG_BUILD
            printf("Branch set %u: %c\n", u8_branchCount+1, currentDirection);
            #endif

            u8_branchCount++;

            DELAY_MS(DEBOUNCE_DELAY);
            while(SET_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
    }

    DELAY_MS(DEBOUNCE_DELAY);
    while(SET_BUTTON_PUSHED);
    DELAY_MS(DEBOUNCE_DELAY);
}

void configure_speed(void) {
    #ifdef DEBUG_BUILD
    printf("Selecting speed\n");
    #endif

    send_display_number(u8_newSpeed);

    // Keep displaying the value and changing until the set button is pressed
    while (SET_BUTTON_RELEASED) {
        if (DOWN_BUTTON_PUSHED) {
            if (u8_newSpeed == 0) {
                u8_newSpeed = 50;
            } else {
                u8_newSpeed = u8_newSpeed - 1;
            }

            send_display_number(u8_newSpeed);

            printf("Current speed %u\n", u8_newSpeed);

            DELAY_MS(DEBOUNCE_DELAY);
            while(DOWN_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
        if (UP_BUTTON_PUSHED) {
            if (u8_newSpeed == 50) {
                u8_newSpeed = 0;
            } else {
                u8_newSpeed = u8_newSpeed + 1;
            }

            send_display_number(u8_newSpeed);

            printf("Current speed %u\n", u8_newSpeed);

            DELAY_MS(DEBOUNCE_DELAY);
            while(UP_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
    }
    set_base_speed(u8_newSpeed);
    DELAY_MS(DEBOUNCE_DELAY);
    while(SET_BUTTON_PUSHED);
    DELAY_MS(DEBOUNCE_DELAY);
}

void configure_recenter_options() {
    char dispBuffer[2];
    uint8_t u8_currentSetting;
    uint8_t u8_gameCount;

    send_display_value("11");

    u8_gameCount = 0;
    u8_currentSetting = 1;

    #ifdef DEBUG_BUILD
    printf("Selecting recenter enable\n");
    #endif

    // Wait until all four branches have been set
    while (u8_gameCount <= 3) {
        // Down button is a left
        if (DOWN_BUTTON_PUSHED) {
            if (u8_currentSetting == 0) {
                u8_currentSetting = 3;
            }
            else {
                u8_currentSetting--;
            }

            dispBuffer[0] = (char)(((int)'0') + u8_gameCount + 1);
            dispBuffer[1] = (char)(((int)'0') + u8_currentSetting);

            send_display_value(dispBuffer);

            #ifdef DEBUG_BUILD
            printf("Game %u: off\n", u8_gameCount+1);
            #endif

            DELAY_MS(DEBOUNCE_DELAY);
            while(DOWN_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }

        // Up button is a right
        if (UP_BUTTON_PUSHED) {
            if (u8_currentSetting == 3) {
                u8_currentSetting = 0;
            }
            else {
                u8_currentSetting++;
            }

            dispBuffer[0] = (char)(((int)'0') + u8_gameCount + 1);
            dispBuffer[1] = (char)(((int)'0') + u8_currentSetting);

            send_display_value(dispBuffer);

            #ifdef DEBUG_BUILD
            printf("Branch %u: on\n", u8_gameCount+1);
            #endif

            DELAY_MS(DEBOUNCE_DELAY);
            while(UP_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }

        // Set the turn to whatever is currently displayed
        if (SET_BUTTON_PUSHED) {
            pu8_recenterList[u8_gameCount] = u8_currentSetting;

            dispBuffer[0] = (char)(((int)'0') + u8_gameCount + 1 + 1);
            dispBuffer[1] = (char)(((int)'0') + u8_currentSetting);

            if (u8_gameCount < 3) {
                send_display_value(dispBuffer);
            }

            u8_gameCount++;

            DELAY_MS(DEBOUNCE_DELAY);
            while(SET_BUTTON_PUSHED);
            DELAY_MS(DEBOUNCE_DELAY);
        }
    }

    DELAY_MS(DEBOUNCE_DELAY);
    while(SET_BUTTON_PUSHED);
    DELAY_MS(DEBOUNCE_DELAY);
}

#ifdef DEBUG_BUILD
void print_order(uint8_t pu8_order[4]) {
    uint8_t u8_i;
    uint8_t game;

    for (u8_i=0;u8_i<=3;u8_i++) {
        game = pu8_order[u8_i];

        if (game == RUBIKS) {
            printf("%u) Rubiks\n", u8_i+1);
        } else if (game == ETCH) {
            printf("%u) Etch\n", u8_i+1);
        } else if (game == SIMON) {
            printf("%u) Simon\n", u8_i+1);
        } else if (game == CARD) {
            printf("%u) Cards\n", u8_i+1);
        }
    }
}
#endif
