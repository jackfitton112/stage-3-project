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


RF24 radio(CE_PIN, CSN_PIN);

enum CommsMode {
    RX,
    TX
};

struct txpayload {
    double lat;
    double lon;
    int headingDeg;
    int timestanp;
};

txpayload payload;





void setupComms() {

    
    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        while (1);
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

    //setup queue
    //queueWorker.start(&queue_worker);

    //setup nrf thread
    //nrfThread.start(&nrf_thread);

    //set up ble thread
    commsThread.start(&comms_thread_worker);
    



}

void nrf_thread(){

    //As this code is for the boat, the device will always be called "boat" where as the land based device will be called "land"

    //Setp the radio to be a receiver
    uint8_t address[][6] = { "boat", "land" };

    //Set the radio number, as this device is on the boat it will be static and set to 0
    const bool radioNumber = 0;

    //The radio should sit in receive mode until it needs to transmit, then it should switch to transmit mode, transmit and go back to receive mode
    bool role = RX;

    //init a copy of the struct to hold the data
    txpayload payload;

    //set the radio config
    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(payload));
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]);

    //set receive mode
    radio.startListening();

    char rx_buffer[32];

    while(1){

        //listen and print the data
        if (radio.available()) {
            radio.read(&rx_buffer, sizeof(rx_buffer));
            Serial.print(F("Received "));
            Serial.print(sizeof(rx_buffer));
            Serial.print(F(" bytes on pipe "));
            Serial.print(radioNumber);
            Serial.print(F(": "));
            Serial.println(rx_buffer);
        }

        rtos::ThisThread::sleep_for(100);

    
    }




}

/*
void queue_worker(){
    txpayload *transactionPayload;
    while(1){
        // Check if the queue is empty
        if(txQueue.empty()){
            // If it is, wait for 100ms
            rtos::ThisThread::sleep_for(100);
        } else {
            // If it isn't, pop the first item off the queue
            osEvent evt = txQueue.get();
            if (evt.status == osEventMessage) {
                transactionPayload = (txpayload*)evt.value.p;
            }

            // Set the radio to transmit mode
            radio.stopListening();

            // Transmit the payload
            bool report = radio.write(transactionPayload, sizeof(txpayload));
            if (report) {
                Serial.print(F("Transmission successful! "));  // payload was delivered
                Serial.print(F("Sent: "));
                Serial.println(transactionPayload->lat);  // print payload sent
                Serial.println(transactionPayload->lon);  // print payload sent
            } else {
                Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
            }

            // Put the radio back into receive mode
            radio.startListening();
        }
    }
}
*/

void comms_thread_worker(){

    while(1){
        BLE.poll();

        //write gps data to the BLE characteristics
        //FIXME: Convert from big endian to little endian for BLE
        boatGPSLat.writeValue(gpsData.lat); //Double
        boatGPSLon.writeValue(gpsData.lon); //Double
        boatGPSHead.writeValue(gpsData.headingDeg); //Int
        boatGPSSpeed.writeValue(gpsData.speed); //Float
 
        //txQueue.put(&payload);
        

        delay(1000);
    }
}



