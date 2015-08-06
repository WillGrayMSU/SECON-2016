/*********************************************************************
*
* Mississippi State University
*
*********************************************************************
* FileName: sensor_array_API.c
* Dependenies: See INCLUDES setion below
* Proessor: PIC24HJ64GP506A
* Compiler: gcc-xc16
* Company: Mississippi State University/ECE
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* MODULE FUNCTION: Functions to help control sensor arrays
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author                Date                    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Steven Calhoun        9/20/2014               SECON 2015
*********************************************************************/

#include "sensor_array_API.h"

///////////////////////////////////////////////
//
// Sensor config
//
///////////////////////////////////////////////

// Initializes and callibrates each sensor array
void sensor_array_init() {
    sensor_array_init_line1();
    calibrate(QTR_EMITTERS_ON, 1);

    sensor_array_init_line2();
    calibrate(QTR_EMITTERS_ON, 2);

    sensor_array_init_line3();
    calibrate(QTR_EMITTERS_ON, 3);

    sensor_array_init_line4();
    calibrate(QTR_EMITTERS_ON, 4);
}

// Calibrate a sensor
void calibrate(char u8_readMode, uint8_t u8_line) {
    uint16_t pau16_sensorValues[SENSOR_NUM];
    uint16_t u16_minValue;
    uint16_t i;
    uint16_t j;

    for (i = 0; i < 400; i++) {
        read(pau16_sensorValues, u8_readMode, u8_line);
        u16_minValue = pau16_sensorValues[0];

        for(j = 1; j < SENSOR_NUM; j++) {
            if(pau16_sensorValues[j] < u16_minValue) {
                u16_minValue = pau16_sensorValues[j];
            }
        }
        set_line_max_value(u16_minValue*2.5, u8_line);
    }
}

void calibrateAllSensorArrays() {
    calibrate(QTR_EMITTERS_ON, 1);
    calibrate(QTR_EMITTERS_ON, 2);
    calibrate(QTR_EMITTERS_ON, 3);
    calibrate(QTR_EMITTERS_ON, 4);
}

///////////////////////////////////////////////
//
// Sensor primitives
//
///////////////////////////////////////////////

