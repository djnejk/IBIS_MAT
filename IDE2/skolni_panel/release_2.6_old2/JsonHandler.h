#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifi_helper.h" // Přidán include na wifi_helper

// Deklarace globální proměnné
extern DynamicJsonDocument jsonData;

// Prototyp funkce
void fetchJsonData();

#endif // JSON_HANDLER_H
