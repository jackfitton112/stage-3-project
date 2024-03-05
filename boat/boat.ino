/**
 * @file boat.ino
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Main file for the Stage 3 Project Boat
 * @version 0.1
 * @date 20-02-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "boat.h"




void setup() {
    Serial.begin(115200); // Open serial port

    while(!Serial); // Wait for serial to open

    // Setup GPS
    if (setup_gps() != OK) {
        Serial.println(F("GPS setup failed"));
        while (1);
    }

    if(setup_comms() != OK){
        Serial.println(F("Comms setup failed"));
        while(1);
    }

    if(setup_radio() != OK){
        Serial.println(F("Radio setup failed"));
        while(1);
    }


}


void loop() {
  


} 
