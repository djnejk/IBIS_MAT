String main_version = "v2.1.36";

#include <Arduino.h>

// Knihovny pro WiFi
#include "internet_checker.h"
#include "wifi_helper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_system.h>  // Knihovna potřebná pro funkci esp_restart()
#include <WebServer.h>

// Knihovny pro IBIS
#include <HardwareSerial.h>

#include "ibis.h"



// Definování sériového portu
HardwareSerial ibisSerial(0);


// NTP klient nastavení
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);  // UTC+1 hodina, aktualizace každých 60 sekund

// Nastavení pro Access Point
const char* ssid_ap = "Prevodnik_IBIS";
const char* password_ap = "ibis1234";
// Nastavení statické IP adresy, brány a masky sítě
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Vytvoření webového serveru na portu 80
WebServer server(80);

// Deklarace globálních proměnných pro stav připojení
bool isWifi = false;
bool isInternet = false;
bool ntpSynchronized = false;  // Příznak pro úspěšnou synchronizaci NTP

int pocitadloMod = 0;



void setup() {
  pinMode(23, OUTPUT);

  // Nastavení sériové komunikace
  Serial.begin(1200);
  Serial.println("Spuštění programu");

  // Nastavení sériové komunikace pro IBIS
  ibisSerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  // ibisSerial.begin(1200);

  // Prvotní vymazání panelu
  sendAsString("l000");  // linka 000
  delay(100);
  sendAsString("zD0000");  // zastávka 0000
  delay(100);
  sendAsString("z000");  // cíl 000
  delay(100);
  sendAsString("zI" + main_version);  //verze
  delay(100);
  sendAsString("zN" + main_version);  //verze


  // Nastavení ESP32 jako Access Point
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid_ap, password_ap);

  // Připojení k WiFi
  connectToWiFi();

  // Kontrola přístupu k internetu
  isInternet = checkInternetAccess();
  if (isInternet) {
    Serial.println("Přístup k internetu je dostupný.");
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
  } else {
    Serial.println("Přístup k internetu není dostupný.");
  }

  // Nastavení obsluhy webového serveru
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
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
        } else {
          Serial.println("Přístup k internetu byl ztracen.");
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

    // Pokud nejsou data validní, neprovádět žádné výpočty
    if (!ntpSynchronized) {
      Serial.println("Čekám na synchronizaci času...");
    }
    // Získání času jako epoch (UNIX timestamp)
    time_t rawTime = timeClient.getEpochTime();

    // Převod na čitelný formát datum a čas
    struct tm* timeInfo = localtime(&rawTime);
    if (pocitadloMod <= 60) {
      sendAsString("z100");
      if ((pocitadloMod % 20) == 0) {
        sendAsString("zD1000");
      } else if ((pocitadloMod % 10) == 0) {
        // Vytvoření formátovaného času ve formátu "h:mm"
        char buffer[6];
        if (timeInfo->tm_hour < 10) {
          sprintf(buffer, "%d:%02d", timeInfo->tm_hour, timeInfo->tm_min);
        } else {
          sprintf(buffer, "%02d:%02d", timeInfo->tm_hour, timeInfo->tm_min);
        }
        sendAsString("zN" + String(buffer));
      }
    } else if ((pocitadloMod > 60) && (pocitadloMod <= 90)) {
      String slovo = "z000";
      if (timeInfo->tm_mon == 11) {  // Prosinec (měsíce jsou 0-11)
        if (timeInfo->tm_mday == 1) {
          Serial.println("Do Vánoc zbývá 23 dní");
          slovo = "z002";
        } else if (timeInfo->tm_mday == 2) {
          Serial.println("Do Vánoc zbývá 22 dní");
          slovo = "z003";
        } else if (timeInfo->tm_mday == 3) {
          Serial.println("Do Vánoc zbývá 21 dní");
          slovo = "z004";
        } else if (timeInfo->tm_mday == 4) {
          Serial.println("Do Vánoc zbývá 20 dní");
          slovo = "z005";
        } else if (timeInfo->tm_mday == 5) {
          Serial.println("Do Vánoc zbývá 19 dní");
          slovo = "z006";
        } else if (timeInfo->tm_mday == 6) {
          Serial.println("Do Vánoc zbývá 18 dní");
          slovo = "z007";
        } else if (timeInfo->tm_mday == 7) {
          Serial.println("Do Vánoc zbývá 17 dní");
          slovo = "z008";
        } else if (timeInfo->tm_mday == 8) {
          Serial.println("Do Vánoc zbývá 16 dní");
          slovo = "z009";
        } else if (timeInfo->tm_mday == 9) {
          Serial.println("Do Vánoc zbývá 15 dní");
          slovo = "z010";
        } else if (timeInfo->tm_mday == 10) {
          Serial.println("Do Vánoc zbývá 14 dní");
          slovo = "z011";
        } else if (timeInfo->tm_mday == 11) {
          Serial.println("Do Vánoc zbývá 13 dní");
          slovo = "z012";
        } else if (timeInfo->tm_mday == 12) {
          Serial.println("Do Vánoc zbývá 12 dní");
          slovo = "z013";
        } else if (timeInfo->tm_mday == 13) {
          Serial.println("Do Vánoc zbývá 11 dní");
          slovo = "z014";
        } else if (timeInfo->tm_mday == 14) {
          Serial.println("Do Vánoc zbývá 10 dní");
          slovo = "z015";
        } else if (timeInfo->tm_mday == 15) {
          Serial.println("Do Vánoc zbývá 9 dní");
          slovo = "z016";
        } else if (timeInfo->tm_mday == 16) {
          Serial.println("Do Vánoc zbývá 8 dní");
          slovo = "z017";
        } else if (timeInfo->tm_mday == 17) {
          Serial.println("Do Vánoc zbývá 7 dní");
          slovo = "z018";
        } else if (timeInfo->tm_mday == 18) {
          Serial.println("Do Vánoc zbývá 6 dní");
          slovo = "z019";
        } else if (timeInfo->tm_mday == 19) {
          Serial.println("Do Vánoc zbývá 5 dní");
          slovo = "z020";
        } else if (timeInfo->tm_mday == 20) {
          Serial.println("Do Vánoc zbývá 4 dny");
          slovo = "z021";
        } else if (timeInfo->tm_mday == 21) {
          Serial.println("Do Vánoc zbývají 3 dny");
          slovo = "z022";
        } else if (timeInfo->tm_mday == 22) {
          Serial.println("Do Vánoc zbývají 2 dny");
          slovo = "z023";
        } else if (timeInfo->tm_mday == 23) {
          Serial.println("Do Vánoc zbývá 1 den");
          slovo = "z024";
        } else if (timeInfo->tm_mday == 24) {
          Serial.println("Šťastné a Veselé Vánoce!");
          slovo = "z025";
        } else {
          slovo = "z000";
        }
      } else {
        slovo = "z000";
      }
      sendAsString(slovo);
    } else {
      pocitadloMod = 0;
    }
  }

  // Pokud není připojení k WiFi, zkoušíme se připojit asynchronně na pozadí
  if (!isWiFiConnected()) {
    connectToWiFi();
  }

  // Můžeme použít isWifi přímo zde pomocí getteru
  if (!getWiFiStatus()) {
    Serial.println("WiFi stále není připojeno...");
  }

  // Obsluha webového serveru
  server.handleClient();

  delay(10);  // Malé zpoždění pro stabilizaci
}






