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

/**
 * @brief BLE service for the boat
 * 
 */
BLEService boatService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEIntCharacteristic boatCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDoubleCharacteristic boatGPSLat("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDoubleCharacteristic boatGPSLon("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEIntCharacteristic boatGPSHead("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic boatGPSSpeed("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);


rtos::Thread ble_thread_runner;
rtos::Thread radio_thread_runner;


RF24 radio(CE_PIN, CSN_PIN);

bool role;

// Set the radio number, as this device is on the boat it will be static and set to 0
bool radioNumber = 0;

// Initialize a copy of the struct to hold the data
txpayload payload;

// Set the radio to be a receiver
uint8_t address[][6] = { "BOAT", "LAND" };






/**
 * @brief Set the up ble object
 * 
 * @note: This function has been tested on the Arduino Nano 33 BLE ONLY
 * 
 * @return int (OK or BLE_INIT_FAIL)
 */
int setup_ble() {

    //Begin the BLE object
    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        return BLE_INIT_FAIL;
    }

  #ifdef DEBUG
    Serial.println("BLE started");
  #endif

    //Set BLE params and services
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

  #ifdef DEBUG
    Serial.println("Bluetooth device active, waiting for connections...");
  #endif

    //set up ble thread
    ble_thread_runner.start(ble_thread);

    return OK;
}

/**
 * @brief The BLE thread
 * 
 */
void ble_thread(){

    while(1){
        //Poll the BLE object for new data
        BLE.poll();

        //write gps data to the BLE characteristics
        boatGPSLat.writeValue(gpsData.lat); //Double
        boatGPSLon.writeValue(gpsData.lon); //Double
        boatGPSHead.writeValue(gpsData.headingDeg); //Int
        boatGPSSpeed.writeValue(gpsData.speed); //Float

        //TODO: Add a "hasFix" characteristic to the BLE service

        //TODO: Make it so the boat updates this data exactly every second
        delay(1000);
    }
}

/**
 * @brief Set the up radio object
 * 
 * @return int (OK or RADIO_INIT_FAIL)
 */
int setup_radio(){
    if(!radio.begin()){
        Serial.println("Radio setup failed");
        return RADIO_INIT_FAIL;
    }

    // The radio should sit in receive mode until it needs to transmit, then it should switch to transmit mode, transmit, and go back to receive mode
    role = TX;

    // Set the radio config
    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(payload));
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]);

    Serial.println("Radio setup complete");
    
    //radio.startListening(); // RX only

    radio_thread_runner.start(radio_thread);

    return OK;

}

/**
 * @brief The radio thread
 * 
 */
void radio_thread(){
  Serial.println("Radio thread started");
    while(1){
      if (role) {
      // This device is a TX node

      //write the gps data to the struct to be sent
      payload.lat = gpsData.lat;
      payload.lon = gpsData.lon;
      payload.headingDeg = gpsData.headingDeg;
      payload.timestamp = gpsData.unixTime;

      bool report = radio.write(&payload, sizeof(txpayload)); // transmit & save the report

      if (!report) {
        Serial.println(F("Transmission failed or timed out"));
      }

    #ifdef DEBUG
        Serial.println(F("Transmission successful"));
    #endif
    
      delay(1000); // slow transmissions down by 1 second

    } else {
      // This device is a RX node

      uint8_t pipe;
      if (radio.available(&pipe)) { // is there a payload? get the pipe number that received it
        radio.read(&payload, sizeof(txpayload)); // fetch payload from FIFO
        Serial.print(F("Received data on pipe "));
        Serial.print(pipe); // print the pipe number
        Serial.print(F(": "));
       //Payload will be different for receiver

      }
    }
  }
}

