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
    private:
        int _baudRate = 115200;
};



#endif // meteoLib_h