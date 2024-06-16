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
