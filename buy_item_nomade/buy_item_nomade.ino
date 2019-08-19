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

// Relay pin (input)
#define RELAY_PIN_IN A5
// Relay pin (output)
#define RELAY_PIN_OUT 8
// RFID card reset pin (output)
#define RST_PIN 9
// RFID card ss pin (output)
#define SS_PIN 10

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Authentication key
MFRC522::MIFARE_Key key;

// Blocks to write (WARNING: be careful where you write)
const byte block = 4;
const byte trailerBlock = 7;

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

  // Debug info
  Serial.println(F("====================================================="));
  Serial.println(F("BUY ITEM"));
  Serial.println(F("====================================================="));
  Serial.print(F("Using key:"));
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);

  // Set relay pin to output
  pinMode(RELAY_PIN_OUT, OUTPUT);

  // Set relay pin to intput
  pinMode(RELAY_PIN_IN, INPUT_PULLUP);
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

  // Debug info
  Serial.println(F("-----------------------------------------------------"));
  Serial.println(F("A new card has appeared"));
  Serial.println(F("-----------------------------------------------------"));

  // Check block value
  if (block != 4) {
    Serial.print(F("Invalid block, are you sure you want to change it?: "));
    Serial.println(block);
    halt();
    return;
  }

  // Check if relay is active
  int relayPinIn = analogRead(RELAY_PIN_IN);
  float voltage = relayPinIn * (5.0 / 1023.0);
  float minVoltage = 2.0;
  Serial.print(F("Current voltage is ("));
  Serial.print(voltage);
  Serial.println(F(")"));
  if (voltage < minVoltage) {
    Serial.println(F("RELAY NOT ACTIVATED"));
    halt();
    return;
  }

  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      && piccType != MFRC522::PICC_TYPE_MIFARE_1K
      && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    halt();
    return;
  }

  // Initialization
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  Serial.println(F("Authenticating using key A"));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    halt();
    return;
  }

  // Read data from the block
  Serial.print(F("Reading amount from card (block "));
  Serial.print(block);
  Serial.println(F(")"));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    halt();
    return;
  }
  dump_byte_array_decimal(buffer, 16);
  dump_byte_array(buffer, 16);

  // Check if remaining card amount is enough
  int amount = (int) buffer[15];
  if (amount > 0) {
    Serial.print(F("Balance OK (card amount: "));
    Serial.print(amount);
    Serial.println(F(")"));
  } else {
    Serial.print(F("Balance KO (card amount: "));
    Serial.print(amount);
    Serial.println(F(")"));
    halt();
    return;
  }

  // Calculate balance and write to card
  int balance = amount - 1;
  byte balanceData[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, byte(balance),
  };
  Serial.print(F("Writing "));
  Serial.print(balance);
  Serial.print(F(" on card (block "));
  Serial.print(block);
  Serial.println(F(")"));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(
      block, balanceData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    halt();
    return;
  }
  Serial.println(F("Write finished"));

  // Send signal to relay pin
  Serial.println(F("SENDING RELAY SIGNAL HIGH (500 ms)"));
  digitalWrite(RELAY_PIN_OUT, HIGH);

  // Pooling until either the cup signal is down or the timeout is reached,
  // with timeout being the service time is ms.  Calculating timeout with an
  // iteration (eg 20 iterations of 50 ms gives 1000 timeout) since its a pain
  // to get current time on arduino
  int serviceTimeMs = 2000;
  int pollTimeMs = 50;
  int totalIterations = serviceTimeMs / pollTimeMs;
  int iteration = 0;
  while (voltage > minVoltage && iteration < totalIterations) {
    Serial.print(iteration);
    Serial.print(F(" "));
    // Cup signal still active and timeout not reached, waiting a bit
    delay(pollTimeMs);
    // Updating values, reread cup signal for next iteration
    iteration = iteration + 1;
    relayPinIn = analogRead(RELAY_PIN_IN);
    voltage = relayPinIn * (5.0 / 1023.0);
  }

  Serial.println(F(" "));
  Serial.print(F("Exiting with iteration ("));
  Serial.print(iteration);
  Serial.print(F(") voltage ("));
  Serial.print(voltage);
  Serial.print(F(") total time ("));
  Serial.print(iteration * pollTimeMs);
  Serial.println(F(")"));

  // Stop signal to relay PIN
  Serial.println(F("SENDING RELAY SIGNAL LOW"));
  digitalWrite(RELAY_PIN_OUT, LOW);

  // Halting this loop
  delay(100);
  halt();
}

/**
 * Halts the current loop.
 */
void halt() {
    Serial.println(F("Halting loop"));
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

/**
 * Dump a byte array as hex values to serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

/**
 * Dump a byte array as decimal to serial.
 */
void dump_byte_array_decimal(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] != 0x00) {
      Serial.print(buffer[i], DEC);
    }
  }
  Serial.println(F(""));
}

