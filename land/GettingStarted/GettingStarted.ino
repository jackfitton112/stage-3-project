#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = { "BOAT", "LAND" };

struct rxpayload {
    int lat;
    int lon;
    short headingDeg;
    int timestamp;
    short pH;
    short turbidity;
    short temperature;
};

enum txCommands {
    RTH,
    OVERRIDE,
    APPEND,
    STOP,
    ENTER_MANUAL,
    MANUAL_CONTROL,
    EXIT_MANUAL,
    SET_HOME,
    NON
};

enum controlCommands {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    HALT
};

struct txpayload {
    txCommands command;
    controlCommands control;
    int lat;
    int lon;
};

txpayload txpayloaddata;
rxpayload payload;

bool radioNumber = 1;
bool role = false;  // false = RX, true = TX
int failcount = 0;  // Moved outside of the loop to maintain state

/**
 * @brief Setup function for the program
 * 
 */
void setup() {
    Serial.begin(115200);
    //while (!Serial) {}

    if (!radio.begin()) {
        Serial.println(F("radio hardware is not responding!!"));
        while (1) {}
    }

    Serial.println(F("Spooky Duck Land Control"));
    radioNumber = 1;

    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(payload));

    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]);

    if (role) {
        radio.stopListening();
    } else {
        radio.startListening();
    }
}


/**
 * @brief Main loop for the program
 * 
 */
void loop() {
    if (role) {
        unsigned long start_timer = micros();
        bool report = radio.write(&txpayloaddata, sizeof(txpayload));
        unsigned long end_timer = micros();

        if (report) {
            Serial.println(F("Transmission successful!"));
            failcount = 0;  // Reset failcount after a successful transmission
            role = false;   // Switch to RX mode after successful transmission
            radio.startListening();
        } else {
            Serial.println(F("Transmission failed or timed out"));
            failcount++;
            if (failcount > 5) {
                Serial.println(F("Too many failed attempts, exiting"));
                failcount = 0;  // Reset failcount when switching modes
                role = false;   // Ensure we switch to RX mode after too many failures
                radio.startListening();
            }
        }
        delay(1000);  // You might want to adjust or remove this delay
    } else {
        uint8_t pipe;
        if (radio.available(&pipe)) {
            uint8_t bytes = radio.getPayloadSize();
            radio.read(&payload, bytes);

            Serial.print(payload.lat);
            Serial.print(",");
            Serial.print(payload.lon);
            Serial.print(",");
            Serial.print(payload.headingDeg);
            Serial.print(",");
            Serial.print(payload.timestamp);
            Serial.print(",");
            Serial.print(payload.pH);
            Serial.print(",");
            Serial.print(payload.turbidity);
            Serial.print(",");
            Serial.println(payload.temperature);
        }
    }

    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        int data[4]; // Assuming 4 fields based on previous usage
        int index = 0;
        for (int i = 0; i < input.length(); i++) {
            int nextComma = input.indexOf(',', i);
            if (nextComma == -1) nextComma = input.length();
            String value = input.substring(i, nextComma);
            data[index++] = value.toInt();
            i = nextComma;
        }

        txpayloaddata.command = static_cast<txCommands>(data[0]);
        txpayloaddata.control = static_cast<controlCommands>(data[1]);
        txpayloaddata.lat = data[2];
        txpayloaddata.lon = data[3];

        Serial.print("Sending: ");
        Serial.print(txpayloaddata.command);
        Serial.print(",");
        Serial.print(txpayloaddata.control);
        Serial.print(",");
        Serial.print(txpayloaddata.lat);
        Serial.print(",");
        Serial.println(txpayloaddata.lon);

        role = true;  // Set to TX mode to attempt sending
        radio.stopListening();
    }
}
