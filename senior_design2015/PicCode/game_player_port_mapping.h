/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: game_player_port_mapping.h
* Dependenies: See INCLUDES setion below
* Proessor: PIC33EP512GP806
* Compiler: gcc-pic33
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Port definitions for the game player PIC
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun                                SECON 2015
*********************************************************************/

#define RUBIKS_PLATFORM_PIN         _LATD11
#define RUBIKS_TWIST_PIN            _LATD2

#define SIMON_YELLOW_PIN            _LATD3
#define SIMON_BLUE_PIN              _LATD4
#define SIMON_GREEN_PIN             _LATD5
#define SIMON_RED_PIN               _LATD6

#define ARM_POSITION_PIN            _LATD7
#define ARM_SLIDE_LEFT_PIN          _LATD8
#define ARM_SLIDE_RIGHT_PIN         _LATB13

#define ETCH_VERTICAL_PIN           _LATD10
#define ETCH_HORIZ_PIN              _LATD9

#define YELLOW_DETECTOR_PIN         _RB0
#define BLUE_DETECTOR_PIN           _RB1
#define RED_DETECTOR_PIN            _RB2
#define GREEN_DETECTOR_PIN          _RB3

#define START_PHOTO_RESISTOR_PIN    _RB4

#define D11_LED                     _LATG15
#define D12_LED                     _LATC1
#define D14_LED                     _LATC2
#define D16_LED                     _LATG6
#define D17_LED                     _LATG7
#define D19_LED                     _LATG8
#define D110_LED                    _LATG9

#define D1A_LED                     D17_LED
#define D1B_LED                     D16_LED
#define D1C_LED                     D14_LED
#define D1D_LED                     D12_LED
#define D1E_LED                     D11_LED
#define D1F_LED                     D19_LED
#define D1G_LED                     D110_LED


#define D21_LED                     _LATB6
#define D22_LED                     _LATB7
#define D24_LED                     _LATB8
#define D26_LED                     _LATB9
#define D27_LED                     _LATB10
#define D29_LED                     _LATB11
#define D210_LED                    _LATB12

#define D2A_LED                     D27_LED
#define D2B_LED                     D26_LED
#define D2C_LED                     D24_LED
#define D2D_LED                     D22_LED
#define D2E_LED                     D21_LED
#define D2F_LED                     D29_LED
#define D2G_LED                     D210_LED
