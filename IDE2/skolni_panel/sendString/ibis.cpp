#include "ibis.h"
#include <HardwareSerial.h>

// Předpokládáme, že mySerial byl deklarován externě v hlavním souboru
extern HardwareSerial mySerial;

void sendString(String slovo) {
  String slovoHEX = stringToHex(slovo);
  String HEXproSUM = String("7f") + slovoHEX + "0D";
  String CheckSUM = checksum8XOR(HEXproSUM);
  String hotovyHEX = HEXproSUM.substring(2) + CheckSUM;

  // Převést String objekt na C-string
  char hexString[hotovyHEX.length() + 1];
  hotovyHEX.toCharArray(hexString, sizeof(hexString));

  sendHexString(hexString);
}

void sendHexString(const char* hexString) {
  // Spočítat délku hexadecimálního řetězce
  size_t len = strlen(hexString);

  // Procházet hexadecimální řetězec a převádět každou dvojici hex číslic na byte
  for (size_t i = 0; i < len; i += 2) {
    char hexByte[3];
    hexByte[0] = hexString[i];
    hexByte[1] = hexString[i + 1];
    hexByte[2] = '\0';

    // Převést hexadecimální byte na integer
    byte b = (byte)strtol(hexByte, NULL, 16);

    // Poslat byte přes sériovou linku
    mySerial.write(b);
  }
}

// Funkce pro převod řetězce na hexadecimální reprezentaci
String stringToHex(String str) {
  String hexKodSlova = "";
  for (int i = 0; i < str.length(); i++) {
    char hex[3];
    sprintf(hex, "%02X", str.charAt(i));
    hexKodSlova += hex;
  }
  return hexKodSlova;
}

// Funkce pro výpočet kontrolního součtu pomocí XOR
String checksum8XOR(String hexString) {
  // Převést hexadecimální řetězec na pole bajtů
  int length = hexString.length() / 2;
  byte bytes[length];
  for (int i = 0; i < length; i++) {
    bytes[i] = strtol(hexString.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }

  // Inicializace kontrolního součtu
  byte checksum = 0x00;

  // Provést XOR pro všechny bajty
  for (int i = 0; i < length; i++) {
    checksum ^= bytes[i];
  }

  // Převést výsledný kontrolní součet na hexadecimální řetězec
  char hexChecksum[3];
  sprintf(hexChecksum, "%02X", checksum);
  return String(hexChecksum);
}
