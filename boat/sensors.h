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

#include <mbed.h>
#include "errorno.h"

#define PH_PIN A0
#define TURBIDITY_PIN A1
#define TEMPERATURE_PIN A2

struct sensorData {
    short pH = 0;
    short turbidity = 0;
    short temperature = 0;
};

extern sensorData sensors;

int setup_sensors();
void temperature();
void pH();
void turbidity();

#endif // SENSORS_H