/**
 * @file gps.cpp
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief GPS source file for the Stage 3 Project Boat
 * @version 0.1
 * @date 20-02-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "gps.h"

gps_data gpsData;
rtos::Thread gps_thread;
SFE_UBLOX_GNSS GPSCHIP; //Create instance of the SFE_UBLOX_GNSS class called GPSCHIP

int setup_gps() {
  //Start the serial port
  Serial.begin(115200);
  //Start the I2C port
  Wire.begin();
  Wire.setClock(400000); //High speed wire needed for high data rate
  //Start the GPS chip
  GPSCHIP.begin();
  //Check if the GPS chip is connected
  if (GPSCHIP.begin() == false) {
    Serial.println("u-blox GNSS not detected at default I2C address. Please check wiring.");
    return 1;
  }
  //Configure the GPS chip and save the configuration
  GPSCHIP.setNavigationFrequency(25);
  GPSCHIP.setI2COutput(COM_TYPE_UBX);
  GPSCHIP.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);

    //Start the gps thread
    gps_thread.start(&gps_thread_worker);


  return 0;
}

void gps_thread_worker(){
    mbed::Timer gpstimer;

    while(1){

        /*
        Fix types:
        0: No fix
        1: Dead reckoning only
        2: 2D-fix
        3: 3D-fix
        4: GNSS + dead reckoning combined
        5: Time only fix
        */
        byte fixType = GPSCHIP.getFixType();
        
        //if fixtype is less than 2D, do not store the data
        if(fixType < 2){
            //Serial.println("No fix / Low quality fix - Not updating GPS data");
            continue;
        }


        gpstimer.start();
        
        //get the data from the GPS chip
        int32_t lat = GPSCHIP.getLatitude();
        int32_t lon = GPSCHIP.getLongitude();
        float heading = GPSCHIP.getHeading();
        heading = heading / 100000.0f; //convert to degrees
        float heading_rad = heading * (PI / 180.0f); //convert to radians

        float speed = GPSCHIP.getGroundSpeed();
        speed = speed / 1000.0f; //convert from mm/s to m/s


        //divide by 10e6 to get the latitude and longitude in degrees
        double lat_deg = lat / 10e6;
        double lon_deg = lon / 10e6;

        int unixTime = GPSCHIP.getUnixEpoch();

        gpstimer.stop();

        //calculate the time taken to get the data
        float time = gpstimer.read();

        float time_remaining = 0.04 - time;

        //wait for the remaining time
        rtos::ThisThread::sleep_for(time_remaining);

        //pack the data into the struct
        gpsData.lat = lat_deg;
        gpsData.lon = lon_deg;
        gpsData.headingDeg = heading;
        gpsData.headingRad = heading_rad;
        gpsData.speed = speed;
        gpsData.unixTime = unixTime;

        //reset the timer
        gpstimer.reset();

    }

}