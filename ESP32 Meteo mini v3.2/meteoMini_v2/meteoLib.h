//include guards
#ifndef meteoLib_h
#define meteoLib_h

#include <arduino.h>
#include "SPI.h"
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>      //HttpClient by Adrian McEwen..., v2.2.0
#include <Adafruit_BME280.h> //Adafruit BME280 Library by Adafruit, v2.2.4

class MeteoLib {
    public:
        MeteoLib();
        void serialSetup(int baudRate = 115200);
        void connectToWiFi(const char *ssid, const char *password);

    private:
        int _baudRate = 115200;
        const char *_ssid;
        const char *_password;
};



#endif // meteoLib_h