/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: static_course_info.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Static information about courses
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        2/24/2014               SECON 2015
*********************************************************************/

#include "pic24_all.h"

#ifdef DEBUG_BUILD
#include <stdio.h>
#endif

#ifndef _STATIC_COURSE_INFO_H
#define _STATIC_COURSE_INFO_H

extern queue_t navigationRoutineQueue;
extern queue_t navigationMoveDistanceQueue;

static inline void prepare_course_routines_one(void) {
    // Game 1
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 840);
    enqueue(&navigationRoutineQueue, LEFT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 150);
    enqueue(&navigationRoutineQueue, PLAY_GAME_PAUSE);

    // Game 2
    enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 150);
    enqueue(&navigationRoutineQueue, RIGHT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 320);
    enqueue(&navigationRoutineQueue, RIGHT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 80);
    enqueue(&navigationRoutineQueue, PLAY_GAME_PAUSE);

    // Game 3
    enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 80);
    enqueue(&navigationRoutineQueue, LEFT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 300);
    enqueue(&navigationRoutineQueue, LEFT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 150);
    enqueue(&navigationRoutineQueue, PLAY_GAME_PAUSE);

    // Game 4
    enqueue(&navigationRoutineQueue, MOVE_REVERSE_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 150);
    enqueue(&navigationRoutineQueue, RIGHT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 160);
    enqueue(&navigationRoutineQueue, RIGHT_TURN);
    enqueue(&navigationRoutineQueue, PLAY_GAME_PAUSE);

    // Finish Line
    enqueue(&navigationRoutineQueue, LEFT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 470);
    enqueue(&navigationRoutineQueue, RIGHT_TURN);
    enqueue(&navigationRoutineQueue, MOVE_FORWARD_DISTANCE);
    enqueue(&navigationMoveDistanceQueue, 230);
}


static inline uint8_t prepare_course_routines(uint8_t u8_course) {
    uint8_t u8_setCourse;

    u8_setCourse = 1;

    switch(u8_course) {
        case 1:
            prepare_course_routines_one();
            break;
        default:
            u8_setCourse = 0;
            break;
    }

    return u8_setCourse;
}


/**
                    x
                    |
              ______|
       x     |   |
       |     |   x
       |_____|
             |
             |
             |----x
             |
             |
         x---|
             |
             |
         ____
        |
        |___
            |
            x
**/
static inline uint8_t prepare_static_course_turn_info_one(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    // To game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_BACK_UP;

    // Leaving game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // To game 2
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 2
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To game 3
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = CURVE_RIGHT_BACK_UP;

    // Leaving game 3
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // To game 4
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 4
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To finish line
    pau8_turnList[u8_turnCount++] = CURVE_LEFT_DEFAULT;

    return u8_turnCount;
}

/**
                x
                |
                |---x
                |
        ________|
        |       |
        x       |
                |
                |---x
                |
                |
                |_____
           x          |
           |__________|
                |
                x
**/
static inline uint8_t prepare_static_course_turn_info_two(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    // To game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // Leaving game 1

    // To game 2
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 2
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To game 3
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_BACK_UP;

    // Leaving game 3
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // To game 4
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 4
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To finish line

    return u8_turnCount;
}


/**
                 ____x
        x       |
        |       |
        |_______|     x
                |     |
                |_____|
                |
                |
                |---x
                |
           x    |
           |____|
                |
                x
**/
static inline uint8_t prepare_static_course_turn_info_three(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    // To game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_BACK_UP;

    // To game 2
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 2
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To game 3
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_BACK_UP;

    // Leaving game 3
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To game 4
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_BACK_UP;

    // Leaving game 4
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // To finish line
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    return u8_turnCount;
}

/**
                x
                |
         _______|
        |       |
        |       |
        x       |
                |----x
                |
                |
                |____
                |    |
                |    |
           x----|    x
                |
                |
                |
                |
                x
**/
static inline uint8_t prepare_static_course_turn_info_four(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    // To game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // Leaving game 1
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // To game 2
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // Leaving game 2
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To game 3
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // Leaving game 3
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;

    // To game 4
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;
    pau8_turnList[u8_turnCount++] = CURVE_LEFT_DEFAULT;

    // Leaving game 4
    pau8_turnList[u8_turnCount++] = NORMAL_RIGHT_DEFAULT;
    pau8_turnList[u8_turnCount++] = NORMAL_LEFT_DEFAULT;

    // To finish line

    return u8_turnCount;
}

/**

**/
static inline uint8_t prepare_static_course_turn_info_five(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    pau8_turnList[0] = 0; // DELETE THIS WHEN THE LIST BECOMES POPULATED

    // To game 1

    // Leaving game 1

    // To game 2

    // Leaving game 2

    // To game 3

    // Leaving game 3

    // To game 4

    // Leaving game 4

    // To finish line

    return u8_turnCount;
}

/**

**/
static inline uint8_t prepare_static_course_turn_info_six(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    pau8_turnList[0] = 0; // DELETE THIS WHEN THE LIST BECOMES POPULATED

    // To game 1

    // Leaving game 1

    // To game 2

    // Leaving game 2

    // To game 3

    // Leaving game 3

    // To game 4

    // Leaving game 4

    // To finish line

    return u8_turnCount;
}

/**

**/
static inline uint8_t prepare_static_course_turn_info_seven(uint8_t* pau8_turnList) {
    uint8_t u8_turnCount = 0;

    pau8_turnList[0] = 0; // DELETE THIS WHEN THE LIST BECOMES POPULATED

    // To game 1

    // Leaving game 1

    // Leaving game 2

    // To game 3

    // To game 4

    // Leaving game 4

    // To finish line

    return u8_turnCount;
}


static inline uint8_t prepare_static_course_turn_info(uint8_t u8_course, uint8_t* pau8_turnList) {
    uint8_t u8_numberOfTurns;

    switch (u8_course) {
        case 1:
            #ifdef DEBUG_BUILD
            printf("Turn layout 1 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_one(pau8_turnList);
            break;
        case 2:
            #ifdef DEBUG_BUILD
            printf("Turn layout 2 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_two(pau8_turnList);
            break;
        case 3:
            #ifdef DEBUG_BUILD
            printf("Turn layout 3 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_three(pau8_turnList);
            break;
        case 4:
            #ifdef DEBUG_BUILD
            printf("Turn layout 4 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_four(pau8_turnList);
            break;
        case 5:
            #ifdef DEBUG_BUILD
            printf("Turn layout 5 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_five(pau8_turnList);
            break;
        case 6:
            #ifdef DEBUG_BUILD
            printf("Turn layout 6 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_six(pau8_turnList);
            break;
        case 7:
            #ifdef DEBUG_BUILD
            printf("Turn layout 7 loaded\n");
            #endif

            u8_numberOfTurns = prepare_static_course_turn_info_seven(pau8_turnList);
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("Invalid turn layout selection\n");
            #endif

            u8_numberOfTurns = 0;
            break;
    }
    return u8_numberOfTurns;
}

#endif
