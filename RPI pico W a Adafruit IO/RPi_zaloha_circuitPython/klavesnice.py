# Návod
# https://www.instructables.com/DIY-Macro-Keyboard-Using-a-Raspberry-PI-Pico/
# Knihovna
# https://github.com/adafruit/Adafruit_CircuitPython_HID/tree/main
# API knihovny
# https://docs.circuitpython.org/projects/hid/en/latest/index.html

import board
import digitalio
import time
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode


keyboard = Keyboard(usb_hid.devices)


ledPin1 = board.GP1   # pin to connect LED to
ledPin2 = board.GP2
ledPin3 = board.GP3

btnPin20 = board.GP20       # pin to connect button to
btnPin21 = board.GP21
btnPin22 = board.GP22


# Initializing LED
led1 = digitalio.DigitalInOut(ledPin1)
led1.direction = digitalio.Direction.OUTPUT

led2 = digitalio.DigitalInOut(ledPin2)
led2.direction = digitalio.Direction.OUTPUT

led3 = digitalio.DigitalInOut(ledPin3)
led3.direction = digitalio.Direction.OUTPUT

# Initializing Button
btn20 = digitalio.DigitalInOut(btnPin20)
btn20.direction = digitalio.Direction.INPUT
btn20.pull = digitalio.Pull.DOWN

btn21 = digitalio.DigitalInOut(btnPin21)
btn21.direction = digitalio.Direction.INPUT
btn21.pull = digitalio.Pull.DOWN

btn22 = digitalio.DigitalInOut(btnPin22)
btn22.direction = digitalio.Direction.INPUT
btn22.pull = digitalio.Pull.DOWN


led = False



while True:
	# Check if button is pressed and if it is, to press the Macros and toggle LED
    if btn20.value == 0:  
        print(" A ")
        keyboard.press(Keycode.A)
        time.sleep(0.15)
        keyboard.release(Keycode.A)
        led = not led
        led1.value = led
    if btn21.value == 0:
        print(" S ")
        keyboard.press(Keycode.S)
        time.sleep(0.15)
        keyboard.release(Keycode.S)
    if btn22.value == 0:
        print(" D ")
        keyboard.press(Keycode.D)
        time.sleep(0.15)
        keyboard.release(Keycode.D)
    time.sleep(0.1)

