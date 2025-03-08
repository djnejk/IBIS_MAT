#include "JsonHandler.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// URL pro stažení JSON dat
const char* url = "https://api.djdevs.eu/firmware/ladislav/version.json";

// Definice globální proměnné
DynamicJsonDocument jsonData(1024);

void fetchJsonData() {
    // Ověření WiFi připojení pomocí funkce z wifi_helper.cpp
    if (!isWiFiConnected()) {
        Serial.println("WiFi není připojeno, nelze načíst JSON.");
        return;
    }

    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DeserializationError error = deserializeJson(jsonData, payload);

            if (error) {
                Serial.printf("Deserialization error: %s\n", error.c_str());
                jsonData.clear(); // Vymaže data při chybě
            } else {
                Serial.println("JSON data načtena:");
                serializeJson(jsonData, Serial);
                Serial.println();
            }
        }
    } else {
        Serial.printf("HTTP GET failed: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}