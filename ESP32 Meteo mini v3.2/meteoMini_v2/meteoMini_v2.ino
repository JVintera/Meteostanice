#include "wifiConfig.h"
#include "meteoLib.h"

MeteoLib meteoLib;

const char *ssid = SSID_1; //  your network SSID (name)
const char *password = PSWRD_1; // your network password


#define SLEEP_SEC 10 * 60   // Measurement interval (N x 60 seconds)

#define SDA 19
#define SCL 18
#define BME280address 0x77 // (0x77) cut left and solder right pad on board

// Přednastavená hodnota 10 je kvůli chybě se čtením dat z BME280
// float temperature = 10;
// float pressure = 10;
// float humidity = 10;
// float bat_voltage = 10;


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

  meteoLib.readBatVoltage(0, 0.001763668); // Přečte napětí na baterii a uloží do proměnné bat_voltage
  meteoLib.readRSSI(); // Přečte sílu wifi signálu a uloží do proměnné rssi

  meteoLib.initBme280(SDA, SCL, BME280address);
  meteoLib.readBme280Temperature();
  meteoLib.readBme280Pressure();
  meteoLib.readBme280Humidity();

  meteoLib.connectToWiFi(ssid, password); // Připojí se k WiFi s danými údaji

  // meteoLib.sendTemperaturePressureHumidity(serverName, meteoLib.temperature, meteoLib.pressure, meteoLib.humidity); // Odešle data na TMEP.cz
  meteoLib.sendTemperaturePressureHumidity(serverName, meteoLib.temperature, meteoLib.pressure, meteoLib.humidity, meteoLib.bat_voltage, meteoLib.rssi); // Odešle data na TMEP.cz

  meteoLib.goToSleep(SLEEP_SEC);
}

void loop()  //Není potřeba
{
}
