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
        Serial.println(_ssid);
        Serial.println(_password);

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