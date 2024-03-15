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
rtos::Thread radio_transmit_adding_runner;

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
rtos::MemoryPool<txpayload, TX_QUEUE_SIZE> mpool;
rtos::Queue<txpayload, TX_QUEUE_SIZE> radioQueue;



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
    role = RX;

    // Set the radio config
    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(payload));
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]);

    Serial.println("Radio setup complete");
    
    radio.startListening(); // RX only

    radio_thread_runner.start(radio_thread);
    radio_transmit_queue_runner.start(radio_transmit_queue);
    radio_transmit_adding_runner.start(radio_transmit_data);

    return OK;

}

/**
 * @brief The radio thread
 * 
 */
void radio_thread(){
      Serial.println("Radio thread started");
      uint8_t pipe;
      rxpayload rxData;
      rxCommands currentStatus = NON;
      bool manualControl = false;
      
    while(1){

      if (radio.available(&pipe) && role == RX) { // is there a payload? get the pipe number that received it
        radio.read(&rxData, sizeof(rxpayload));
        Serial.print("Received data: ");
        rxCommands command = rxData.command;
        Serial.println(command);

        switch (command)
        {
        case RTH:
          boat_RTH();
          break;

        case OVERRIDE:
          boat_OVERRIDE(rxData);
          break;

        case APPEND:
          boat_APPEND(rxData);
          break;

        case HALT:
          boat_STOP();
          break;

        case ENTER_MANUAL:
          boat_STOP();
          boat_ENTER_MANUAL();
          manualControl = true;
          break;
        
        case MANUAL_CONTROL:
          if(manualControl){
            boat_MANUAL_CONTROL(rxData.control);
          }
          break;

        case EXIT_MANUAL:
          manualControl = false;
          break;

        case SET_HOME:
          boat_SET_HOME(rxData);
          break;

        default:
          Serial.print("Unknown command: ");
          Serial.println(command);
          break;
      }

    
    } else {
      rtos::ThisThread::sleep_for(50);
    }
  }
}

//TODO: Implement the RTH command
/**
  * @brief Return to home
  * 
  * @return int 
  */
int boat_RTH(){
  Serial.println("Returning to home");
  return OK;
}

//TODO: Implement the OVERRIDE command
/**
  * @brief Override the boat
  * 
  * @param rxData 
  * @return int 
  */
int boat_OVERRIDE(rxpayload rxData){
  Serial.println("Overriding");
  return OK;
}

//TODO: Implement the APPEND command
/**
  * @brief Append data to the boat
  * 
  * @param rxData 
  * @return int 
  */
int boat_APPEND(rxpayload rxData){
  Serial.println("Appending");
  return OK;
}

//TODO: Implement the STOP command
/**
  * @brief Stop the boat
  * 
  * @return int 
  */
int boat_STOP(){
  Serial.println("Stopping");
  return OK;
}

//TODO: Implement the ENTER_MANUAL command
/**
  * @brief Enter manual mode
  * 
  * @return int 
  */
int boat_ENTER_MANUAL(){
  Serial.println("Entering manual mode");
  return OK;
}

//TODO: Implement the MANUAL_CONTROL command
/**
  * @brief Manual control of the boat
  * 
  * @param control 
  * @return int 
  */
int boat_MANUAL_CONTROL(controlCommands control){
  Serial.println("Manual control");
  return OK;
}

/**
 * @brief Set the home location
 * 
 * @param rxData 
 * @return int 
 */
int boat_SET_HOME(rxpayload rxData){
  Serial.println("Setting home");
  return OK;
}


/**
 * @brief Print a debug message
 * 
 * @param message 
 */
void debugPrint(const String& message) {
    #ifdef DEBUG
    Serial.println(message);
    #endif
}

/**
 * @brief Switch the radio to transmit mode
 * 
 */
void txMode() {
    role = TX;
    radio.stopListening();
    //debugPrint(F("Switched to TX mode"));
}

/**
 * @brief Switch the radio to receive mode
 * 
 */
void rxMode() {
    role = RX;
    radio.startListening();
    //debugPrint(F("Switched to RX mode"));
}


/**
 * @brief Transmit data to the radio queue
 * 
 */
void radio_transmit_data(){

  while(1){

        //set up mpool space
        txpayload *data = mpool.alloc();
        data->lat = gpsData.lat * 1000000;
        data->lon = gpsData.lon * 1000000;
        data->headingDeg = gpsData.headingDeg;
        data->timestamp = gpsData.unixTime;
        data->pH = 0;
        data->turbidity = 0;
        data->temperature = 0;


        //add the data to the queue
        radioQueue.put(data);

        rtos::ThisThread::sleep_for(1000);
  }
}

/**
 * @brief Transmit data from the queue
 * 
 */
void radio_transmit_queue() {
    bool report;

    while (1) {
        // Check if there are items in the queue and whether we should start emptying it
        if (!radioQueue.empty()) {
            if (radioQueue.count() > TX_TRANSMIT_THREASHOLD) {
                debugPrint(F("Queue is full, starting to transmit data"));

                while (!radioQueue.empty()) {
                    osEvent evt = radioQueue.get();
                    if (evt.status == osEventMessage) {
                        txpayload* data = (txpayload*)evt.value.p;

                        int failCount = 0;
                        report = false;

                        while (!report && failCount <= 5) {
                            txMode();

                            debugPrint(F("Transmitting data"));
                            Serial.print(data->lat);
                            Serial.print(",");
                            Serial.print(data->lon);
                            Serial.print(",");
                            Serial.print(data->headingDeg);
                            Serial.print(",");
                            Serial.println(data->timestamp);

                            report = radio.write(data, sizeof(txpayload));
                            if (!report) {
                                Serial.println(F("Transmission failed or timed out"));

                                // Switch to receive mode and wait for a random time before trying again
                                rxMode();
                                unsigned int waitTime = 1000 + rand() % 2000;
                                rtos::ThisThread::sleep_for(waitTime);
                                failCount++;
                            }
                        }

                        if (failCount > 5) {
                            Serial.println(F("Failed to transmit data after 5 attempts"));
                        }

                        // Switch to receive mode after the attempt
                        rxMode();

                        // Free the memory used by the message
                        mpool.free(data);
                    }
                }
            } else {
                Serial.print("Queue size: ");
                Serial.println(radioQueue.count());
                rxMode();
                rtos::ThisThread::sleep_for(1000);
            }
        } else {
            //debugPrint(F("Queue is empty, waiting for data"));
            rxMode();
            rtos::ThisThread::sleep_for(1000);
        }
    }
}
