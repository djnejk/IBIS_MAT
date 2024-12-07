#include <Arduino.h>
#include <HardwareSerial.h>
#include "ibis.h"

// Definování sériového portu
HardwareSerial mySerial(0);

void setup() {
  mySerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
}

void loop() {
  // Ukázkové volání funkce sendString
  sendString("Hello");
  delay(1000); // Přidána prodleva pro lepší sledování komunikace
}
