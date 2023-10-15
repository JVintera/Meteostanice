//ESP32C3 Dev Module



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