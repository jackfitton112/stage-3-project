/**
 * @file comms.cpp
 * @author Jack Fitton (jf1595@york.ac.uk)
 * @brief Communication functions for the boat
 * @version 0.1
 * @date 20-02-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "comms.h"

BLEService boatService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEIntCharacteristic boatCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDoubleCharacteristic boatGPSLat("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDoubleCharacteristic boatGPSLon("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEIntCharacteristic boatGPSHead("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic boatGPSSpeed("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);


rtos::Thread commsThread;
rtos::Thread nrfThread;
rtos::Thread queueWorker;







int setup_comms() {

    
    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        return BLE_INIT_FAIL;
    }

    Serial.println("BLE started");
    BLE.setLocalName("SpookyDuckBoat");
    BLE.setAdvertisedService(boatService);
    boatService.addCharacteristic(boatCharacteristic);
    boatService.addCharacteristic(boatGPSLat);
    boatService.addCharacteristic(boatGPSLon);
    boatService.addCharacteristic(boatGPSHead);
    boatService.addCharacteristic(boatGPSSpeed);
    BLE.addService(boatService);
    boatCharacteristic.setValue(0);
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");

    //start nrf thread
    //nrfThread.start(&nrf_thread);

    //set up ble thread
    commsThread.start(&comms_thread_worker);

    return OK;
}



/*
For now the boat will only transmit data and the land will only receive data
*/
void nrf_thread(){

    return;

}


void comms_thread_worker(){

    while(1){
        BLE.poll();

        //write gps data to the BLE characteristics
        boatGPSLat.writeValue(gpsData.lat); //Double
        boatGPSLon.writeValue(gpsData.lon); //Double
        boatGPSHead.writeValue(gpsData.headingDeg); //Int
        boatGPSSpeed.writeValue(gpsData.speed); //Float

        delay(1000);
    }
}



