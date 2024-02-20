/**
 * @file gps.h
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief GPS header file for the Stage 3 Project Boat
 * @version 0.1
 * @date 20-02-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef GPS_H
#define GPS_H

#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
#include <mbed/mbed.h>

struct gps_data {
    double lat;
    double lon;
    int headingDeg;
    float headingRad;
    float speed;
    int unixTime;
};

int setup_gps();
void gps_thread_worker();

extern gps_data gpsData;

#endif // GPS_H