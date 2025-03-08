#include "internet_checker.h"
#include <HTTPClient.h>
#include <WiFi.h>

bool checkInternetAccess() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://www.google.com");
    int httpCode = http.GET();
    http.end();
    return (httpCode > 0); // Pokud je odpověď kladná, máme přístup k internetu
  }
  return false;
}
