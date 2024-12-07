#include <Arduino.h>
String main_version = "v2.3.26";

// Knihovny pro WiFi
#include "internet_checker.h"
#include "wifi_helper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// Knihovny pro IBIS
#include <HardwareSerial.h>

#include "ibis.h"

// Definování sériového portu
HardwareSerial ibisSerial(0);


// NTP klient nastavení
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);  // UTC+1 hodina, aktualizace každých 60 sekund


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
  // sendAsString("zN" + main_version);  //verze
  delay(100);
  sendAsString("l001");
  delay(100);


  // Připojení k WiFi
  connectToWiFi();

  // Kontrola přístupu k internetu
  isInternet = checkInternetAccess();
  if (isInternet) {
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
  } else {
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
    struct tm* timeInfo = localtime(&rawTime);
    if (pocitadloMod <= 30) {
      sendAsString("z100");

      if ((pocitadloMod <= 10) || (pocitadloMod > 20 && pocitadloMod <= 30) || (pocitadloMod > 40 && pocitadloMod <= 50)) {
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
    } else if ((pocitadloMod > 30) && (pocitadloMod <= 60)) {
      if (timeInfo->tm_mon == 11) {  //prosinec
        if (timeInfo->tm_mday <= 24) {
          Serial.println("Do Vánoc zbývá " + String(24 - timeInfo->tm_mday) + " dní");
          if ((timeInfo->tm_mday + 1) < 10) {
            sendAsString("z00" + String(timeInfo->tm_mday + 1));
          } else {
            sendAsString("z0" + String(timeInfo->tm_mday + 1));
          }
        } else {
          pocitadloMod = 0;
          Serial.println("ne1");
        }
      } else {
        pocitadloMod = 0;
        Serial.println("ne2");
      }
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


  delay(10);  // Malé zpoždění pro stabilizaci
}


String getFormattedTime(time_t rawTime) {
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
