--------------------------------------------------
## DESKA
# METEO mini v3.2
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
## DESKA
# ESP32-S3-DEVKit, ver1.0
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
# Adafruit BME280
--------------------------------------------------
https://github.com/adafruit/Adafruit_BME280_Library/tree/master
https://learn.adafruit.com/adafruit-bme280-humidity-barometric-pressure-temperature-sensor-breakout/downloads
Adresa pro I2C: 0x77 nebo 0x76
Umí též SPI

--------------------------------------------------
# LaskaKit BMP280 Senzor tlaku a teploty vzduchu
--------------------------------------------------
https://www.laskakit.cz/laskakit-bmp280-senzor-tlaku-a-teploty-vzduchu/

--------------------------------------------------
# LaskaKit microSD Card modul (LaskaKit microSD Reader)
--------------------------------------------------
https://www.laskakit.cz/laskakit-microsd-card-modul/
https://github.com/LaskaKit/microSD-Reader/blob/main/README_CZ.md
