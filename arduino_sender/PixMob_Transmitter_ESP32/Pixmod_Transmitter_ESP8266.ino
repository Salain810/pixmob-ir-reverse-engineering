
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// IR LED connected to GPIO4 (D2 on D1 Mini)
const uint16_t kIrLed = 4;

String incomingString = "";
IRsend irsend(kIrLed);

void setup() {
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
}

void loop() {
  if (Serial.available() > 0) {
    incomingString = Serial.readStringUntil('['); // read until [
    incomingString = Serial.readStringUntil(']'); // read until ]
    int newLength = incomingString.toInt();
    uint16_t newRawData[newLength] = {};

    String newVals = Serial.readStringUntil(',');
    for (int i = 0; i < newVals.length(); i++) {
      int intVal = newVals.substring(i, i + 1).toInt() * 700;
      newRawData[i] = intVal;
    }
    irsend.sendRaw(newRawData, newLength, 38);  // Send a raw data capture at 38kHz.
    delay(3);
  }
}
