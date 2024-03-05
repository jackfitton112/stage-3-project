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
#include <RF24.h>
#include "gps.h"
#include "errorno.h" 

enum CommsMode {
    RX,
    TX
};

/*
struct txpayload {
    double lat;
    double lon;
    int headingDeg;
    int timestamp;
};
*/

#define CE_PIN 7
#define CSN_PIN 8


int setup_comms();
void comms_thread_worker();
void nrf_thread();



#endif // COMMS_H


