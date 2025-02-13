#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <WiFi.h>
#include <Preferences.h>

extern Preferences preferences; // Deklarace globální proměnné

void connectToWiFi();
bool isWiFiConnected();
bool getWiFiStatus();

#endif // WIFI_HELPER_H
