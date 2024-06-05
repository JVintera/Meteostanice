/*
--------------------------------------------------
ESP32-S3-DEVKit, ver1.0
--------------------------------------------------
https://www.laskakit.cz/laskakit-esp32-s3-devkit/
https://github.com/LaskaKit/ESP32-S3-DEVKit
Programování přes USB-C označené UART
V Arduino IDE nastavit ESP32S3 Dev Module

GPIO9 - měření baterie
GPIO47 - spínání napětí pro uŠup SPI a uŠup I2C
GPIO45 - RGB LED SK6812-EC20

uŠup I2C - SDA GPIO42, SCL GPIO2
uŠup SPI - MISO GPIO13, MOSI GPIO11, SCK GPIO12, CS GPIO10

--------------------------------------------------
LaskaKit microSD Card modul (LaskaKit microSD Reader)
--------------------------------------------------
https://www.laskakit.cz/laskakit-microsd-card-modul/
https://github.com/LaskaKit/microSD-Reader/blob/main/README_CZ.md



*/

#include <config.h>
#include <meteoLib.h>

// LaskaKit microSD Card modul
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define SD_CS_PIN 10
#define SD_MOSI_PIN 11
#define SD_CLK_PIN 12
#define SD_MISO_PIN 13
/*
#define VSPI_SS 10
#define VSPI_MOSI 11
#define VSPI_SCLK 12
#define VSPI_MISO 13

#define HSPI_SS 10
#define HSPI_MOSI 11
#define HSPI_SCLK 12
#define HSPI_MISO 13
*/

// Definujeme SPI2 sběrnici
//SPIClass SPI2(FSPI);
//SPI2 = new SPIClass(VSPI);
//SPI2 = new SPIClass(HSPI);
SPIClass *hspi = NULL;

// json soubor
#include <ArduinoJson.h>


void setup() {
  Serial.begin(115200);
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");

  vspi = new SPIClass(FSPI);
  hspi = new SPIClass(HSPI);

  //SPI2.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);  //SCLK, MISO, MOSI, SS
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);  //SCLK, MISO, MOSI, SS

  pinMode(vspi->pinSS(), OUTPUT);  //VSPI SS
  pinMode(hspi->pinSS(), OUTPUT);
  //pinMode(SD_CS_PIN, OUTPUT); // SS

  if (!SD.begin(VSPI_SS, FSPI))
//  if (!SD.begin(SD_CS_PIN))
  {
    Serial.println("vspi  Inicializace SD karty se nezdařila");
    //return;
  }
  else Serial.println("vspi  OK");
/*  if (!SD.begin(HSPI_SS, HSPI))
  {
    Serial.println("hspi  Inicializace SD karty se nezdařila");
    //return;
  }
  else Serial.println("hspi  OK");
*/



  File file = SD.open("/wifi_credentials.json");
  if (!file)
  {
    Serial.println("Nepodařilo se otevřít soubor");
    return;
  }

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.println("Chyba při deserializaci JSON");
    return;
  }

  const char *key_SSID = doc["SSID"];
  Serial.println(key_SSID);
  const char *key_PSWRD = doc["PSWRD"];
  Serial.println(key_PSWRD);

  file.close();
}

void loop() {
//  static int i = 0;
//  Serial.println(i++);
//  delay(1000);

}
