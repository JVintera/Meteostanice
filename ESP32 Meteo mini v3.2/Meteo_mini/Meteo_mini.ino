//V Arduino IDE: ESP32C3 Dev Module

/*
Brainstorming
- měření vlhkosti a teploty v interiéru s lokálním úložištěm kvůli plísním, 5 místností, jedna jednotka 
  s displejem, záznam dat do grafů?
- https://grafana.com/
- další senzory
- zobrazení hodnot na webových stránkách
*/ 





//**********************************************
/*
1) V dokumentaci wifi.h zjistit, zda jde vypnout aktualizace firmwaru, případně jak.
2) Jak funguje sleep, resp. probuzení z něj.
    Rozdělit co se má provést v setupu a co v loopu
    Konfiguraci wifi provést jen jednou, víckrát by se k tomu nemělo
*/
// Wifi
// https://www.arduino.cc/reference/en/libraries/wifi/
// https://github.com/arduino-libraries/WiFi/tree/master/docs

// Meteo Mini Meteostanice 2.0
// https://www.laskakit.cz/laskakit-meteo-mini-meteostanice/?variantId=11912 - e-shop LaskaKit
// https://www.laskakit.cz/user/related_files/l__skakit_meteo_mini_meteostanice_2-0_-_n__vod.pdf - pdf návod
// https://youtu.be/Ipeo-O5kT-w?feature=shared - video návod
// https://github.com/LaskaKit/Weather_Station_Mini/tree/main/SW/tmep/weather_mini - github, konkrétně verze
//    programu pro tmep.cz

// Deska LaskaKit Meteo Mini
// https://www.laskakit.cz/laskakit-meteo-mini/?variantId=10473 - e-shop LaskaKit
// https://github.com/LaskaKit/Meteo_Mini/blob/main/HW/MeteoMini_v3_2.pdf - github, konkrétně schéma zapojení
//**********************************************
/* LaskaKit DIY Mini Weather Station.
 * TMEP edition
 * Read Temperature, Humidity and pressure and send to TMEP.cz
 *
 * For settings see config.h
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 * Board: ESP32-C3 Dev Module
 *
 */

// připojení knihoven
#include "config.h" // change to config.h and fill the file.

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> // BME280 by Adafruit
#include <ESP32AnalogRead.h> // ESP32AnalogRead by madhephaestus https://github.com/madhephaestus/ESP32AnalogRead
#include <WiFiManager.h>     // WiFi manager by tzapu https://github.com/tzapu/WiFiManager

#define BME280_ADDRESS 0x77       // (0x77) cut left and solder right pad on board
#define SLEEP_SEC 15 * 60         // Measurement interval (seconds)
#define ADC_PIN 0                 // ADC pin on LaskaKit Meteo mini
#define deviderRatio 1.7693877551 // Voltage devider ratio on ADC pin 1M + 1.3MOhm

// Vytvoření instance | Instance creation
Adafruit_BME280 bme;
ESP32AnalogRead adc;

float temperature;
float pressure;
float humidity;
float bat_voltage;

void postData()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // GUID, nasleduje hodnota teploty, pro vlhkost "humV", pro CO2 "CO2", pro napeti baterie "v"
    String serverPath = serverName + "" + GUID + "=" + temperature + "&humV=" + humidity + "&pressV=" + pressure + "&v=" + bat_voltage;

    // zacatek http spojeni
    http.begin(serverPath.c_str());

    // http get request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP odpoved: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error kod: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else
    Serial.println("Wi-Fi odpojeno");
}

void GoToSleep()
{
  delay(1);
  // ESP Deep Sleep
  Serial.println("ESP in sleep mode");
  esp_sleep_enable_timer_wakeup(SLEEP_SEC * 1000000);
  esp_deep_sleep_start();
}

