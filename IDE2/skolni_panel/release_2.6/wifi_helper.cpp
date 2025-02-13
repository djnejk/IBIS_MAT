#include "wifi_helper.h"
#include <HTTPClient.h>
#include "ibis.h"

Preferences preferences;  // Jediná definice globální proměnné

extern bool isWifi;

void connectToWiFi() {
  preferences.begin("wifi-config", false);
  String ssid = preferences.getString("ssid", "default");
  String password = preferences.getString("password", "default");
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

    // Výpis lokální IP adresy
    Serial.print("Lokální IP adresa: ");
    Serial.println(WiFi.localIP());
    sendAsString("lIP:" + String(WiFi.localIP()));
    delay(1000);
    // Výpis MAC adresy
    Serial.print("MAC adresa: ");
    Serial.println(WiFi.macAddress());
    sendAsString("MAC:" + String(WiFi.macAddress()));
    delay(1000);
    // Získání a výpis veřejné IP adresy
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin("http://api.ipify.org");  // Použití API pro získání veřejné IP
      int httpCode = http.GET();
      if (httpCode > 0) {
        String publicIP = http.getString();
        Serial.print("Veřejná IP adresa: ");
        Serial.println(publicIP);
        sendAsString("pIP:" + String(publicIP));
      } else {
        Serial.println("Nepodařilo se získat veřejnou IP adresu");
      }
      http.end();
    }


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
