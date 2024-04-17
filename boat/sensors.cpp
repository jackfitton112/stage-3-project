/**
 * @file sensors.cpp
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief 
 * @version 0.1
 * @date 15-03-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "sensors.h"

sensorData sensors;

rtos::Thread temp;
rtos::Thread ph;
rtos::Thread turb;



/**
 * @brief Set the up sensors and start the threads
 * 
 */
int setup_sensors() {
    // Setup sensors here

    temp.start(temperature);
    ph.start(pH);
    turb.start(turbidity);
    return OK;
}

void temperature() {
    // Read temperature sensor here
    return;
}

void pH() {
    // Read pH sensor here
    return;
}

void turbidity() {
    // Read turbidity sensor here
    return;
}