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



//Left Motor is on D2 (p1.11)
Motor leftMotor(P1_11);

//Right Motor is on D3
Motor rightMotor(P1_12);



/**
 * @brief Setup function for the boat
 * 
 */
void setup() {
    Serial.begin(115200); // Open serial port

    //TODO: Remove this before final build as program will hang if no serial connection
    //while(!Serial); // Wait for serial to open

    // Setup GPS
    
    if (setup_gps() != OK) {
        Serial.println(F("GPS setup failed"));
        while (1);
    }

    if(setup_radio() != OK){
        Serial.println(F("Radio setup failed"));
        while(1);
    }


    if (setup_sensors() != OK) {
        Serial.println(F("Sensor setup failed"));
        while (1);
    }



    //Only prints if debug is enabled
    #ifdef DEBUG
    Serial.println(F("Setup complete"));
    #endif


}


void loop() {
  
  //stops the program from running too fast
  rtos::ThisThread::sleep_for(1000);

} 
