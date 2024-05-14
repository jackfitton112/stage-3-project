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
    while(1){
        // Test function, pick a random temprature between 10 and 12c
        //if the temp is already set, change it by +/- 0.3c (randomly)
        if (sensors.temperature != 0) {
            sensors.temperature += (rand() % 2) ? 3 : -3;
        } else {
            sensors.temperature = 10 + (rand() % 2);
        }

        //if the temperature is greater than 12, set it to 12
        if (sensors.temperature > 12) {
            sensors.temperature = 12;
        }

        //if the temperature is less than 10, set it to 10
        if (sensors.temperature < 10) {
            sensors.temperature = 10;
        }

        //print the temperature value
        //Serial.print("Temperature: ");
        //Serial.println(sensors.temperature);

        rtos::ThisThread::sleep_for(1000);

    }

}

void pH() {

    while(1){
        // Test function, pick a random pH between 7 and 8.5
        //if the pH is already set, change it by +/- 0.1 (randomly)

        sensors.pH = (rand() % 2) ? 7 : 8;
  


        
        //print the pH value
        //Serial.print("pH: ");
        //Serial.println(sensors.pH);
        rtos::ThisThread::sleep_for(1000);

    }
}

void turbidity() {
    while(1){
        // Test function, pick a random turbidity between 40 and 60%
        //if the turbidity is already set, change it by +/- 5% (randomly)
        if (sensors.turbidity != 0) {
            sensors.turbidity += (rand() % 2) ? 2 : -2;
        } else {
        sensors.turbidity = 40 + (rand() % 20);
        }

        //if the turbidity is greater than 60, set it to 60
        if (sensors.turbidity > 61) {
            sensors.turbidity = 61;
        }

        //print the turbidity value
        //Serial.print("Turbidity: ");
        //Serial.println(sensors.turbidity);
        rtos::ThisThread::sleep_for(1000);
    }
}