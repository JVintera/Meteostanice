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
private:
    int _baudRate = 115200;
    const char *_ssid;
    const char *_password;

    Adafruit_BME280 _bme; // Instance of BME280 sensor

public:
    MeteoLib();

    // Pokud se neuloží naměřená hodnota, zůstane defaultně -1
    float bat_voltage = -1;
    long rssi = -1;
    float temperature = -1;
    float pressure = -1;
    float humidity = -1;
    float co2 = -1;

    bool serialSetup(int baudRate = 115200);
    bool connectToWiFi(const char *ssid, const char *password);
    void goToSleep(unsigned long sleepSec);

    // Init and read from sensors
    float readBatVoltage(int ADCpin = 0, float ADCToVolt = 0.0049645); // max z ADC 843 = 4,19V, delic napeti z akumulatoru 20kOhm + 5,1kOhm
    long readRSSI();
    bool initBme280(uint8_t sda = 19, uint8_t scl = 18, uint8_t addr = 0x77);
    float readBme280Temperature();
    float readBme280Pressure();
    float readBme280Humidity();

    // Send data to server
    void sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity, float bat_voltage, long rssi);
    void sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity, float bat_voltage);
    void sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity, long rssi);
    void sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity);
};



#endif // _meteoLib_h_