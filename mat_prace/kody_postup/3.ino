#include <Preferences.h>

Preferences preferences;

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>

const float currentVersion = 7.0;  // Aktuální verze nahraná na ESP32
const char* versionURL = "https://api.djdevs.eu/testota/version.json";

void setup() {
  Serial.begin(115200);

  // Inicializace NVS
  preferences.begin("my-app", false);  // Druhý argument "false" je režim čtení/zápisu
  String ssid = preferences.getString("ssid_sta", "default");
  String password = preferences.getString("password_sta", "default");
  Serial.println(ssid);
  Serial.println(password);
  WiFi.begin(ssid, password);

  Serial.print("Připojuji k Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi připojeno!");

  checkForUpdates();  // Zkontroluje aktualizace při startu
}

void loop() {
  // Hlavní kód aplikace – tady můžeš dělat cokoli
  // Blikání LED
  digitalWrite(2, HIGH);  // Zapnout LED
  Serial.println("verze " + String(currentVersion));
  delay(500);            // Čekat 500 ms
  digitalWrite(2, LOW);  // Vypnout LED
  delay(500);            // Čekat 500 ms
}

void checkForUpdates() {
  HTTPClient http;
  http.begin(versionURL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {  // Pokud je odpověď OK
    String payload = http.getString();
    Serial.println("Dostažen JSON:");
    Serial.println(payload);

    StaticJsonDocument<256> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, payload);

    if (!error) {
      float newVersion = jsonDoc["version"];
      const char* downloadURL = jsonDoc["download"];

      Serial.printf("Aktuální verze: %.1f, Nová verze: %.1f\n", currentVersion, newVersion);

      if (newVersion > currentVersion) {
        Serial.println("Nová verze dostupná! Spouštím aktualizaci...");
        performOTAUpdate(downloadURL);
      } else {
        Serial.println("Zařízení je aktuální.");
      }
    } else {
      Serial.println("Chyba při parsování JSON.");
    }
  } else {
    Serial.printf("Chyba při přístupu na URL: %s, HTTP kód: %d\n", versionURL, httpCode);
  }
  http.end();
}

void performOTAUpdate(const char* downloadURL) {
  HTTPClient http;
  http.begin(downloadURL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient& client = http.getStream();

    if (Update.begin(contentLength)) {
      Serial.println("Začíná OTA aktualizace...");

      int written = 0;
      int prevProgress = 0;
      uint8_t buffer[1024];  // Buffer pro bloky dat

      while (client.connected() && written < contentLength) {
        size_t bytesAvailable = client.available();
        if (bytesAvailable > 0) {
          int bytesRead = client.readBytes(buffer, min((size_t)bytesAvailable, sizeof(buffer)));
          if (bytesRead > 0) {
            size_t bytesWritten = Update.write(buffer, bytesRead);
            if (bytesWritten > 0) {
              written += bytesWritten;

              // Výpočet a výpis procentuálního průběhu
              int progress = (written * 100) / contentLength;
              if (progress != prevProgress) {
                Serial.printf("Progres: %d%%\n", progress);
                prevProgress = progress;
              }
            } else {
              Serial.println("Chyba při zápisu dat!");
              break;
            }
          }
        }
      }

      if (Update.end()) {
        if (Update.isFinished()) {
          Serial.println("OTA aktualizace byla úspěšná!");
          Serial.println("Restart zařízení...");
          ESP.restart();
        } else {
          Serial.println("Aktualizace nedokončena.");
        }
      } else {
        Serial.printf("Chyba při ukončení aktualizace: %s\n", Update.errorString());
      }
    } else {
      Serial.println("Není dostatek místa pro OTA update.");
    }
  } else {
    Serial.printf("Chyba při stahování binárního souboru, HTTP kód: %d\n", httpCode);
  }
  http.end();
}


