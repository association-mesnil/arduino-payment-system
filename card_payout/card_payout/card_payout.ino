#include <SPI.h>

#define PAYOUT_PIN 2
#define RESET_PIN 3
#define VD_PIN 4

void setup() {
  // Initialize serial communications with the PC
  Serial.begin(9600);
  // Do nothing if no serial port is opened (added for Arduinos based on
  // ATMEGA32U4)
  while (!Serial);
  // Init SPI bus
  SPI.begin();

  Serial.println(F("====================================================="));
  Serial.println(F("STARTED"));
  Serial.println(F("====================================================="));

  // Sets output pins
  pinMode(PAYOUT_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(VD_PIN, OUTPUT);

  // Send reset
//  Serial.println(F("Reset"));
//  digitalWrite(RESET_PIN, HIGH);
//  delay(1000);
//  digitalWrite(RESET_PIN, LOW);
}

void loop() {
  // Read the input OUTOK (card ready signal high if ready)
//  int sensorValue = analogRead(A0);
//  float voltage = sensorValue * (5.0 / 1023.0);
//  while (voltage < 2) {
//    Serial.print("Voltage: ");
//    Serial.println(voltage);
//    digitalWrite(RESET_PIN, HIGH);
//    delay(1000);
//    digitalWrite(RESET_PIN, LOW);
//  }

  // Send reset
  digitalWrite(RESET_PIN, HIGH);
  delay(1000);
  digitalWrite(RESET_PIN, LOW);
  delay(1000);

  // Send payout and VD at the same time (send card into slot)
  Serial.println(F("Payout card"));
  digitalWrite(PAYOUT_PIN, HIGH);
//  digitalWrite(VD_PIN, HIGH);
  delay(1000);
  digitalWrite(PAYOUT_PIN, LOW);
//  digitalWrite(VD_PIN, LOW);
//  delay(1000);

  // Send payout and VD at the same time (send card into print area)
//  Serial.println(F("Payout card"));
//  digitalWrite(PAYOUT_PIN, HIGH);
//  digitalWrite(VD_PIN, HIGH);
//  delay(500);
//  digitalWrite(PAYOUT_PIN, LOW);
//  digitalWrite(VD_PIN, LOW);
//  delay(1000);
//
//  // Send payout and VD at the same time (send card outside)
//  Serial.println(F("Payout card"));
//  digitalWrite(PAYOUT_PIN, HIGH);
//  digitalWrite(VD_PIN, HIGH);
//  delay(500);
//  digitalWrite(PAYOUT_PIN, LOW);
//  digitalWrite(VD_PIN, LOW);
//  delay(1000);

  // Wait (TODO not necessary)
  Serial.println(F("Waiting 5 sec"));
  delay(5000);
}
