/*
--------------------------------------------------
DESKA
METEO mini v3.2
--------------------------------------------------
https://www.laskakit.cz/laskakit-meteo-mini/
https://github.com/LaskaKit/Meteo_Mini
Programování přes programátor (převodník USB na UART)
https://www.laskakit.cz/laskakit-ch9102-programmer-usb-c--microusb--uart/
https://www.laskakit.cz/prevodnik-6pin-microusb-ttl-uart--cp2102--dtr-pin/
dříve https://www.laskakit.cz/laskakit-ch340-programmer-usb-c--microusb--uart/
V Arduino IDE nastavit **ESP32C3 Dev Module**

GPIO10 - DS18B20
GPIO19 - SDA
GPIO18 - SCL
uŠup I2C - SDA GPIO19, SCL GPI18

--------------------------------------------------
DESKA
ESP32-S3-DEVKit, ver1.0
--------------------------------------------------
https://www.laskakit.cz/laskakit-esp32-s3-devkit/
https://github.com/LaskaKit/ESP32-S3-DEVKit
Programování přes USB-C označené UART
V Arduino IDE nastavit **ESP32S3 Dev Module**

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

--------------------------------------------------
LaskaKit BMP280 Senzor tlaku a teploty vzduchu
--------------------------------------------------
https://www.laskakit.cz/laskakit-bmp280-senzor-tlaku-a-teploty-vzduchu/



*/

#include <wifiConfig.h>
#include <meteoLib.h>

MeteoLib meteoLib;


char ssid[] = SSID_1; //  your network SSID (name)
char pass[] = PSWRD_1;  // your network password

//int status = WL_IDLE_STATUS; // the Wifi radio's status



// LaskaKit microSD Card modul
/*
// Problém je s tím, že nerozpozná kartu -> ověřit, jestli vůbec funguje SPI2
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS_PIN 10
#define SD_MOSI_PIN 11
#define SD_CLK_PIN 12
#define SD_MISO_PIN 13

#define HSPI_SCLK 12
#define HSPI_MISO 13
#define HSPI_MOSI 11
#define HSPI_SS 10

SPIClass SPI2(HSPI);
*/

void setup()
{
  meteoLib.serialSetup(); // Nastaví sériovou linku defaultně s rychlostí 115200 baud, a počká, dokus není komunikace funkční
}

void loop()  //Není potřeba
{

}
