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

#define DEBUG

/**
 * @brief BLE service for the boat
 * 
 */
rtos::Thread ble_thread_runner;
BLEService boatService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEIntCharacteristic boatCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDoubleCharacteristic boatGPSLat("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDoubleCharacteristic boatGPSLon("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEIntCharacteristic boatGPSHead("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic boatGPSSpeed("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);



/**
 * @brief Nrf24l01 radio object and config
 * 
 */
rtos::Thread radio_thread_runner;

RF24 radio(CE_PIN, CSN_PIN);

bool role;
// Set the radio number, as this device is on the boat it will be static and set to 0
bool radioNumber = 0;

bool outOfRange = false;

// Initialize a copy of the struct to hold the data
txpayload payload;

// Set the radio to be a receiver
uint8_t address[][6] = { "BOAT", "LAND" };


//Radio Queue
rtos::Thread radio_transmit_queue_runner;
rtos::MemoryPool<txpayload, 16> mpool;
rtos::Queue<txpayload, 16> radioQueue;



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
    radio_transmit_queue_runner.start(radio_transmit_queue);

    return OK;

}

/**
 * @brief The radio thread
 * 
 */
void radio_thread(){
  Serial.println("Radio thread started");
      uint8_t pipe;
    while(1){

      //set up mpool space
      txpayload *data = mpool.alloc();
      data->lat = gpsData.lat;
      data->lon = gpsData.lon;
      data->headingDeg = gpsData.headingDeg;
      data->timestamp = gpsData.unixTime;

      //add the data to the queue
      radioQueue.put(data);

      //delay for 1000ms
      rtos::ThisThread::sleep_for(1000);

      
      if (radio.available(&pipe) && role == RX) { // is there a payload? get the pipe number that received it
        radio.read(&payload, sizeof(txpayload)); // fetch payload from FIFO
        Serial.print(F("Received data on pipe "));
        Serial.print(pipe); // print the pipe number
        Serial.print(F(": "));
        Serial.print(payload.lat);
        Serial.print(F(", "));
        Serial.print(payload.lon);
       //Payload will be different for receiver

      }

    
  }
}


/**
 * @brief The radio transmit queue
 * @note Radio only goes into TX mode when there is data to send
 */
#include <cstdlib> // For random number generation. If not available, use an alternative method.

void radio_transmit_queue() {
  bool report;
  bool emptyTheQueue = false;

  while (1) {
    // If there are items in the queue and we're not already set to empty it,
    // check if we should start emptying the queue.
    if (!radioQueue.empty() && !emptyTheQueue) {
      if (radioQueue.count() > 10) {
        emptyTheQueue = true; // Set to start emptying the queue.
      } else {
        // If there are not enough items, just wait a bit and check again.
        rtos::ThisThread::sleep_for(1000); // Adjust the timing as needed.
        continue;
      }
    }

    while (!radioQueue.empty() && emptyTheQueue) {
      osEvent evt = radioQueue.get();
      if (evt.status == osEventMessage) {
        txpayload* data = (txpayload*)evt.value.p;

        int failCount = 0;
        report = false;
        while (!report && failCount <= 5) {
          role = TX; // Switch to transmit mode
          radio.stopListening();

          #ifdef DEBUG
          Serial.println(F("Transmitting data"));
          Serial.print(data->lat);
          Serial.print(",");
          Serial.print(data->lon);
          Serial.print(",");
          Serial.print(data->headingDeg);
          Serial.print(",");
          Serial.println(data->timestamp);
          #endif

          report = radio.write(data, sizeof(txpayload));

          if (!report) {
            Serial.println(F("Transmission failed or timed out"));

            // Switch to receive mode and wait for a random time between 1 to 3 seconds
            role = RX;
            radio.startListening();
            unsigned int waitTime = 1000 + rand() % 2000; // Random wait between 1000ms to 3000ms
            rtos::ThisThread::sleep_for(waitTime);
            failCount++;
          }

          if (failCount > 5) {
            Serial.println(F("Failed to transmit data after 5 attempts"));
          }
        }

        // Switch to receive mode
        role = RX;

        // Free the memory used by the message
        mpool.free(data);
      }



      // If the queue is empty, stop emptying it.
      if (radioQueue.empty()) {
        emptyTheQueue = false;
      }

      // Wait a bit before checking the queue again
      rtos::ThisThread::sleep_for(1000); // Adjust the timing as needed.

    } 

  }

}