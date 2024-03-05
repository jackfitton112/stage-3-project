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

#define CE_PIN 7
#define CSN_PIN 8


extern RF24 radio;

enum CommsMode {
    RX,
    TX
};

extern bool role;
// Set the radio number, as this device is on the boat it will be static and set to 0
extern bool radioNumber;

// Define the struct to hold the data
struct txpayload {
    double lat;
    double lon;
    int headingDeg;
    int timestamp;
};

// Initialize a copy of the struct to hold the data
extern txpayload payload;

// Set the radio to be a receiver
extern uint8_t address[][6];



int setup_comms();
void comms_thread_worker();
void nrf_thread();
int setup_radio();
void radio_thread();



#endif // COMMS_H


