/**
 * ----------------------------------------------------------------------------
 * TODO doc
 * ----------------------------------------------------------------------------
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
#define RST_PIN 9
// Configurable, see typical pin layout above
#define SS_PIN 10

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

/**
 * Initialize.
 */
void setup() {
    // Initialize serial communications with the PC
    Serial.begin(9600);
    // Do nothing if no serial port is opened (added for Arduinos based on
    // ATMEGA32U4)
    while (!Serial);
    // Init SPI bus
    SPI.begin();
    // Init MFRC522 card
    mfrc522.PCD_Init();
    // Show details of PCD - MFRC522 Card Reader details
    mfrc522.PCD_DumpVersionToSerial();

    // Prepare the key (used both as key A and as key B) using FFFFFFFFFFFF
    // which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("====================================================="));
    Serial.println(F("RESET CARD"));
    Serial.println(F("====================================================="));
    Serial.print(F("Using key:"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
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

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
            && piccType != MFRC522::PICC_TYPE_MIFARE_1K
            && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // Empty data block (all zeros)
    MFRC522::StatusCode status;
    byte trailerBlock = 7;
    byte dataBlock[] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Write data to the block (WARNING: but careful where you write)
    byte block = 4;
    Serial.print(F("Writing data into block "));
    Serial.print(block);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(
            block, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.println(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }

    Serial.println(F("Write finished"));

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();

    Serial.println(F("Exiting"));
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
