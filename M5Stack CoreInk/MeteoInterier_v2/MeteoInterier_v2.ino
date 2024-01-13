// x   Stav baterie zobrazit
// x   Při deep sleep se vypíná baterie
// Signalizovat nizky stav baterie blikanim LED
// x   Dodelat wifi
// x   komunikaci s tmep.cz
// Overit mereni
// Stahovat data z TMEP.cz
// Přepínání mezi dalšími měřeními: venku, místnosti
// Nahradit http za https ???

/*
Knihovny k m5stack
https://github.com/m5stack/M5Core-Ink/blob/master/README.md
https://github.com/m5stack/M5Unit-ENV/blob/master/README.md
Návod jak nastavit parametry čidla v TMEP.cz
https://wiki.tmep.cz/doku.php?id=portal:nastaveni_cidla
Příklady
https://blog.laskakit.cz/jednoducha-vizualizace-dat-na-tmep-cz-s-esp32-lpkit-a-cidlem-co2-teploty-vlhkosti-scd41-kompletni-navod/
https://chiptron.cz/articles.php?article_id=318
DEEP SLEEP režim
https://www.itnetwork.cz/hardware-pc/arduino/esp32/mod-hlubokeho-spanku-na-modulu-esp-32
Ukládání dat do FLASH
https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
*/

#include "M5CoreInk.h"
// Knihovny k senzoru ENV III
#include "M5_ENV.h"
// Knihovna pro trvalé ukládání dat do FLASH
#include <Preferences.h>
// Knihovny k Wi-fi
#include <WiFi.h>
#include <HTTPClient.h>

// Prihlasovaci údaje k wifi
#include "wifiConfig.h"
/*
Vytvořte soubor s názvem:  wifiConfig.h
DO něj vložte následující kod:
#define SSID_1 "tvoje_SSID_1";
#define PSWRD_1 = "tvoje_heslo_na_wifi_1";
#define SSID_2 "tvoje_SSID_2";
#define PSWRD_2 = "tvoje_heslo_na_wifi_2";
String serverName = "http://domena_vaseho_cidla.tmep.cz/index.php?";
A nahradte text v uvozovkach svymi prihlasovacimi udaji
*/

#define LED_PIN 10
#define ADC_PIN 35
#define ADC_TO_VOLT 0.0049645  // max z ADC 843 = 4,19V, delic napeti z akumulatoru 20kOhm + 5,1kOhm

// Konstanty pro výpočet doby spánku - deep sleep
#define uS_NA_S 1000000
#define uS_NA_MIN 60000000
#define DELKA_SPANKU 5
RTC_DATA_ATTR int countOfBoot = 0;

// Adresy zařízení na I2C
#define SHT30_ADDRESS 0x44
#define QMP6988_ADDRESS 0x56

// Vytvoření instance | Instance creation
Ink_Sprite InkPageSprite(&M5.M5Ink);
SHT3X sht30;
QMP6988 qmp6988;
Preferences preferences;

