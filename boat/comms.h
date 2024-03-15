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
#include <cstdlib> 

#include "gps.h"
#include "errorno.h" 

#define CE_PIN 7
#define CSN_PIN 8

#define TX_QUEUE_SIZE 48
#define TX_TRANSMIT_THREASHOLD 20


extern RF24 radio;

enum CommsMode {
    RX,
    TX
};

enum rxCommands {
    RTH,
    OVERRIDE,
    APPEND,
    HALT,
    ENTER_MANUAL,
    MANUAL_CONTROL, //This command comes with a control command but boat must be in manual mode
    EXIT_MANUAL,
    SET_HOME,
    NON

};

enum controlCommands {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    STOP
};

extern bool role;
// Set the radio number, as this device is on the boat it will be static and set to 0
extern bool radioNumber;

// Define the struct to hold the data
struct txpayload {
    int lat;           // 4 bytes, scaled integer representation of latitude
    int lon;           // 4 bytes, scaled integer representation of longitude
    short headingDeg;  // 2 bytes
    int timestamp;     // 4 bytes
    short pH;          // 2 bytes
    short turbidity;   // 2 bytes
    short temperature; // 2 bytes
}; // Total: 20 bytes


struct rxpayload {
    rxCommands command; //Required
    controlCommands control; //Optional, required when in manual control
    int lat; //Required when overriding or appending or setting home
    int lon; //Required when overriding or appending or setting home

}; //Total: 16 bytes


// Initialize a copy of the struct to hold the data
extern txpayload payload;

// Set the radio to be a receiver
extern uint8_t address[][6];



int setup_ble();
int setup_radio();
void ble_thread();
void radio_thread();
void radio_transmit_queue();

int boat_RTH();
int boat_OVERRIDE(rxpayload rxData);
int boat_APPEND(rxpayload rxData);
int boat_STOP();
int boat_ENTER_MANUAL();
int boat_MANUAL_CONTROL(controlCommands control);
int boat_SET_HOME(rxpayload rxData);
void radio_transmit_data();



#endif // COMMS_H