// Config a line sensor as output
void config_outputs(uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            config_outputs_line1();
            break;
        case 2:
            config_outputs_line2();
            break;
        case 3:
            config_outputs_line3();
            break;
        case 4:
            config_outputs_line4();
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Config a line sensor as input
void config_inputs(uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            config_inputs_line1();
            break;
        case 2:
            config_inputs_line2();
            break;
        case 3:
            config_inputs_line3();
            break;
        case 4:
            config_inputs_line4();
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Set the sensor values high for a line
void set_sensors_high(uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            set_sensors_high_line1();
            break;
        case 2:
            set_sensors_high_line2();
            break;
        case 3:
            set_sensors_high_line3();
            break;
        case 4:
            set_sensors_high_line4();
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Check the sensor values of a line
void check_sensor_values(uint16_t* pau16_sensorValues, uint16_t u16_delta, uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            check_sensor_values_line1(pau16_sensorValues, u16_delta);
            break;
        case 2:
            check_sensor_values_line2(pau16_sensorValues, u16_delta);
            break;
        case 3:
            check_sensor_values_line3(pau16_sensorValues, u16_delta);
            break;
        case 4:
            check_sensor_values_line4(pau16_sensorValues, u16_delta);
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Turn emitters on for a line
void emitters_on(uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            emitters_on_line1();
            break;
        case 2:
            emitters_on_line2();
            break;
        case 3:
            emitters_on_line3();
            break;
        case 4:
            emitters_on_line4();
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Turn emitters off for a line
void emitters_off(uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            emitters_off_line1();
            break;
        case 2:
            emitters_off_line2();
            break;
        case 3:
            emitters_off_line3();
            break;
        case 4:
            emitters_off_line4();
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Get the max value from the calibration step
uint16_t get_line_max_value(uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            return get_line_max_value_line1();
            break;
        case 2:
            return get_line_max_value_line2();
            break;
        case 3:
            return get_line_max_value_line3();
            break;
        case 4:
            return get_line_max_value_line4();
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            return 0;
            break;
    }
}

//Set the max value during calibration
void set_line_max_value(uint16_t u16_maxValue, uint8_t u8_line) {
    switch(u8_line) {
        case 1:
            set_line_max_value_line1(u16_maxValue);
            break;
        case 2:
            set_line_max_value_line2(u16_maxValue);
            break;
        case 3:
            set_line_max_value_line3(u16_maxValue);
            break;
        case 4:
            set_line_max_value_line4(u16_maxValue);
            break;
        default:
            #ifdef DEBUG_BUILD
            printf("NOT GOOD CAP'N\n");
            #endif
            break;
    }
}

// Reads the sensor values using a specific read mode
void read(uint16_t* pau16_sensorValues, char u8_readMode, uint8_t u8_line) {
    uint16_t pau16_off_values[SENSOR_NUM];
    uint16_t u16_i;

    if(u8_readMode == QTR_EMITTERS_ON || u8_readMode == QTR_EMITTERS_ON_AND_OFF) {
        emitters_on(u8_line);
    }
    read_values(pau16_sensorValues, u8_line);

    emitters_off(u8_line);
    if(u8_readMode == QTR_EMITTERS_ON_AND_OFF) {
        read_values(pau16_off_values, u8_line);
    }

    if(u8_readMode == QTR_EMITTERS_ON_AND_OFF) {
        for(u16_i = 0; u16_i < SENSOR_NUM; u16_i++) {
            pau16_sensorValues[u16_i] += get_line_max_value(u8_line) - pau16_off_values[u16_i];
        }
    }
}

// Measures the discharge time for each capacitor associated with a sensor for line 1
void read_values(uint16_t* pau16_sensorValues, uint8_t u8_line) {
    uint16_t u16_start, u16_delta, u16_i, u16_maxValue;
    uint8_t pau8_sensorCheck[SENSOR_NUM];

    u16_maxValue = get_line_max_value(u8_line);
    config_outputs(u8_line);

    DELAY_US(1);
    for(u16_i = 0; u16_i < SENSOR_NUM; u16_i++) {
        pau16_sensorValues[u16_i] = u16_maxValue;
        pau8_sensorCheck[u16_i] = 0;
    }
    set_sensors_high(u8_line);
    DELAY_US(10);

    config_inputs(u8_line);
    DELAY_US(1);

    config_timer4();
    u16_start = TMR4;

    while(TMR4 - u16_start < u16_maxValue) {
        u16_delta = TMR4 - u16_start;

        check_sensor_values(pau16_sensorValues, u16_delta, u8_line);
    }
    T4CONbits.TON = 0;
}

///////////////////////////////////////////////
//
// Sensor usage
//
///////////////////////////////////////////////

// Binary encodes the raw values of each sensor
void read_sensor_array(uint16_t* pau16_sensorValues, char u8_readMode, uint8_t u8_line) {
    uint16_t i;
    read(pau16_sensorValues, u8_readMode, u8_line);

    for(i = 0; i < SENSOR_NUM; i++) {
        if(pau16_sensorValues[i] < get_line_max_value(u8_line) - 1) {
            pau16_sensorValues[i] = 1;
        }
        else {
            pau16_sensorValues[i] = 0;
        }
    }
}

// Read all the of the sensor arrays
void read_all_sensor_arrays(uint16_t* pau16_allSensorValues, char u8_readMode) {
    uint16_t pau16_sensorValues[SENSOR_NUM];
    uint8_t i;
    uint8_t j;


    // Read all the lines
    for(i = 1; i <= NUM_ARRAYS; i++) {
        // Read one line at a time
        read_sensor_array(pau16_sensorValues, u8_readMode, i);

        // Take the values from the read and put it in a combined array
        for(j = 0; j < SENSOR_NUM; j++) {
            pau16_allSensorValues[((i - 1)*SENSOR_NUM)+j] = pau16_sensorValues[j];
        }
    }
}

// Read the consecutive line of three lines
void read_sensor_triple(uint16_t* pau16_tripleSensorValues, char u8_readMode) {
    uint16_t pau16_sensorValues[SENSOR_NUM];
    uint8_t i;

    // Read the left line and place them in the combined array
    read_sensor_array(pau16_sensorValues, u8_readMode, TRIPLE_LEFT_LINE);
    for(i = 0; i < SENSOR_NUM; i++) {
        pau16_tripleSensorValues[i] = pau16_sensorValues[SENSOR_NUM - 1 - i];
    }

    // Read the main line and place them in the combined array
    read_sensor_array(pau16_sensorValues, u8_readMode, MAIN_LINE);
    for(i = 0; i < SENSOR_NUM; i++) {
        pau16_tripleSensorValues[i + SENSOR_NUM*1] = pau16_sensorValues[SENSOR_NUM - 1 - i];
    }

    // Read the right line and place them in the combined array
    read_sensor_array(pau16_sensorValues, u8_readMode, TRIPLE_RIGHT_LINE);
    for(i = 0; i < SENSOR_NUM; i++) {
        pau16_tripleSensorValues[i + SENSOR_NUM*2] = pau16_sensorValues[SENSOR_NUM - 1 - i];
    }
}

// Read the high resolution double sensor line
void read_sensor_hi_res(uint16_t* pau16_hiResSensorValues, char u8_readMode) {
    uint16_t pau16_backSensorValues[SENSOR_NUM];
    uint16_t pau16_frontSensorValues[SENSOR_NUM];
    uint8_t i;

    read_sensor_array(pau16_backSensorValues, u8_readMode, HI_RES_LINE);
    read_sensor_array(pau16_frontSensorValues, u8_readMode, MAIN_LINE);

    // Front sensors go in the even slots
    for(i = 0; i < SENSOR_NUM; i++) {
        pau16_hiResSensorValues[i*2] = pau16_frontSensorValues[SENSOR_NUM - 1 - i];
    }

    // Back sensors go in the odd slots
    for(i = 0; i < SENSOR_NUM; i++) {
        pau16_hiResSensorValues[(i*2)+1] = pau16_backSensorValues[SENSOR_NUM - 1 - i];
    }
}

// Read the consecutive line of three lines plus the high resolution double sensor line
void read_sensor_triple_plus_hi_res(uint16_t* pau16_tripleHiResSensorValues, char u8_readMode) {
    uint16_t pau16_sensorValues[SENSOR_NUM];
    uint16_t pau16_hiResSensorValues[SENSOR_NUM*2];
    uint8_t i;

    // Read the left line and place them in the combined array
    read_sensor_array(pau16_sensorValues, u8_readMode, TRIPLE_LEFT_LINE);
    for(i = (SENSOR_NUM*0); i < SENSOR_NUM; i++) {
        pau16_tripleHiResSensorValues[i] = pau16_sensorValues[SENSOR_NUM - 1 - i];
    }

    // Read the main line plus the hires line and place them in the combine array
    read_sensor_hi_res(pau16_hiResSensorValues, u8_readMode);
    for(i = 0; i < SENSOR_NUM*2; i++) {
        pau16_tripleHiResSensorValues[i + SENSOR_NUM*1] = pau16_hiResSensorValues[i];
    }

    // Read the right line and place them in the combined array
    read_sensor_array(pau16_sensorValues, u8_readMode, TRIPLE_RIGHT_LINE);
    for(i = 0; i < SENSOR_NUM; i++) {
        pau16_tripleHiResSensorValues[i + SENSOR_NUM*3] = pau16_sensorValues[SENSOR_NUM - 1 - i];
    }
}

int16_t read_line(char u8_readMode) {
    uint16_t pau16_sensors[PID_SENSOR_NUM];
    uint8_t i;
    uint8_t u8_onLine;

    uint16_t u16_sum;
    uint16_t u16_value;

    uint32_t u32_avg;

    static int16_t i16_lastValue;

    u8_onLine = 0;
    u16_sum = 0;
    u32_avg = 0;
    i16_lastValue = 0;

    read_sensor_hi_res(pau16_sensors, u8_readMode);

    for(i = 0;i < PID_SENSOR_NUM;i++) {
        u16_value = (1000 * pau16_sensors[i]);

        if(u16_value > 200) {
            u8_onLine = 1;
        }

        u32_avg += (uint32_t)(u16_value) * (i * 1000);
        u16_sum += u16_value;
    }

    if(u8_onLine == 0) {
        if(i16_lastValue < (PID_SENSOR_NUM-1)*1000/2) {
            return 0;
        } else {
            return (PID_SENSOR_NUM-1)*1000;
        }

    }

    i16_lastValue = u32_avg/u16_sum;

    return i16_lastValue;
}



///////////////////////////////////////////////
//
// Sensor printing (self explanatory)
//
///////////////////////////////////////////////

#ifdef DEBUG_BUILD
// Print out all the values for the passed in line
void print_sensor_array(uint8_t u8_line) {
    uint16_t pau16_sensorValues[SENSOR_NUM];
    uint8_t i;

    read_sensor_array(pau16_sensorValues, QTR_EMITTERS_ON, u8_line);

    for(i = 0; i < SENSOR_NUM; i++) {
        printf("%u ", pau16_sensorValues[i]);
    }
    printf("\n");
}

// Print out all of the sensor array
void print_all_sensor_arrays() {
    uint16_t pau16_allSensorValues[SENSOR_NUM*NUM_ARRAYS];
    uint8_t i;

    // Read all the sensor arrays
    read_all_sensor_arrays(pau16_allSensorValues, QTR_EMITTERS_ON);

    // Print all of the lines
    for(i = 0; i < SENSOR_NUM*NUM_ARRAYS; i++) {
        printf("%u", pau16_allSensorValues[i]);
    }
    printf("\n");
}

// Print out the line of three lines
void print_sensor_triple() {
    uint16_t pau16_tripleSensorValues[SENSOR_NUM*3];
    uint8_t i;

    read_sensor_triple(pau16_tripleSensorValues, QTR_EMITTERS_ON);

    for(i = 0; i < SENSOR_NUM*3; i++) {
        printf("%u", pau16_tripleSensorValues[i]);
    }
    printf("\n");
}

// Print out the hi res line
void print_sensor_hi_res() {
    uint16_t pau16_hiResSensorValues[SENSOR_NUM*2];
    uint8_t i;

    read_sensor_hi_res(pau16_hiResSensorValues, QTR_EMITTERS_ON);

    for(i = 0; i < SENSOR_NUM*2; i++) {
        printf("%u", pau16_hiResSensorValues[i]);
    }
    printf("\n");
}

// Print out the triple line plus hi res line
void print_sensor_triple_plus_hi_res() {
    uint16_t pau16_tripleHiResSensorValues[SENSOR_NUM*5];
    uint8_t i;
    read_sensor_triple_plus_hi_res(pau16_tripleHiResSensorValues, QTR_EMITTERS_ON);

    for(i = 0; i < SENSOR_NUM*4; i++) {
        printf("%u", pau16_tripleHiResSensorValues[i]);
    }
    printf("\n");
}
#endif
