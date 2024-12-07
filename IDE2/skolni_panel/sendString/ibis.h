#ifndef IBIS_H
#define IBIS_H

#include <Arduino.h>

// Deklarace funkcí, které budou implementovány v ibis.cpp
void sendString(String slovo);
void sendHexString(const char* hexString);
String stringToHex(String str);
String checksum8XOR(String hexString);

#endif
