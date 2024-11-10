#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);

  // Inicializace NVS
  preferences.begin("my-app", false);  // Druhý argument "false" je režim čtení/zápisu

  // Zápis dat
  preferences.putString("ssid_sta", "ssid");
  preferences.putString("password_sta", "password");

 
  // Ukončení práce s NVS
  preferences.end();
}

void loop() {

}