// pripojeni k WiFi | WiFi Connection
void WiFiConnection()
{
  // Probudit WiFi | Wake up WiFi Modem
  WiFi.mode(WIFI_STA);

  // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  wm.setConfigPortalTimeout(180); // set portal time to  3 min, then sleep.
  bool res;
  res = wm.autoConnect("LaskaKit AutoConnectAP", "laskakit"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("Wi-Fi connected successfully");
  }
}

// Přečíst data z BME280 | Read data from BME280
void readBME()
{
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("% RH");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println("hPa");
}

// Měření napětí baterie | Battery voltage measurement
void readBat()
{
  bat_voltage = adc.readVoltage() * deviderRatio;
  Serial.print("Battery voltage " + String(bat_voltage) + "V");
}

void setup()
{
  // Hned vypneme WiFi | disable WiFi, coming from DeepSleep, as we do not need it right away
  WiFi.mode(WIFI_OFF);
  delay(1);

  adc.attach(ADC_PIN);

  Serial.begin(115200);
  while (!Serial)
  {
  } // Wait

  // initilizace BME280 | BME280 Initialization
  Wire.begin(19, 18); // SDA SCL
  if (!bme.begin(BME280_ADDRESS))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1)
      ;
  }
  Serial.println("-- Weather Station Scenario --");
  Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
  Serial.println("filter off");
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF);
  delay(10);

  readBME();
  readBat();

  // Pripojeni k WiFi | Connect to WiFi
  WiFiConnection();
  postData();

  WiFi.disconnect(true);
  GoToSleep();
}

void loop()
{
  // Nepotřebujeme loop | We dont use the loop
}



//**********************************************
// Funkční program pro Meteo Mini - původní
// Správná adresa BME280 je 0x77, viz DPS senzoru

// Meteo Mini Meteostanice 2.0
// https://www.laskakit.cz/laskakit-meteo-mini-meteostanice/?variantId=11912 - e-shop LaskaKit
// https://www.laskakit.cz/user/related_files/l__skakit_meteo_mini_meteostanice_2-0_-_n__vod.pdf - pdf návod
// https://youtu.be/Ipeo-O5kT-w?feature=shared - video návod
// https://github.com/LaskaKit/Weather_Station_Mini/tree/main/SW/tmep/weather_mini - github, konkrétně verze 
//    programu pro tmep.cz

// Deska LaskaKit Meteo Mini
// https://www.laskakit.cz/laskakit-meteo-mini/?variantId=10473 - e-shop LaskaKit
// https://github.com/LaskaKit/Meteo_Mini/blob/main/HW/MeteoMini_v3_2.pdf - github, konkrétně schéma zapojení
//**********************************************
/* LaskaKit DIY Mini Weather Station.
 * TMEP edition
 * Read Temperature, Humidity and pressure and send to TMEP.cz
 *
 * For settings see config.h
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 * Board: ESP32-C3 Dev Module
 *
 */
