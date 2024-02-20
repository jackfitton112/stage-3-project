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
    Serial.begin(115200); //Open serial port

    //Setup GPS
    if (setup_gps() != 0) {
        Serial.println(F("GPS setup failed"));
        while (1);
    }

    setupComms();


}

void loop() {
    //print the data to the serial port
    Serial.print("Latitude: ");
    Serial.println(gpsData.lat, 6);
    Serial.print("Longitude: ");
    Serial.println(gpsData.lon, 6);
    Serial.print("Heading (degrees): ");
    Serial.println(gpsData.headingDeg);
    Serial.print("Heading (radians): ");
    Serial.println(gpsData.headingRad);
    Serial.print("Speed (m/s): ");
    Serial.println(gpsData.speed);
    Serial.print("Unix Time: ");
    Serial.println(gpsData.unixTime);


    //Wait for 1 second
    delay(1000);
}
