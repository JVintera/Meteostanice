//include guards
#ifndef _meteoLib_h_
#define _meteoLib_h_

#include <arduino.h>

#include <SPI.h>
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
        void sendBatVoltage(int ADCpin = 0, float ADCToVolt = 0.0049645, String serverName = "0"); // max z ADC 843 = 4,19V, delic napeti z akumulatoru 20kOhm + 5,1kOhm
        void initBME280(uint8_t sda = 19, uint8_t scl = 18, uint8_t addr = 0x77);
        void readBME280(float &temperature, float &pressure, float &humidity);
        void sendBME280(float &temperature, float &pressure, float &humidity, String serverName);

    private:
        int _baudRate = 115200;
        const char *_ssid;
        const char *_password;
        Adafruit_BME280 bme; // Instance of BME280 sensor
};



#endif // _meteoLib_h_