/*
// připojení knihoven
#include "config.h"            // change to config.h and fill the file.

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>      // BME280 by Adafruit
#include <ESP32AnalogRead.h>      // ESP32AnalogRead by madhephaestus https://github.com/madhephaestus/ESP32AnalogRead
#include <WiFiManager.h>          // WiFi manager by tzapu https://github.com/tzapu/WiFiManager

#define BME280_ADDRESS 0x77     // (0x77) cut left and solder right pad on board
#define SLEEP_SEC 15*60           // Measurement interval (seconds)
#define ADC_PIN 0                 // ADC pin on LaskaKit Meteo mini
#define deviderRatio 1.7693877551 // Voltage devider ratio on ADC pin 1M + 1.3MOhm

// Vytvoření instance | Instance creation
Adafruit_BME280 bme;
ESP32AnalogRead adc;

float temperature;
float pressure;
float humidity;
float bat_voltage;

void postData(){

  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;

    //GUID, nasleduje hodnota teploty, pro vlhkost "humV", pro CO2 "CO2", pro napeti baterie "v"
    String serverPath = serverName + "" + GUID + "=" + temperature + "&humV=" + humidity + "&pressV=" + pressure + "&v=" + bat_voltage;

    // zacatek http spojeni
    http.begin(serverPath.c_str());

    // http get request
    int httpResponseCode = http.GET();

    if (httpResponseCode>0) {
      Serial.print("HTTP odpoved: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error kod: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  } else
      Serial.println("Wi-Fi odpojeno");

}

void GoToSleep(){
    delay(1);
  // ESP Deep Sleep
  Serial.println("ESP in sleep mode");
  esp_sleep_enable_timer_wakeup(SLEEP_SEC * 1000000);
  esp_deep_sleep_start();
}

// pripojeni k WiFi | WiFi Connection
void WiFiConnection(){
    // Probudit WiFi | Wake up WiFi Modem
  WiFi.mode( WIFI_STA);

 //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    //wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    wm.setConfigPortalTimeout(180);   // set portal time to  3 min, then sleep.
    bool res;
    res = wm.autoConnect("LaskaKit AutoConnectAP","laskakit"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    }
    else {
        //if you get here you have connected to the WiFi
        Serial.println("Wi-Fi connected successfully");
    }
}

// Přečíst data z BME280 | Read data from BME280
void readBME(){
  temperature = bme.readTemperature();
  humidity    = bme.readHumidity();
  pressure    = bme.readPressure() / 100.0F;

  Serial.print("Temp: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("% RH");
  Serial.print("Pressure: "); Serial.print(pressure); Serial.println("hPa");
}


// Měření napětí baterie | Battery voltage measurement
void readBat(){
  bat_voltage = adc.readVoltage()*deviderRatio;
  Serial.print("Battery voltage " + String(bat_voltage) + "V");

}

void setup() {
  // Hned vypneme WiFi | disable WiFi, coming from DeepSleep, as we do not need it right away
  WiFi.mode( WIFI_OFF );
  delay( 1 );

  adc.attach(ADC_PIN);

  Serial.begin(115200);
  while(!Serial) {} // Wait

  // initilizace BME280 | BME280 Initialization
  Wire.begin(19,18); // SDA SCL
  if (! bme.begin(BME280_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("-- Weather Station Scenario --");
  Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
  Serial.println("filter off");
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
  delay(10);

  readBME();
  readBat();

  // Pripojeni k WiFi | Connect to WiFi
  WiFiConnection();
  postData();

  WiFi.disconnect(true);
  GoToSleep();
}

void loop(){
  // Nepotřebujeme loop | We dont use the loop
}
*/

