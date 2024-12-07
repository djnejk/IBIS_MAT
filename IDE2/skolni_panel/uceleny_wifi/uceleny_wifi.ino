#include <Arduino.h>
#include "wifi_helper.h"
#include "internet_checker.h"

// Deklarace globálních proměnných pro stav připojení
bool isWifi = false; 
bool isInternet = false;

void setup() {
  Serial.begin(115200);

  // Připojení k WiFi
  connectToWiFi();

  // Kontrola přístupu k internetu
  isInternet = checkInternetAccess();
  if (isInternet) {
    Serial.println("Přístup k internetu je dostupný.");
  } else {
    Serial.println("Přístup k internetu není dostupný.");
  }
}

void loop() {
  // Každou sekundu kontrolujeme stav WiFi
  static unsigned long lastCheckTime = 0;
  if (millis() - lastCheckTime >= 1000) {
    lastCheckTime = millis();

    if (isWiFiConnected()) {
      bool internetStatus = checkInternetAccess();
      if (internetStatus != isInternet) {
        isInternet = internetStatus;
        if (isInternet) {
          Serial.println("Přístup k internetu je nyní dostupný.");
        } else {
          Serial.println("Přístup k internetu byl ztracen.");
        }
      }
    }
  }

  // Pokud není připojení k WiFi, zkoušíme se připojit asynchronně na pozadí
  if (!isWiFiConnected()) {
    connectToWiFi();
  }

  // Můžeme použít isWifi přímo zde pomocí getteru
  if (!getWiFiStatus()) {
    Serial.println("WiFi stále není připojeno...");
  }

  delay(10); // Malé zpoždění pro stabilizaci
}
