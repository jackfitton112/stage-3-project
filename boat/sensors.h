/**
 * @file sensors.h
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief 
 * @version 0.1
 * @date 15-03-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "mbed/mbed.h"
#include "errorno.h"

struct sensorData {
    short pH;
    short turbidity;
    short temperature;
};


int setup_sensors();
void temperature();
void pH();
void turbidity();

#endif // SENSORS_H