//**********************************************
// Ručně upraven kod
// Přidání knihovny #include <SPI.h>
// Smazání zpoždění za Serial.begin(115200); - zbytečné
// 
//**********************************************
/*
#include <SPI.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//const char *ssid = "Zima24_guest";     // nastaví jméno Wi-Fi sítě
//const char *password = "Zimikovi812/"; // nastaví heslo Wi-Fi sítě
const char *ssid = "Zima";     // nastaví jméno Wi-Fi sítě
const char *password = "Ro2uJjej"; // nastaví heslo Wi-Fi sítě

Adafruit_BME280 bme; // inicializuje objekt pro práci se senzorem BME280

void setup()
{
  Wire.begin (21, 22);
  Serial.begin(115200); // inicializuje sériovou komunikaci s rychlostí 115200 baudů

  Serial.println();               // vypíše prázdný řádek do sériového monitoru
  Serial.print("Connecting to "); // vypíše text "Connecting to " do sériového monitoru
  Serial.println(ssid);           // vypíše jméno Wi-Fi sítě do sériového monitoru

  WiFi.begin(ssid, password); // připojí se k Wi-Fi síti se jménem a heslem uloženým v proměnných ssid a password

  while (WiFi.status() != WL_CONNECTED)
  {                    // dokud ESP32-C3 není připojeno k Wi-Fi síti
    delay(1000);       // čeká 1 sekundu
    Serial.print("."); // vypíše tečku do sériového monitoru
  }

  Serial.println("");               // vypíše prázdný řádek do sériového monitoru
  Serial.println("WiFi connected"); // vypíše text "WiFi connected" do sériového monitoru
  Serial.println("IP address: ");   // vypíše text "IP address: " do sériového monitoru
  Serial.println(WiFi.localIP());   // vypíše IP adresu přiřazenou ESP32-C3 v síti do sériového monitoru

  if (!bme.begin(0x76))
  {                                                                        // pokud se nepodaří inicializovat senzor BME280
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); // vypíše chybovou hlášku do sériového monitoru
    while (1); // zastaví program
  }
}

void loop()
{
  float temperature = bme.readTemperature();    // načte teplotu ze senzoru BME280
  float humidity = bme.readHumidity();          // načte vlhkost ze senzoru BME280
  float pressure = bme.readPressure() / 100.0F; // načte tlak ze senzoru BME280 a převede ho na hektopaskaly

  Serial.print("Temperature = "); // vypíše text "Temperature = " do sériového monitoru
  Serial.print(temperature);      // vypíše hodnotu teploty do sériového monitoru
  Serial.println(" *C");          // vypíše jednotku teploty do sériového monitoru

  Serial.print("Humidity = "); // vypíše text "Humidity = " do sériového monitoru
  Serial.print(humidity);      // vypíše hodnotu vlhkosti do sériového monitoru
  Serial.println(" %");        // vypíše jednotku vlhkosti do sériového monitoru

  Serial.print("Pressure = "); // vypíše text "Pressure = " do sériového monitoru
  Serial.print(pressure);      // vypíše hodnotu tlaku do sériového monitoru
  Serial.println(" hPa");      // vypíše jednotku tlaku do sériového monitoru

  delay(1000); // čeká 1 sekundu
}
*/


//**********************************************
//Prompt:
//Proč je po řádku Serial.begin(115200); zpoždění jedna sekunda?
//**********************************************
//Zpoždění po Serial.begin(115200); je často používáno k tomu, aby se zajistilo, že sériová komunikace je plně 
//inicializována předtím, než program pokračuje. Toto zpoždění je obzvláště užitečné při ladění, protože bez něj 
//by mohly být některé počáteční výstupy ztraceny. Nicméně, v tomto konkrétním kódu to nemusí být nezbytné a může 
//být odstraněno bez jakýchkoli negativních důsledků. Zpoždění 1 sekundy je zde pravděpodobně zvykem autora kódu.



