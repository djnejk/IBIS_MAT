#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);

  // Inicializace NVS
  preferences.begin("my-app", false);  // Druhý argument "false" je režim čtení/zápisu

  // Zápis dat
  preferences.putString("ssid_sta", "ssid");
  preferences.putString("password_sta", "password");

 
Serial.println(preferences.getString("ssid_sta", "default"));
Serial.println(preferences.getString("password_sta", "default"));
  // Ukončení práce s NVS
  preferences.end();
}

void loop() {

}
