#include "wifi_helper.h"
#include <HTTPClient.h>

Preferences preferences;
extern bool isWifi;

void connectToWiFi() {
  preferences.begin("my-app", false);
  String ssid = preferences.getString("ssid_sta", "default");
  String password = preferences.getString("password_sta", "default");
  preferences.end();

  Serial.println(ssid);
  Serial.println(password);

  // Pokus o připojení k WiFi po dobu 10 sekund
  WiFi.begin(ssid.c_str(), password.c_str());
  // WiFi.begin(ssid.c_str());
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  // Kontrola, jestli se podařilo připojit
  if (WiFi.status() == WL_CONNECTED) {
    isWifi = true;
    Serial.println("\nWiFi připojeno!");
  } else {
    isWifi = false;
    Serial.println("\nWiFi se nepodařilo připojit.");
  }
}

bool isWiFiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!isWifi) {
      Serial.println("WiFi bylo znovu připojeno!");
    }
    isWifi = true;
  } else {
    if (isWifi) {
      Serial.println("WiFi bylo odpojeno!");
    }
    isWifi = false;
  }
  return isWifi;
}

bool getWiFiStatus() {
  return isWifi;
}
