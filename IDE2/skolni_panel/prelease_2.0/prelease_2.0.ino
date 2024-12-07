#include <Arduino.h>
String main_version = "v2.0.43";

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

      if ((pocitadloMod % 20) == 0 || pocitadloMod == 2) {
        sendAsString("zD1000");
      } else if ((pocitadloMod % 10) == 0) {
        // Vytvoření formátovaného času ve formátu "h:mm"
        char buffer[6];
        if (timeInfo->tm_hour < 10) {
          sprintf(buffer, "%d:%02d", timeInfo->tm_hour, timeInfo->tm_min);
        } else {
          sprintf(buffer, "%d:%02d", timeInfo->tm_hour, timeInfo->tm_min);
        }
        if (pocitadloMod % 2 == 0) {
          sendAsString("zN" + String(buffer));
        } else {
          String modifiedBuffer = String(buffer);  // Převedeme buffer na String
          modifiedBuffer.replace(":", ";");        // Provedeme nahrazení přímo na existujícím Stringu

          sendAsString("zN" + modifiedBuffer);  // Použijeme upravený řetězec
        }
        sendAsString("zN" + String(buffer));
      }
    } else if ((pocitadloMod > 60) && (pocitadloMod <= 90)) {
      if (timeInfo->tm_mon == 11) {  //prosinec
        if (timeInfo->tm_mon <= 24) {
          Serial.println("Do Vánoc zbývá " + String(24 - timeInfo->tm_mon) + " dní");
          if ((timeInfo->tm_mon+1) < 10) {
            sendAsString("z00" + String(timeInfo->tm_mon + 1));
          } else {
            sendAsString("z0" + String(timeInfo->tm_mon + 1));
          }
        } else {
          pocitadloMod = 0;
        }
      } else {
        pocitadloMod = 0;
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