void setup() {
  // Initialize CoreInk
  M5.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED svítí
  Serial.begin(115200);
  if (!M5.M5Ink.isInit()) {  // check if the initialization is successful.
    Serial.printf("Ink Init faild");
  }
  M5.M5Ink.clear();  // Clear the screen.
  delay(1000);
  // creat ink Sprite.
  if (InkPageSprite.creatSprite(0, 0, 200, 200, true) != 0) {
    Serial.printf("Ink Sprite creat faild");
  }
  // Test displeje
  //InkPageSprite.drawString(20, 20, "Hello Core-INK");
  //InkPageSprite.pushSprite();
  //delay(2000);
  //M5.M5Ink.clear();  // Clear the screen.

  Wire.begin(25, 26);  // Wire init, adding the I2C bus.
  qmp6988.init(QMP6988_ADDRESS);
  sht30.init(SHT30_ADDRESS);


  // Uložení přihlašovacích údajů k Wi-fi do FLASH paměti
  messageToInk("Vyberte Wi-FI...");        // Scroll wheel up.
  Serial.println("Vyberte Wi-FI, nebo vypněte MCU");
  if (M5.BtnPWR.isPressed())
  {
    if (M5.BtnUP.isPressed())
    {
      messageToInk("Wi-FI 1"); // Scroll wheel up.
      Serial.println("Wi-FI 1");
      preferences.begin("credentials", false); // název datového prostoru, false = read/write mode
      preferences.putString("ssid", SSID_1);
      preferences.putString("password", PSWRD_1);
      preferences.end();
    }
    if (M5.BtnDOWN.isPressed())
    {
      messageToInk("Wi-FI 2"); // Scroll wheel up.
      Serial.println("Wi-FI 2");
      preferences.begin("credentials", false); // název datového prostoru, false = read/write mode
      preferences.putString("ssid", SSID_2);
      preferences.putString("password", PSWRD_2);
      preferences.end();
    }
  }

  // Připojení k Wi-fi
  int pokus = 0;
  preferences.begin("credentials", false); // název datového prostoru, false = read/write mode
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();
  if (ssid == NULL || password == NULL) {
    Serial.print("Není zadané SSID a/nebo heslo.");
    messageToInk("Neni SSID a heslo");
    // Zde se použije DEEP SLEEP od RLC k resetu MCU
    // Pokud po nahrání programu DEEP SLEEP nefunguje, nejprve je dobré restartovat MCU
    // deepSleepRTC();  // Deep sleep pro klasické ESP32
    M5.shutdown(1); // Deep sleep pro M5CoreInk, čas v sekundách
  }
  Serial.print("Načtené SSID z FLASH: ");
  Serial.println(ssid);
  Serial.print("Načtené heslo z FLASH: ");
  Serial.println(password);
  WiFi.begin(ssid, password);
  Serial.println("Pripojovani");
  messageToInk("Pripojovani");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (pokus > 20) // Pokud se behem 10s nepripoji, uspi se na 300s = 5min
    {
      // Pokud po nahrání programu DEEP SLEEP nefunguje, nejprve je dobré restartovat MCU
      // deepSleepRTC();  // Deep sleep pro klasické ESP32
      M5.shutdown(300); // Deep sleep pro M5CoreInk, čas v sekundách
    }
    pokus++;
  }

    Serial.println();
    Serial.print("Pripojeno do site, IP adresa zarizeni: ");
    Serial.println(WiFi.localIP());

    // Merene veliciny
    float tempSHT = 0.1;
    float humSHT = 0.1;
    float tempQMP = 0.1;
    float pressQMP = 0.1;
    float batVoltage = 0.1;

    // Cteni hodnot
    pressQMP = ((qmp6988.calcPressure()) / 100); // Převod na hPa
    tempQMP = qmp6988.calcTemperature();
    if (sht30.get() == 0)
    {
      tempSHT = sht30.cTemp;
      humSHT = sht30.humidity;
    }
    batVoltage = analogRead(ADC_PIN) * ADC_TO_VOLT;
    long rssi = WiFi.RSSI(); // Měření síly signálu wi-fi

    // Odeslání dat na TMEP.cz
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;
      // GUID pro teplotu "teplota", pro vlhkost "vlhkost", ...
      // Více viz https://wiki.tmep.cz/doku.php?id=zarizeni:vlastni_hardware
      String serverPath = serverName + "temperature=" + tempSHT + "&humidity=" + humSHT + "&pressure=" + pressQMP + "&v=" + batVoltage + "&rssi=" + rssi;
      // zacatek http spojeni
      http.begin(serverPath.c_str());

      // http get request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Uvolneni
    http.end();
  } else {
    Serial.println("Wi-Fi odpojeno");
  }

  // Vypis na seriove lince
  Serial.println("\n*** VNITRNI MERENI ***");
  Serial.print("QMP pressure: ");
  Serial.print(pressQMP);
  Serial.println(" hPa");
  Serial.print("QMP temperature: ");
  Serial.print(tempQMP);
  Serial.println(" °C");
  Serial.print("SHT temperature: ");
  Serial.print(tempSHT);
  Serial.println(" °C");
  Serial.print("SHT humidity: ");
  Serial.print(humSHT);
  Serial.println(" %RH");
  Serial.print("Baterie voltage: ");
  Serial.print(batVoltage);
  Serial.println(" V");
  Serial.println("**********************\n");

  //Vypis na displeji
  InkPageSprite.clear();
  InkPageSprite.drawString(5, 5, "VNITRNI MERENI");

  char sPressQMP[10];
  dtostrf(pressQMP, 2, 2, sPressQMP);
  InkPageSprite.drawString(5, 30, "QMP6988 P: ");
  InkPageSprite.drawString(109, 30, sPressQMP);
  InkPageSprite.drawString(170, 30, "hPa");

  char sTempQMP[10];
  dtostrf(tempQMP, 2, 2, sTempQMP);
  InkPageSprite.drawString(5, 50, "QMP6988 T: ");
  InkPageSprite.drawString(118, 50, sTempQMP);
  InkPageSprite.drawString(175, 50, "'C");

  char sTempSHT[10];
  dtostrf(tempSHT, 2, 2, sTempSHT);
  InkPageSprite.drawString(5, 70, "SHT30 T: ");
  InkPageSprite.drawString(118, 70, sTempSHT);
  InkPageSprite.drawString(175, 70, "'C");

  char sHumSHT[10];
  dtostrf(humSHT, 2, 2, sHumSHT);
  InkPageSprite.drawString(5, 90, "SHT30 RH: ");
  InkPageSprite.drawString(118, 90, sHumSHT);
  InkPageSprite.drawString(168, 90, "%RH");

  char sBatVoltage[10];
  dtostrf(batVoltage, 2, 2, sBatVoltage);
  InkPageSprite.drawString(5, 110, "Bat V: ");
  InkPageSprite.drawString(125, 110, sBatVoltage);
  InkPageSprite.drawString(182, 110, "V");

  InkPageSprite.pushSprite();
  M5.update(); // Refresh device button.

  // Pokud po nahrání programu DEEP SLEEP nefunguje, nejprve je dobré restartovat MCU
  // deepSleepRTC();  // Deep sleep pro klasické ESP32
  M5.shutdown(300);  // Deep sleep pro M5CoreInk, čas v sekundách

      /*
      // ***** TLACITKA *****
        if (M5.BtnUP.wasPressed())
          messageToInk("Btn UP Pressed"); // Scroll wheel up.
        if (M5.BtnDOWN.wasPressed())
          messageToInk("Btn DOWN Pressed"); // Trackwheel scroll down.
        if (M5.BtnMID.wasPressed())
          messageToInk("Btn MID Pressed"); // Dial down.
        if (M5.BtnEXT.wasPressed())
          messageToInk("Btn EXT Pressed"); // Top button press.
        if (M5.BtnPWR.wasPressed())
        { // Right button press.
          messageToInk("Btn PWR Pressed");
          M5.shutdown(); // Turn off the power, restart it, you need to wake up through the PWR button.
        }
      */
  }

