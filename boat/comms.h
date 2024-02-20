/**
 * @file comms.h
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Communication functions for the boat header file
 * @version 0.1
 * @date 20-02-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef COMMS_H
#define COMMS_H


#include <ArduinoBLE.h>
#include <mbed/mbed.h>
#include "gps.h"


void setupComms();
void comms_thread_worker();


#endif // COMMS_H


