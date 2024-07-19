#include <arduino.h>

#include "meteoLib.h"

#define serialDebug  // Uncomment for serial debug
#define serialDebugSecure  // Uncomment for serial debug with secure data

MeteoLib::MeteoLib()
{
    // empty constructor
}

bool MeteoLib::serialSetup(int baudRate)
{
    _baudRate = baudRate;
    unsigned int pokus = 0;
    Serial.begin(_baudRate);
    while (!Serial && pokus < 500) // wait for serial port to connect. Needed for native USB port only
    {
        delay(10);
        #ifdef serialDebug
            Serial.print(".");
        #endif
        pokus++;
    }
    if (Serial)
    {
        #ifdef serialDebug
            Serial.println("Sériová komunikace je připravena.");
        #endif
        return true;
    }
    else
    {
        #ifdef serialDebug
            Serial.println("Sériová komunikace nejspíš nebude fungovat. Zkontrolujte zapojení a nastavení sériové linky. Pokud používáte USB, zkontrolujte, zda je správně nainstalován ovladač.");
        #endif
        return false;
    }
}

bool MeteoLib::connectToWiFi(const char *ssid, const char *password)
{
    unsigned int pokus = 0;
    _ssid = ssid;
    _password = password;
    #ifdef serialDebugSecure
        Serial.println(_ssid);
        Serial.println(_password);
    #endif

    if (WiFi.status() != WL_NO_SHIELD)
    {
        WiFi.begin(_ssid, _password);
        #ifdef serialDebug
            Serial.println("Připojování k Wifi");
        #endif
        while (WiFi.status() != WL_CONNECTED && pokus < 20)
        {
            delay(500);
            #ifdef serialDebug
                Serial.print(".");
            #endif
            pokus++;
        }
        if (WiFi.status() == WL_CONNECTED)
        {
            #ifdef serialDebug
                Serial.println("Pripojeno k WiFi");
            #endif
            return true;
        }
        else
        {
            #ifdef serialDebug
                Serial.println("Nepodarilo se pripojit k WiFi");
            #endif
            return false;
        }
    }
    else
    {
        #ifdef serialDebug
            Serial.println("WiFi modul není připojený, nebo nefunguje. Zkontrolujte zapojení a napájení modulu.");
        #endif
        return false;
    }
}

void MeteoLib::goToSleep(unsigned long sleepSec)
{
    //delay(1); // Zajištění, že všechny příkazy Serial byly odeslány
    #ifdef serialDebug
        Serial.println("ESP in sleep mode");
    #endif
    esp_sleep_enable_timer_wakeup(sleepSec * 1000000); // Nastavení časovače pro probuzení
    esp_deep_sleep_start();                            // Spuštění hlubokého spánku
}

// Init and read from sensors
float MeteoLib::readBatVoltage(int ADCpin, float ADCToVolt)
{
    bat_voltage = analogRead(ADCpin) * ADCToVolt;
    #ifdef serialDebug
        Serial.print("Voltage = ");
        Serial.print(bat_voltage);
        Serial.println(" V");
    #endif
    return bat_voltage; // Vrací hodnotu napětí na baterii
}

long MeteoLib::readRSSI()
{
    rssi = WiFi.RSSI();
    #ifdef serialDebug
        Serial.print("Received Signal Strength Indication = ");
        Serial.print(rssi);
        Serial.println(" dBm");
    #endif
    return rssi; // Vrací hodnotu napětí na baterii
}

// BME280
bool MeteoLib::initBme280(uint8_t sda, uint8_t scl, uint8_t addr)
{
    Wire.begin(sda, scl);
    if (!_bme.begin(addr, &Wire))
    {
        #ifdef serialDebug
            Serial.println("Could not find a valid BME280 sensor, check wiring!");
        #endif
        return false;
    }
    else
    {
        #ifdef serialDebug
            Serial.println("BME280 sensor is connected.");
        #endif
        // BME280 settings for weather monitoring
        _bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_OFF);
        return true;
    }
}

float MeteoLib::readBme280Temperature()
{
    temperature = _bme.readTemperature();
    #ifdef serialDebug
        Serial.print("Temperature = ");
        Serial.print(temperature);
        Serial.println(" *C");
    #endif
    return temperature;
}

float MeteoLib::readBme280Pressure()
{
    pressure = _bme.readPressure();
    #ifdef serialDebug
        Serial.print("Pressure = ");
        Serial.print(pressure);
        Serial.println(" Pa");
    #endif
    return pressure;
}

float MeteoLib::readBme280Humidity()
{
    humidity = _bme.readHumidity();
    #ifdef serialDebug
        Serial.print("Humidity = ");
        Serial.print(humidity);
        Serial.println(" %");
    #endif
    return humidity;
}


// Send data to server
void MeteoLib::sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity, float bat_voltage, long rssi)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        // GUID pro teplotu "teplota", pro vlhkost "vlhkost", ...
        // Více viz https://wiki.tmep.cz/doku.php?id=zarizeni:vlastni_hardware
        String serverPath = serverName + "temV=" + temperature + "&humV=" + humidity + "&pressV=" + pressure + "&v=" + bat_voltage + "&rssi=" + rssi;
        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            String payload = http.getString();
            #ifdef serialDebug
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                Serial.println(payload);
            #endif
        }
        else
        {
            #ifdef serialDebug
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            #endif
        }
        http.end();
    }
    else
    {
        Serial.println("Wi-Fi odpojeno");
    }
}

void MeteoLib::sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity, float bat_voltage)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        // GUID pro teplotu "teplota", pro vlhkost "vlhkost", ...
        // Více viz https://wiki.tmep.cz/doku.php?id=zarizeni:vlastni_hardware
        String serverPath = serverName + "temV=" + temperature + "&humV=" + humidity + "&pressV=" + pressure + "&v=" + bat_voltage;
        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            String payload = http.getString();
            #ifdef serialDebug
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                Serial.println(payload);
            #endif
        }
        else
        {
            #ifdef serialDebug
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            #endif
        }
        http.end();
    }
    else
    {
        Serial.println("Wi-Fi odpojeno");
    }
}

void MeteoLib::sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity, long rssi)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        // GUID pro teplotu "teplota", pro vlhkost "vlhkost", ...
        // Více viz https://wiki.tmep.cz/doku.php?id=zarizeni:vlastni_hardware
        String serverPath = serverName + "temV=" + temperature + "&humV=" + humidity + "&pressV=" + pressure + "&rssi=" + rssi;
        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            String payload = http.getString();
#ifdef serialDebug
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            Serial.println(payload);
#endif
        }
        else
        {
#ifdef serialDebug
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
#endif
        }
        http.end();
    }
    else
    {
        Serial.println("Wi-Fi odpojeno");
    }
}

void MeteoLib::sendTemperaturePressureHumidity(String serverName, float temperature, float pressure, float humidity)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        // GUID pro teplotu "teplota", pro vlhkost "vlhkost", ...
        // Více viz https://wiki.tmep.cz/doku.php?id=zarizeni:vlastni_hardware
        String serverPath = serverName + "temV=" + temperature + "&humV=" + humidity + "&pressV=" + pressure;
        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            String payload = http.getString();
            #ifdef serialDebug
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                Serial.println(payload);
            #endif
        }
        else
        {
            #ifdef serialDebug
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            #endif
        }
        http.end();
    }
    else
    {
        Serial.println("Wi-Fi odpojeno");
    }
}