// Není potřeba, protože při probuzení z deep sleep se restartuje MCU
void loop() {
}

// Vypíše text na displeji po stisknutí tlačítka
void messageToInk(char *str) {
  InkPageSprite.clear();                  // clear the screen.
  InkPageSprite.drawString(35, 59, str);  // draw the string.
  InkPageSprite.pushSprite();             // push the sprite.
  delay(2000);
}

// Funkce pro probuzení MCU z deep sleep pomocí tlačítka
// podle https://www.itnetwork.cz/hardware-pc/arduino/esp32/mod-hlubokeho-spanku-na-modulu-esp-32
// Seznam RTC_GPIO v datasheetu, např. pro ESP32 pico D4
// https://www.espressif.com/sites/default/files/documentation/esp32-pico_series_datasheet_en.pdf
void deepSleepRTC_GPIO() {

}

// Funkce pro uspání MCU a nastavení RTC pro probuzení
// podle https://www.itnetwork.cz/hardware-pc/arduino/esp32/mod-hlubokeho-spanku-na-modulu-esp-32
void deepSleepRTC() {
  ++countOfBoot;  // Globální proměnná
  Serial.println("Boot cycles: " + String(countOfBoot));
  typeAwakenedFromDeepSleep();
  esp_sleep_enable_timer_wakeup(DELKA_SPANKU * uS_NA_MIN);  // Nastavení časovače RTC
  Serial.println("Aktivuji mod hlubokeho spanku...");
  Serial.flush();          // Počká, až se odešlou všechna data po sériové lince, pak program pokračuje dál
  esp_deep_sleep_start();  // Spusteni uspani
  Serial.println("Testovaci zprava, nikdy se nevypisu");
}

// Funkce, která vypíše zdroje probuzení
// podle https://www.itnetwork.cz/hardware-pc/arduino/esp32/mod-hlubokeho-spanku-na-modulu-esp-32
void typeAwakenedFromDeepSleep() {
  esp_sleep_wakeup_cause_t zdrojProbuzeni;

  zdrojProbuzeni = esp_sleep_get_wakeup_cause();

  switch (zdrojProbuzeni) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Cip probuzen externim signalem - RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Cip probuzen externim signalem - RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Cip probuzen casovacem");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Cip probuzen dotykovym senzorem");
      break;
    default:
      Serial.printf("Jiny zdroj probuzeni: %d\n", zdrojProbuzeni);
      break;
  }
}