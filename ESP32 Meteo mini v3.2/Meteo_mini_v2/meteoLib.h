//include guards
#ifndef meteoLib_h
#define meteoLib_h

#include <arduino.h>

class MeteoLib {
    public:
        MeteoLib();
        void serialSetup(int baudRate = 115200);
    private:
        int _baudRate = 115200;
};



#endif // meteoLib_h