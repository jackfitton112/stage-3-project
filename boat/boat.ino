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

RF24 radio(CE_PIN, CSN_PIN);

bool role;
// Set the radio number, as this device is on the boat it will be static and set to 0
const bool radioNumber = 0;

// Define the struct to hold the data
struct txpayload {
    double lat;
    double lon;
    int headingDeg;
    int timestamp;
};

// Initialize a copy of the struct to hold the data
txpayload payload;

// Set the radio to be a receiver
uint8_t address[][6] = { "BOAT", "LAND" };

void setup() {
    Serial.begin(115200); // Open serial port

    while(!Serial); // Wait for serial to open

    // Setup GPS
    if (setup_gps() != OK) {
        Serial.println(F("GPS setup failed"));
        while (1);
    }

    if(setup_comms() != OK){
        Serial.println(F("Comms setup failed"));
        while(1);
    }

    if(!radio.begin()){
        Serial.println("Radio setup failed");
        while(1);
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
}

void loop() {
  if (role) {
    // This device is a TX node

    //write the gps data to the struct to be sent
    payload.lat = gpsData.lat;
    payload.lon = gpsData.lon;
    payload.headingDeg = gpsData.headingDeg;
    payload.timestamp = gpsData.unixTime;

    unsigned long start_timer = micros(); // start the timer
    bool report = radio.write(&payload, sizeof(txpayload)); // transmit & save the report
    unsigned long end_timer = micros(); // end the timer

    if (report) {
      Serial.println(F("Transmission successful!")); // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer); // print the timer result
      Serial.println(F(" us."));
      // No need to increment payload here as it's a structured data now, should be updated elsewhere as needed
    } else {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }

    delay(1000); // slow transmissions down by 1 second

  } else {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) { // is there a payload? get the pipe number that received it
      radio.read(&payload, sizeof(txpayload)); // fetch payload from FIFO
      Serial.print(F("Received data on pipe "));
      Serial.print(pipe); // print the pipe number
      Serial.print(F(": Lat="));
      Serial.print(payload.lat); // print latitude
      Serial.print(F(", Lon="));
      Serial.print(payload.lon); // print longitude
      Serial.print(F(", Heading="));
      Serial.print(payload.headingDeg); // print heading
      Serial.print(F(", Timestamp="));
      Serial.println(payload.timestamp); // print timestamp
    }
  } // role

  if (Serial.available()) {
    char c = toupper(Serial.read());
    if (c == 'T' && !role) {
      role = true;
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      radio.stopListening();
    } else if (c == 'R' && role) {
      role = false;
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      radio.startListening();
    }
  }
} // loop
