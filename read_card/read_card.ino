/*
 * ----------------------------------------------------------------------------
 * TODO doc
 * 
 * Typical pin layout used:
 * ----------------------------------------------------------------------------
 *             MFRC522      Arduino
 *             Reader/PCD   Uno
 * Signal      Pin          Pin
 * ----------------------------------------------------------------------------
 * RST/Reset   RST          9
 * SPI SS      SDA(SS)      10
 * SPI MOSI    MOSI         11 / ICSP-4
 * SPI MISO    MISO         12 / ICSP-1
 * SPI SCK     SCK          13 / ICSP-3
 */

#include <SPI.h>
#include <MFRC522.h>

// Configurable, see typical pin layout above
#define RST_PIN         9
// Configurable, see typical pin layout above
#define SS_PIN          10

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

/**
 * Initialize.
 */
void setup() {
    // Initialize serial communications with the PC
    Serial.begin(9600);
    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    while (!Serial);
    // Init SPI bus
    SPI.begin();
    // Init MFRC522 card
    mfrc522.PCD_Init();
    // Show details of PCD - MFRC522 Card Reader details
    mfrc522.PCD_DumpVersionToSerial();      

    Serial.println(F("READ ALL THE DATA."));
}

/**
 * Main loop.
 */
void loop() {
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // Dump debug info about the card; PICC_HaltA() is automatically called
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

