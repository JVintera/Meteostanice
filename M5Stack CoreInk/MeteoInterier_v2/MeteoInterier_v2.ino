// Stav baterie zobrazit
// Signalizovat nizky stav baterie blikanim LED
// Dodelat wifi
// komunikaci s tmep.cz
// Overit mereni
// Stahovat data z TMEP.cz

#include "M5CoreInk.h"
// Knihovny k senzoru ENV II
#include "M5_ENV.h"
#include <Adafruit_BMP280.h>
#include "Adafruit_Sensor.h"
#include "SHT85.h" //Problem s SHT30, resetovalo MCU, vzresen jinou knihovnou
// Knihovny k Wi-fi
#include <WiFi.h>

#define SHT30_ADDRESS 0x44

// Vytvoření instance | Instance creation
Ink_Sprite InkPageSprite(&M5.M5Ink);
Adafruit_BMP280 bme;
//SHT3X sht30;
SHT85 sht30;

// Merene veliciny
float pressBMP = 0.0;
float tempBMP = 23.6;
float tempSHT = 18.3;
float humSHT = 0.0;

void setup() {
  // Initialize CoreInk
  M5.begin(); 
  Serial.begin(115200);
  if (!M5.M5Ink.isInit())
  { // check if the initialization is successful.
    Serial.printf("Ink Init faild");
  }
  M5.M5Ink.clear(); // Clear the screen.
  delay(1000);
  // creat ink Sprite.
  if (InkPageSprite.creatSprite(0, 0, 200, 200, true) != 0)
  {
    Serial.printf("Ink Sprite creat faild");
  }

  sht30.begin(SHT30_ADDRESS);

  // Test displeje
  InkPageSprite.drawString(20, 20, "Hello Core-INK");
  InkPageSprite.pushSprite();
  delay(2000);
  M5.M5Ink.clear(); // Clear the screen.
}



void loop() {
  //Cteni hodnot
  while (!bme.begin(0x76))   //ENVII má adresu 0x76
  { // Init the sensor of bme
    InkPageSprite.drawString(20, 20, "Could not find");
    InkPageSprite.drawString(20, 40, "a valid BMP280");
    InkPageSprite.pushSprite();
  }
  pressBMP = bme.readPressure(); // Stores the pressure gained by BMP.
  tempBMP = bme.readTemperature(); 

//  sht30.get();                   // Obtain the data of shT30.
  sht30.read();
  tempSHT = sht30.getTemperature();
  humSHT = sht30.getHumidity();
           //  tempSHT = sht30.cTemp;             // Store the temperature obtained from shT30.
           //  humSHT = sht30.humidity;

           // Vypis na seriove lince
           Serial.println("   VNITRNI MERENI");
  Serial.print("BMP pressure: ");
  Serial.println(pressBMP);
  Serial.print("BMP temperature: ");
  Serial.println(tempBMP);
  Serial.print("SHT temperature: ");
  Serial.println(tempSHT);
  Serial.print("SHT humidity: ");
  Serial.println(humSHT);

  //Vypis na displeji
  InkPageSprite.drawString(5, 5, "VNITRNI MERENI");

  char sPressBMP[10];
  dtostrf(pressBMP, 2, 2, sPressBMP);
  InkPageSprite.drawString(5, 30, "BMP280 pres.: ");
  InkPageSprite.drawString(130, 30, sPressBMP);
  char sTempBMP[10];
  dtostrf(tempBMP, 2, 2, sTempBMP);
  InkPageSprite.drawString(5, 50, "BMP280 temper.: ");
  InkPageSprite.drawString(153, 50, sTempBMP);
  char sTempSHT[10];
  dtostrf(tempSHT, 2, 2, sTempSHT);
  InkPageSprite.drawString(5, 70, "SHT30 temper.: ");
  InkPageSprite.drawString(153, 70, sTempSHT);
  char sHumSHT[10];
  dtostrf(humSHT, 2, 2, sHumSHT);
  InkPageSprite.drawString(5, 90, "SHT30 humidity: ");
  InkPageSprite.drawString(153, 90, sHumSHT);
  InkPageSprite.pushSprite();

  /*
  // ***** TLACITKA *****
    if (M5.BtnUP.wasPressed())
      ButtonTest("Btn UP Pressed"); // Scroll wheel up.  拨轮向上滚动
    if (M5.BtnDOWN.wasPressed())
      ButtonTest("Btn DOWN Pressed"); // Trackwheel scroll down.  拨轮向下滚动
    if (M5.BtnMID.wasPressed())
      ButtonTest("Btn MID Pressed"); // Dial down.  拨轮按下
    if (M5.BtnEXT.wasPressed())
      ButtonTest("Btn EXT Pressed"); // Top button press.  顶部按键按下
    if (M5.BtnPWR.wasPressed())
    { // Right button press.  右侧按键按下
      ButtonTest("Btn PWR Pressed");
      M5.shutdown(); // Turn off the power, restart it, you need to wake up through the PWR button.  关闭电源,再次启动需要通过PWR按键唤醒
    }
  */
  M5.update(); // Refresh device button.
  delay(10000);
}

/*
// ***** TLACITKA *****
void ButtonTest(char *str)
{
  InkPageSprite.clear();                 // clear the screen.  清屏
  InkPageSprite.drawString(35, 59, str); // draw the string.  绘制字符串
  InkPageSprite.pushSprite();            // push the sprite.  推送图片
  delay(2000);
}
*/