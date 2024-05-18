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

//define analog pins for sensors
#define PH_PIN A0
#define TURBIDITY_PIN A1
#define TEMPERATURE_PIN A2




/**
 * @brief Set the up sensors and start the threads
 * 
 */
int setup_sensors() {
    // Setup sensors here
    mbed::AnalogIn tempSensor(TEMPERATURE_PIN);
    mbed::AnalogIn phSensor(PH_PIN);
    mbed::AnalogIn turbiditySensor(TURBIDITY_PIN);

    temp.start(temperature);
    ph.start(pH);
    turb.start(turbidity);
    return OK;
}

void temperature() {
    while(1){
        //READ TEMPERATURE SENSOR pin A2
        //convert the value to a temperature value
        sensors.temperature = tempSensor.read() * 100;
        //print the temperature value
        //Serial.print("Temperature: ");
        //Serial.println(sensors.temperature);
        

}

void pH() {

    while(1){

  
        //READ PH SENSOR pin A0
        //convert the value to a pH value
        sensors.pH = phSensor.read() * 14;

        //print the pH value
        //Serial.print("pH: ");
        //Serial.println(sensors.pH);
        rtos::ThisThread::sleep_for(1000);

    }
}

void turbidity() {
    while(1){
        //READ TURBIDITY SENSOR pin A1
        //convert the value to a turbidity value
        sensors.turbidity = turbiditySensor.read() * 100;
        

        //print the turbidity value
        //Serial.print("Turbidity: ");
        //Serial.println(sensors.turbidity);
        rtos::ThisThread::sleep_for(1000);
    }
}