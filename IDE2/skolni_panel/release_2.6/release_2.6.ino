#include <Arduino.h>
#include <WebServer.h>
#include <HTTPClient.h>


// Knihovny pro WiFi
#include "internet_checker.h"
#include "wifi_helper.h"
// Pro NTP
#include <NTPClient.h>
#include <WiFiUdp.h>
// OTA knihovny
#include "OTAUpdater.h"

// Knihovny pro IBIS
#include <HardwareSerial.h>
#include "ibis.h"

#include "JsonHandler.h"
#include "breaks.h"

const String currentVersion = "v2.6.76";


// Definování sériového portu
HardwareSerial ibisSerial(2);

WebServer server(80);
 
 

const char *apSSID = "********";
const char *apPassword = "********";
String wifiSSID; 
String wifiPassword;


// NTP klient nastavení
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);  // UTC+1 hodina, aktualizace každých 60 sekund


// Deklarace globálních proměnných pro stav připojení
bool isWifi = false;
bool isInternet = false;
bool wasInternet = false;
bool ntpSynchronized = false;  // Příznak pro úspěšnou synchronizaci NTP

int pocitadloMod = 0;



void setup() {
  pinMode(23, OUTPUT);

  // Nastavení sériové komunikace
  Serial.begin(115200);
  Serial.println("Spuštění programu");

  // Nastavení sériové komunikace pro IBIS
  ibisSerial.begin(1200, SERIAL_7E2, 16, 17);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  // ibisSerial.begin(1200);

  preferences.begin("wifi-config", true);
  wifiSSID = preferences.getString("ssid", "");
  wifiPassword = preferences.getString("password", "");
  preferences.end();

  // Prvotní vymazání panelu
  sendAsString("l000");  // linka 000
  delay(100);
  sendAsString("zD0000");  // zastávka 0000
  delay(100);
  sendAsString("z000");  // cíl 000
  delay(100);
  sendAsString("zI " + currentVersion);  //verze
  delay(100);
  // sendAsString("zN" + main_version);  //verze
  delay(100);
  sendAsString("l001");
  delay(100);

  setupAP();

  // Připojení k WiFi
  connectToWiFi();

  // Kontrola přístupu k internetu
  isInternet = checkInternetAccess();
  if (isInternet) {
    wasInternet = true;
    Serial.println("Přístup k internetu je dostupný.");
    sendAsString("l000");
    // Spuštění NTP klienta
    timeClient.begin();
    // Počáteční synchronizace času
    while (!timeClient.update()) {
      Serial.println("Čekám na synchronizaci času...");
      timeClient.forceUpdate();
      delay(1000);
    }
    ntpSynchronized = true;
    Serial.println("Čas úspěšně synchronizován!");

    // První kontrola aktualizace při startu
    OTAUpdater::checkForUpdates(currentVersion);
  } else {
    wasInternet = false;
    Serial.println("Přístup k internetu není dostupný.");
  }
}

