
int lz = true;
int zapnuto = true;
String HEXproSUM = "";
void setup() {
  // Nastavení sériové komunikace
  Serial.begin(1200, SERIAL_7E2);  // 1200 baud, 7 datových bitů, parita sudá, 2 stop bity
  randomSeed(analogRead(0));
}

void loop() {
  // Délka řetězce
  int length = 5;

  // Vytvoříme prázdný řetězec pro náhodné znaky
  char randomString[length + 1];  // +1 pro ukončovací znak '\0'

  // Generování náhodných znaků
  for (int i = 0; i < length; i++) {
    randomString[i] = char(random(32, 127));  // ASCII znaky od 32 do 126
  }

  // Ukončovací znak pro řetězec
  randomString[length] = '\0';
if(zapnuto){
  if (lz) {
    HEXproSUM = String("7f") + stringToHex("l001") + "0D";
    lz = false;
  } else {
    HEXproSUM = String("7f") + stringToHex("z001") + "0D";
    lz = true;
    zapnuto = false;
  }
}
else{
  if (lz) {
    HEXproSUM = String("7f") + stringToHex("l000") + "0D";
    lz = false;
  } else {
    HEXproSUM = String("7f") + stringToHex("z000") + "0D";
    lz = true;
  }
}

  String CheckSUM = checksum8XOR(HEXproSUM);
  String hotovyHEX = HEXproSUM.substring(2) + CheckSUM;

  // Převedeme String na C-string
  char hexString[hotovyHEX.length() + 1];
  hotovyHEX.toCharArray(hexString, sizeof(hexString));

  sendHexString(hexString);
  delay(1000);
}

void sendHexString(const char *hexString) {
  // Délka hexadecimálního řetězce
  size_t len = strlen(hexString);

  // Projdeme hex řetězec a každý pár znaků převedeme na byte
  for (size_t i = 0; i < len; i += 2) {
    char hexByte[3];
    hexByte[0] = hexString[i];
    hexByte[1] = hexString[i + 1];
    hexByte[2] = '\0';

    // Převod hexadecimálního bajtu na integer
    byte b = (byte)strtol(hexByte, NULL, 16);

    // Odeslání bajtu přes sériovou linku
    Serial.write(b);
  }
}

// Funkce pro převod řetězce na hexadecimální řetězec
String stringToHex(String str) {
  String hexKodSlova = "";
  for (int i = 0; i < str.length(); i++) {
    char hex[3];
    sprintf(hex, "%02X", str.charAt(i));
    hexKodSlova += hex;
  }
  return hexKodSlova;
}

// Funkce pro výpočet checksumu pomocí XOR
String checksum8XOR(String hexString) {
  // Převod hex řetězce na pole bajtů
  int length = hexString.length() / 2;
  byte bytes[length];
  for (int i = 0; i < length; i++) {
    bytes[i] = strtol(hexString.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }

  // Inicializace checksumu
  byte checksum = 0x00;

  // XOR pro všechny bajty
  for (int i = 0; i < length; i++) {
    checksum ^= bytes[i];
  }

  // Převod výsledného checksumu na hex řetězec
  char hexChecksum[3];
  sprintf(hexChecksum, "%02X", checksum);
  return String(hexChecksum);
}