// funkce pro webserver::
// funkce pro získání času:
String getTime() {
  // Získání času jako epoch (UNIX timestamp)
  time_t rawTime = timeClient.getEpochTime();

  // Převod na čitelný formát datum a čas
  struct tm* timeInfo = localtime(&rawTime);


  // Vytvoření formátovaného času ve formátu "h:mm"
  char buffer[6];
  if (timeInfo->tm_hour < 10) {
    sprintf(buffer, "%d:%02d", timeInfo->tm_hour, timeInfo->tm_min);
  } else {
    sprintf(buffer, "%02d:%02d", timeInfo->tm_hour, timeInfo->tm_min);
  }

  // Vrátíme formátovaný čas jako řetězec
  return String(buffer);
}
// Funkce pro zobrazení webové stránky
void handleRoot() {
  String currentTime = isInternet ? getTime() : "Není přístup k internetu";
  preferences.begin("my-app", false);
  String pre_ssid_sta = preferences.getString("ssid_sta", "null").c_str();
  String pre_password_sta = preferences.getString("password_sta", "null").c_str();

  String html =
    "<html><head><meta charset=\"UTF-8\"><style>"
    "table { width: 100%; border-collapse: collapse; }"
    "table, th, td { border: 1px solid black; }"
    "th, td { padding: 10px; text-align: left; }"
    "</style><script>"
    "function togglePassword() {"
    "  var x = document.getElementById('passwordInput');"
    "  if (x.type === 'password') {"
    "    x.type = 'text';"
    "  } else {"
    "    x.type = 'password';"
    "  }"
    "}"
    "</script></head><body><h1>Nastavení WIFI</h1>"
    + currentTime + "<form action=\"/save\" method=\"POST\">"
                    "ssid_sta: <input type=\"text\" name=\"ssid_sta\" value=\""
    + pre_ssid_sta + "\"><br>"
                     "password_sta: <input type=\"password\" id=\"passwordInput\" name=\"password_sta\" value=\""
    + pre_password_sta + "\"><button type=\"button\" onclick=\"togglePassword()\">Show/Hide</button><br>"
                         "<input type=\"submit\" value=\"Uložit\">"
                         "</form>";

  html += "</form></body></html>";

  server.send(200, "text/html", html);
}


// Funkce pro zpracování uložených hodnot
void handleSave() {
  if (server.hasArg("ssid_sta")) {
    // Inicializace Preferences s názvem prostoru "my-app"
    preferences.begin("my-app", false);
    preferences.putString("ssid_sta", server.arg("ssid_sta"));
    // Ukončení práce s Preferences
    preferences.end();
  }
  if (server.hasArg("password_sta")) {
    // Inicializace Preferences s názvem prostoru "my-app"
    preferences.begin("my-app", false);
    preferences.putString("password_sta", server.arg("password_sta"));
    // Ukončení práce s Preferences
    preferences.end();
  }
  // Zobrazení potvrzovací zprávy
  String html = "<html><head><meta charset=\"UTF-8\"><meta http-equiv=\"refresh\" content=\"1; url=/\" /></head><body><h1>Změna údajů sítě WiFi úspěšná, provádím restart ESP32...</h1></body></html>";
  server.send(200, "text/html", html);
  delay(5000);    // Krátká prodleva před restartem
  esp_restart();  // Restartuj ESP32
}
