

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
    - mnemo: `b l1 f7 y0 #251 FF f3 m2 y10 s34 'Hradec Králové' f3 m2 y0 s34 'SPŠ, SOŠ a SOU,' f7 y0 s3 #174 CR` (pozn: Musel jsem dát nejdřív hradec králové, jinak tento text nebyl vidět)

>12.11.2024, 13.11.2024
- hledám možná řešení pro ovládání IBIS převodníku přes systém chytré domácnosti Tuya -- našel jsem řešení v originálním modulu Tuya WBR3, modul jsem objednal za 5$
- navrhuji 3D model pro tento modul, který by byl zobrazen v 3D zobrazení DPS.
  - Pro vytvoření modelu jsem se rozhodl použít program OnShape, jelikož ho aktivně používám pro modelování všech věcí. Následně jsem se rozhodl použít školní CAD program Fusion 360 od Autodesk, ve kterém jsem přiřadil k modelu textury.
  - Následně jsem vyexportoval model ve formátu step, který je podporován programem easyeda
  - bohužel se mi nedaří exportovat step soubor i s texturami a importovaný model je jenom nabarvený
  - zjistil jsem, že modely v step nemají texturu obrázkovou, ale pouze barevnou, tak jsem nabarvil určité části modelu, aby to připomínalo texturu (obr: 4 )

>13.11.2024
- pracuji na v3 PCB, která bude obsahovat možnost osadit Tuya WBR3
- připravuji data pro nahrání na panel, který bude na dnu otevřených dveří
  - místo panelu č.1 (přední) bude nakonec panel č.2 (boční), který je o trochu kratší, takže jsem musel upravit zobrazovaná data
  - mnemo: `f7 y0 s3 #174 f3 y10 s29 'Hradec Králové' f2 y0 s29 'SPŠ, SOŠ a SOU,' CR`
  - (obr: 5; hex: 2)
- na PCB jsem ještě přemýšlel o možnosti relé, které by spínalo podsvit panelu
  - nejdřív jsem chtěl použít 5VDC relé a kreslit pro to schéma s možností ovládání přes optron pomocí 3v3
  - nakonec jsem se rozhodl použít 3VDC relé a dát jenom předřadný rezistor pro dostatečný úbytek napětí

>14.11.2024
- přinesl jsem boční panel jiný, než se kterým jsem původně počítal.
  - Přinesený panel má oproti původně počítanému panelu na výšku namísto 19 px pouhých 16 px
  - Musel jsem zasáhnout přímo do nastavení fontu a jednotlivých znaků, jelikož znaky byly větší než polovina panelu
- zjistil jsem, že i přesto, že nastavím data na cíl 0, tak stejně se musí přes IBIS kód cíle zavolat
  - namontoval jsem do panelu převodník s arduino nano (obr: 7), na který jsem nahrál jednoduchý kód, který volá každou vteřinu linku 000 a cíl 000
  - (kod: 4; obr: 8)

>15.11.2024
- panel jsme namontovali s pomocí spolužáků a za dozoru učitele ke vchodu do školy a uvedl jsem ho do provozu
- později jsem si všiml grafické chyby, kde znak l byl malý a nezasahoval do horní poloviny velkého znaku a vypadal spíš jako i. (obr: 9)
  - připravil jsem si opravená data a přišel opravená data do panelu nahrát; oddělal jsem zadní kryt, připojil převodník k počítači a nahrál data