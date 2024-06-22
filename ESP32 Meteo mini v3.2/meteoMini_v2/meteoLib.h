//include guards
#ifndef meteoLib_h
#define meteoLib_h

#include <arduino.h>
#include "SPI.h"
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>      //HttpClient by Adrian McEwen..., v2.2.0
#include <Adafruit_Sensor.h> //Adafruit Unified Sensor by Adafruit, v1.1.4
#include <Adafruit_BME280.h> //Adafruit BME280 Library by Adafruit, v2.2.4

class MeteoLib {
    public:
        MeteoLib();
        void serialSetup(int baudRate = 115200);
        void connectToWiFi(const char *ssid, const char *password);
        void goToSleep(unsigned long sleepSec);
        void initBME280(uint8_t sda = 19, uint8_t scl = 18, uint8_t addr = 0x77);
        void readSensors(float &temperature, float &pressure, float &humidity);
        void sendDataToTMEP(float &temperature, float &pressure, float &humidity, String serverName);

    private:
        int _baudRate = 115200;
        const char *_ssid;
        const char *_password;
        Adafruit_BME280 bme; // Instance of BME280 sensor
};



#endif // meteoLib_h