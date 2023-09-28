from m5stack import *
from m5ui import *
from uiflow import *
import time
import unit
import math

setScreenColor(lcd.WHITE) 
env2_0 = unit.get(unit.ENV2, unit.PORTA)


hour = 0
minute = 1


Temp = M5TextBox(13, 29, "C", lcd.FONT_UNICODE, lcd.BLACK, rotate=0)
Termostat = M5Title(title="Termostat", x=3, fgcolor=lcd.WHITE, bgcolor=lcd.BLACK)
Humid = M5TextBox(13, 55, "%", lcd.FONT_UNICODE, lcd.BLACK, rotate=0)
Press = M5TextBox(13, 80, "Pa", lcd.FONT_UNICODE, lcd.BLACK, rotate=0)
Time = M5TextBox(13, 110, "hh:mm", lcd.FONT_DejaVu56, lcd.BLACK, rotate=0)
Date = M5TextBox(13, 165, "dd. mm. yy", lcd.FONT_UNICODE, lcd.BLACK, rotate=0)
batteryVoltage = M5TextBox(190, 90, "Battery voltage", lcd.FONT_DefaultSmall, lcd.BLACK, rotate=270)


def buttonUP_wasPressed():
  global hour
  hour = (hour if isinstance(hour, Number) else 0) + 1
  coreInkShow()
  pass
btnUP.wasPressed(buttonUP_wasPressed)

def buttonMID_wasPressed():
  # global params
  pass
btnMID.wasPressed(buttonUP_wasPressed)

def buttonDOWN_wasPressed():
  global minute
  minute = minute + 1
  coreInkShow()
  pass
btnDOWN.wasPressed(buttonUP_wasPressed)


M5Led.off()
rtc.set_datetime((23, 7, 5, 3, minute, hour, 0))
while True:
  batteryVoltage.setText(str(bat.voltage()))
  Temp.setText(str((str((env2_0.temperature)) + str(' °C'))))
  Humid.setText(str((str((env2_0.humidity)) + str(' %'))))
  Press.setText(str((str(round(env2_0.pressure,2)) + str(' hPa'))))
  Time.setText(str((str((rtc.datetime()[4])) + str(((str(':') + str((rtc.datetime()[5]))))))))
  Date.setText(str((str((rtc.datetime()[2])) + str(((str('. ') + str(((str((rtc.datetime()[1])) + str(((str(': ') + str((rtc.datetime()[0]))))))))))))))
#  coreInkShow()
#  wait(60)
  wait_ms(2)
  
  
  
  
  