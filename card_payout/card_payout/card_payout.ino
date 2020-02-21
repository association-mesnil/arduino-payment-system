//#include <SPI.h>

#define PAYOUT_PIN 2
#define RESET_PIN 3
#define VD_PIN 4

#define OUT_OK_PIN  9 // card on or get card
#define LOWER_PIN  10 // less card
#define ERROR_PIN  11 // pr-dispense card erroror block card
#define EMPTY_PIN  12 // card box empty or collecting box empty

int incomingByte = 0; 
String readStream; 

void setup() {
  // Initialize serial communications with the PC
  Serial.begin(9600);
  // Do nothing if no serial port is opened (added for Arduinos based on
  // ATMEGA32U4)
  while (!Serial);
  // Init SPI bus
  // SPI.begin();

  // Sets output pins
  pinMode(PAYOUT_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(VD_PIN, OUTPUT);

  pinMode(13,OUTPUT);

  Serial.write("Beginning loop\n");
}

void loop() {
  while(Serial.available() > 0) {
    digitalWrite(13,HIGH);
    readStream = Serial.readString();
    Serial.println("I received: \'" + readStream + "\'");
    
    if (readStream == "dispense\n" || readStream == "dispense") {
      Serial.println("Dispensing card");
      dispendCard();
    }
    
    delay(2000);
  }

  digitalWrite(13,LOW);
}