void loop() {

  // Každou sekundu kontrolujeme stav WiFi
  static unsigned long lastCheckTime = 0;
  if (millis() - lastCheckTime >= 1000) {
    lastCheckTime = millis();
    pocitadloMod++;
    if (isWiFiConnected()) {
      bool internetStatus = checkInternetAccess();
      if (internetStatus != isInternet) {
        isInternet = internetStatus;
        if (isInternet) {
          Serial.println("Přístup k internetu je nyní dostupný.");
          sendAsString("l000");
        } else {
          Serial.println("Přístup k internetu byl ztracen.");
          sendAsString("l002");
        }
      }
    }
    // Další věci provádějící se každou sekundu

    // Aktualizace NTP klienta
    if (!timeClient.update() && !ntpSynchronized) {
      Serial.println("CHYBA: Nelze získat aktuální data z NTP serveru");
      ntpSynchronized = false;
    } else {
      ntpSynchronized = true;
    }
    Serial.println(pocitadloMod);

    // Pokud nejsou data validní, neprovádět žádné výpočty
    if (!ntpSynchronized) {
      Serial.println("Čekám na synchronizaci času...");
    }
    // Získání času jako epoch (UNIX timestamp)
    time_t rawTime = timeClient.getEpochTime();

    // Převod na čitelný formát datum a čas
    struct tm *timeInfo = localtime(&rawTime);
    if (false) {
      // Kontrola přestávky
      if (isBreak(timeInfo->tm_hour, timeInfo->tm_min)) {
        sendAsString("zI Je");
      } else {
        sendAsString("zI není");
      }

    } else if (!jsonData["sendCmd"].isNull() && jsonData["sendCmd"].size() > 0) {
      for (JsonObject cmd : jsonData["sendCmd"].as<JsonArray>()) {
        const char *command = cmd["cmd"];
        int time = cmd["time"];
        sendAsString(String(command));
        delay(time);
      }
    } else {
      if (timeInfo->tm_hour >= 6 && timeInfo->tm_hour < 17) {
        if (pocitadloMod <= 30) {

          if (((pocitadloMod <= 10) || (pocitadloMod > 20 && pocitadloMod <= 30) || (pocitadloMod > 40 && pocitadloMod <= 50)) && !isBreak(timeInfo->tm_hour, timeInfo->tm_min)) {
            sendAsString("zD1000");
          } else {
            // Vytvoření formátovaného času ve formátu "h:mm"
            String cas = "88:88";
            if (pocitadloMod % 2 == 0) {
              cas = String(timeInfo->tm_hour) + ":" + (((timeInfo->tm_min) < 10) ? "0" + String(timeInfo->tm_min) : timeInfo->tm_min);
            } else {
              cas = String(timeInfo->tm_hour) + ";" + (((timeInfo->tm_min) < 10) ? "0" + String(timeInfo->tm_min) : timeInfo->tm_min);
            }
            sendAsString("zN" + cas);  // Použijeme upravený řetězec
          }
          sendAsString("z100");
        } else if ((pocitadloMod > 30) && (pocitadloMod <= 60)) {
          // if (timeInfo->tm_mon == 11) {  //prosinec
          //   if (timeInfo->tm_mday <= 24) {
          //     Serial.println("Do Vánoc zbývá " + String(24 - timeInfo->tm_mday) + " dní");
          //     if ((timeInfo->tm_mday + 1) < 10) {
          //       sendAsString("z00" + String(timeInfo->tm_mday + 1));
          //     } else {
          //       sendAsString("z0" + String(timeInfo->tm_mday + 1));
          //     }
          //   } else {
          //     pocitadloMod = 0;
          //     Serial.println("ne1");
          //   }
          // }


          if (timeInfo->tm_mon == 0 && timeInfo->tm_mday == 25) {
            if ((pocitadloMod <= 40) || (pocitadloMod > 50 && pocitadloMod <= 60)) {
              sendAsString("zD1000");
            } else {
              // Vytvoření formátovaného času ve formátu "h:mm"
              String cas = "88:88";
              if (pocitadloMod % 2 == 0) {
                cas = String(timeInfo->tm_hour) + ":" + (((timeInfo->tm_min) < 10) ? "0" + String(timeInfo->tm_min) : timeInfo->tm_min);
              } else {
                cas = String(timeInfo->tm_hour) + ";" + (((timeInfo->tm_min) < 10) ? "0" + String(timeInfo->tm_min) : timeInfo->tm_min);
              }
              sendAsString("zN" + cas);  // Použijeme upravený řetězec
            }
            sendAsString("z103");
          } else {
            pocitadloMod = 0;
            Serial.println("ne2");
          }
        } else {
          pocitadloMod = 0;
        }
      } else {
        sendAsString("z000");
        sendAsString("l000");
        sendAsString("zD0000");
      }
    }

    // každých 10 sekund kontrola aktualizace
    if (pocitadloMod % 10 == 0) {
      // Načtení JSON dat
      fetchJsonData();
      if (!OTAUpdater::upToDate(currentVersion)) {
        Serial.println("Nová verze detekována! Aktualizace dostupná.");
        sendAsString("l001");
      }
    }
  }

  // Pokud není připojení k WiFi, zkoušíme se připojit asynchronně na pozadí
  if (!isWiFiConnected() && wasInternet) {
    connectToWiFi();
  }

  // Můžeme použít isWifi přímo zde pomocí getteru
  if (!getWiFiStatus()) {
    Serial.println("WiFi stále není připojeno...");
  }

  server.handleClient();


  delay(10);  // Malé zpoždění pro stabilizaci
}




void handleRoot() {
  String html = "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f9; color: #333; }";
  html += "h1 { background-color: #4CAF50; color: white; padding: 10px 20px; }";
  html += "form { margin: 20px; padding: 20px; background-color: white; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
  html += "label { display: block; margin-bottom: 5px; font-weight: bold; }";
  html += "input[type='text'], input[type='password'] { width: 100%; padding: 8px; margin-bottom: 10px; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }";
  html += "input[type='submit'] { background-color: #4CAF50; color: white; border: none; padding: 10px 20px; cursor: pointer; border-radius: 4px; }";
  html += "input[type='submit']:hover { background-color: #45a049; }";
  html += "</style></head><body><h1>ESP32 Radio Config</h1>";
  html += "<form action='/save' method='POST'>";
  html += "<label>WiFi SSID:</label><input type='text' name='ssid' value='" + wifiSSID + "'><br>";
  html += "<label>WiFi Password:</label><input type='password' name='password' value='" + wifiPassword + "'><br>";
  html += "<input type='submit' value='Save'></form></body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    wifiSSID = server.arg("ssid");
    wifiPassword = server.arg("password");

    preferences.begin("wifi-config", false);
    preferences.putString("ssid", wifiSSID);
    preferences.putString("password", wifiPassword);
    preferences.end();

    server.send(200, "text/html", "<html><body><h1>Settings saved! Rebooting...</h1></body></html>");
    delay(2000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "<html><body><h1>Missing parameters!</h1></body></html>");
  }
}

void setupAP() {
  WiFi.softAP(apSSID, apPassword);
  IPAddress local_IP(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.println("HTTP server started.");
}