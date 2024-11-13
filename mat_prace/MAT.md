

>4.11.2024
- při hodinách MTR se věnuji programování panelů a zjišťování fungování MNEMO jazyka

>8.11.2024
- navrhuji novou verzi DPS IBIS převodníku, která je kompatibilní s vývojovou deskou ESP WROOM 32 (easyeda: v2)

>10.11.2024
- Zkouším novou vývojovou desku ESP WROOM 32
- zkouším rozchodit ArduinoOTA (over the internet), který mi umožní nahrávat program do ESP vzdáleně
- nejdřív zkouším nahrávání pomocí lokální sítě (kod: 1)
- zkouším zprovoznit a ucelit dálkové nahrávání OTA z online zdroje "online aktualizace zařízení" (kod: 3, serial: 1)
- z důvodu zachování citlivých údajů jako heslo a ssid k wifi, nahrál jsem tyto údaje do flash paměti na ESP pomocí knihovny Preferences.h (kod: 2)

>11.11.2024
- při hodinách MTR se věnuji programování panelů pro den otevřených dveří:
  - nakreslil jsem logo školy jako znak v editoru fontů a uložij jej jako znak #174 ve fotnu č.7 (obr: 1)
  - následně jsem si vytvořil "cíl" č. 000 s logem školy a textem SPŠ, SOŠ a SOU, Hradec Králové pomocí mnemo jazyku (obr: 2, obr: 3, hex: 1)
    - mnemo: `b l1 f7 y0 #251 FF f3 m2 y10 s34 'Hradec Králové' f3 m2 y0 s34 'SPŠ, SOŠ a SOU,' f7 y0 s3 #174 CR`

>12.11.2024, 13.11.2024
- hledám možná řešení pro ovládání IBIS převodníku přes systém chytré domácnosti Tuya -- našel jsem řešení v originálním modulu Tuya WBR3, modul jsem objednal za 5$
- navrhuji 3D model pro tento modul, který by byl zobrazen v 3D zobrazení DPS.
  - Pro vytvoření modelu jsem se rozhodl použít program OnShape, jelikož ho aktivně používám pro modelování všech věcí. Následně jsem se rozhodl použít školní CAD program Fusion 360 od Autodesk, ve kterém jsem přiřadil k modelu textury.
  - Následně jsem vyexportoval model ve formátu step, který je podporován programem easyeda
  - bohužel se mi nedaří exportovat step soubor i s texturami a importovaný model je jenom nabarvený
  - zjistil jsem, že modely v step nemají texturu obrázkovou, ale pouze barevnou, tak jsem nabarvil určité části modelu, aby to připomínalo texturu (obr: 4 )