//**********************************************
//Prompt:
//Můžeš mi, prosím, do předešlého programu doplnit čtení všech parametrů ze senzoru BME280 s výpisem do sériového 
//monitoru jednou za 1s?
//**********************************************
/*
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const char* ssid = "Zima24_guest"; // nastaví jméno Wi-Fi sítě
const char* password = "Zimikovi812/"; // nastaví heslo Wi-Fi sítě

Adafruit_BME280 bme; // inicializuje objekt pro práci se senzorem BME280

void setup() {
  Serial.begin(115200); // inicializuje sériovou komunikaci s rychlostí 115200 baudů
  delay(1000); // čeká 1 sekundu

  Serial.println(); // vypíše prázdný řádek do sériového monitoru
  Serial.print("Connecting to "); // vypíše text "Connecting to " do sériového monitoru
  Serial.println(ssid); // vypíše jméno Wi-Fi sítě do sériového monitoru

  WiFi.begin(ssid, password); // připojí se k Wi-Fi síti se jménem a heslem uloženým v proměnných ssid a password

  while (WiFi.status() != WL_CONNECTED) { // dokud ESP32-C3 není připojeno k Wi-Fi síti
    delay(1000); // čeká 1 sekundu
    Serial.print("."); // vypíše tečku do sériového monitoru
  }

  Serial.println(""); // vypíše prázdný řádek do sériového monitoru
  Serial.println("WiFi connected"); // vypíše text "WiFi connected" do sériového monitoru
  Serial.println("IP address: "); // vypíše text "IP address: " do sériového monitoru
  Serial.println(WiFi.localIP()); // vypíše IP adresu přiřazenou ESP32-C3 v síti do sériového monitoru

  if (!bme.begin(0x76)) { // pokud se nepodaří inicializovat senzor BME280
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); // vypíše chybovou hlášku do sériového monitoru
    while (1); // zastaví program
  }
}

void loop() {
  float temperature = bme.readTemperature(); // načte teplotu ze senzoru BME280
  float humidity = bme.readHumidity(); // načte vlhkost ze senzoru BME280
  float pressure = bme.readPressure() / 100.0F; // načte tlak ze senzoru BME280 a převede ho na hektopaskaly

  Serial.print("Temperature = "); // vypíše text "Temperature = " do sériového monitoru
  Serial.print(temperature); // vypíše hodnotu teploty do sériového monitoru
  Serial.println(" *C"); // vypíše jednotku teploty do sériového monitoru

  Serial.print("Humidity = "); // vypíše text "Humidity = " do sériového monitoru
  Serial.print(humidity); // vypíše hodnotu vlhkosti do sériového monitoru
  Serial.println(" %"); // vypíše jednotku vlhkosti do sériového monitoru

  Serial.print("Pressure = "); // vypíše text "Pressure = " do sériového monitoru
  Serial.print(pressure); // vypíše hodnotu tlaku do sériového monitoru
  Serial.println(" hPa"); // vypíše jednotku tlaku do sériového monitoru

  delay(1000); // čeká 1 sekundu
}
*/



//**********************************************
//Prompt:
//Prosím tě, napiš mi program pro ESP32-C3 v arduino ide 2.0, pro komunikaci po wi-fi s notebookem ve stejné síti. 
//Rád bych, aby jsem v sériovém monitoru viděl postupně jednotlivé fáze připojování. Místo skutečného SSID a hesla 
//v programu napiš jen your_SSID a your_PSWD.
//**********************************************
/*
#include <WiFi.h> // přidává knihovnu pro práci s Wi-Fi

const char* ssid = "Zima24_guest"; // nastaví jméno Wi-Fi sítě
const char* password = "Zimikovi812/"; // nastaví heslo Wi-Fi sítě

void setup() {
  Serial.begin(115200); // inicializuje sériovou komunikaci s rychlostí 115200 baudů
  delay(1000); // čeká 1 sekundu

  Serial.println(); // vypíše prázdný řádek do sériového monitoru
  Serial.print("Connecting to "); // vypíše text "Connecting to " do sériového monitoru
  Serial.println(ssid); // vypíše jméno Wi-Fi sítě do sériového monitoru

  WiFi.begin(ssid, password); // připojí se k Wi-Fi síti se jménem a heslem uloženým v proměnných ssid a password

  while (WiFi.status() != WL_CONNECTED) { // dokud ESP32-C3 není připojeno k Wi-Fi síti
    delay(1000); // čeká 1 sekundu
    Serial.print("."); // vypíše tečku do sériového monitoru
  }

  Serial.println(""); // vypíše prázdný řádek do sériového monitoru
  Serial.println("WiFi connected"); // vypíše text "WiFi connected" do sériového monitoru
  Serial.println("IP address: "); // vypíše text "IP address: " do sériového monitoru
  Serial.println(WiFi.localIP()); // vypíše IP adresu přiřazenou ESP32-C3 v síti do sériového monitoru
}

void loop() {
  // your code here
}
*/



//**********************************************
//Hallo word
//**********************************************
/*
void setup()
{
    Serial.begin(57600);
}

void loop()
{
    int counter = 0;
    while (1)
    {
        Serial.print("Hodnota promenne je: ");
        Serial.println(counter++);
        delay(500);
    }
}
*/