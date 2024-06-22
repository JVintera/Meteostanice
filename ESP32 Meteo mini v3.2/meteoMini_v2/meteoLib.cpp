#include <arduino.h>
#include "meteoLib.h"

MeteoLib::MeteoLib()
{
    // empty constructor
}

void MeteoLib::serialSetup(int baudRate)
{
    _baudRate = baudRate;
    Serial.begin(_baudRate);
    while (!Serial); // wait for serial port to connect. Needed for native USB port only
    Serial.println("Sériová komunikace je připravena");
}

void MeteoLib::connectToWiFi(const char *ssid, const char *password)
{
    int pokus = 0;
    _ssid = ssid;
    _password = password;
    if (WiFi.status() != WL_NO_SHIELD)
    {
        WiFi.begin(_ssid, _password);
        //Serial.println(_ssid);
        //Serial.println(_password);

        Serial.println("Pripojovani k Wifi");
        while (WiFi.status() != WL_CONNECTED && pokus < 20)
        {
            delay(500);
            Serial.print(".");
            pokus++;
        }
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("Pripojeno k WiFi");
        }
        else
        {
            Serial.println("Nepodarilo se pripojit k WiFi");
        }
    }
    else
    {
        Serial.println("WiFi modul není připojený, nebo nefunguje. Zkontrolujte zapojení a napájení modulu.");
    }
}

void MeteoLib::goToSleep(unsigned long sleepSec)
{
    //delay(1); // Zajištění, že všechny příkazy Serial byly odeslány
    Serial.println("ESP in sleep mode");
    esp_sleep_enable_timer_wakeup(sleepSec * 1000000); // Nastavení časovače pro probuzení
    esp_deep_sleep_start();                            // Spuštění hlubokého spánku
}

void MeteoLib::initBME280(uint8_t sda, uint8_t scl, uint8_t addr)
{
    Wire.begin(sda, scl);
    if (!bme.begin(addr, &Wire))
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
    else
    {
        Serial.println("BME280 sensor is connected.");
        // BME280 settings for weather monitoring
        bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_OFF);
    }
}

void MeteoLib::readSensors(float &temperature, float &pressure, float &humidity)
{
    temperature = bme.readTemperature();
    pressure = bme.readPressure();
    humidity = bme.readHumidity();

    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" Pa");
    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
}

void MeteoLib::sendDataToTMEP(float &temperature, float &pressure, float &humidity, String serverName)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String serverPath = serverName + "temV=" + temperature + "&humV=" + humidity + "&pressV=" + pressure;
        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    else
    {
        Serial.println("Wi-Fi odpojeno");
    }
}