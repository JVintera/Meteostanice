# Měření teloty a vlhkosti pomocí BME280 (I2C na pinech: GP1 SDA, GP0 SCL)
#
# Návod
# https://learn.adafruit.com/pico-w-wifi-with-circuitpython/pico-w-with-adafruit-io
#
# API
# https://docs.circuitpython.org/projects/bme280/en/latest/api.html#adafruit_bme280.advanced.Adafruit_BME280_I2C
#
# Library
# https://github.com/adafruit/Adafruit_CircuitPython_BME280/blob/main/examples/bme280_normal_mode.py



import os
import time
import ssl
import wifi
import socketpool
import microcontroller
import board
import busio
import adafruit_requests
#import adafruit_ahtx0
from adafruit_bme280 import basic as adafruit_bme280
from adafruit_io.adafruit_io import IO_HTTP, AdafruitIO_RequestError

try:
    wifi.radio.connect(os.getenv('CIRCUITPY_WIFI_SSID'), os.getenv('CIRCUITPY_WIFI_PASSWORD'))
except TypeError:
    print("Could not find WiFi info. Check your settings.toml file!")
    raise

try:
    aio_username = os.getenv('ADAFRUIT_AIO_USERNAME')
    aio_key = os.getenv('ADAFRUIT_AIO_KEY')
except TypeError:
    print("Could not find Adafruit IO info. Check your settings.toml file!")
    raise

pool = socketpool.SocketPool(wifi.radio)
requests = adafruit_requests.Session(pool, ssl.create_default_context())
# Initialize an Adafruit IO HTTP API object
io = IO_HTTP(aio_username, aio_key, requests)
print("connected to io")

#  use Pico W's GP0 for SDA and GP1 for SCL
i2c = busio.I2C(board.GP1, board.GP0)
bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c)


try:
# get feed
    picowTemp_feed = io.get_feed("pitemp")
    picowHumid_feed = io.get_feed("pihumid")
except AdafruitIO_RequestError:
# if no feed exists, create one
    picowTemp_feed = io.create_new_feed("pitemp")
    picowHumid_feed = io.create_new_feed("pihumid")

#  pack feed names into an array for the loop
feed_names = [picowTemp_feed, picowHumid_feed]
print("feeds created")

clock = 5

while True:
    try:
        #  when the clock runs out..
        if clock > 5:
            #  read sensor
            data = [bme280.temperature, bme280.relative_humidity]
            #  send sensor data to respective feeds
            for z in range(2):
                io.send_data(feed_names[z]["key"], data[z])
                print("sent %0.1f" % data[z])
                time.sleep(1)
            #  print sensor data to the REPL
            print("\nTemperature: %0.1f C" % bme280.temperature)
            print("Humidity: %0.1f %%" % bme280.relative_humidity)
            print()
            time.sleep(1)
            #  reset clock
            clock = 0
        else:
            clock += 1
    # pylint: disable=broad-except
    #  any errors, reset Pico W
    except Exception as e:
        print("Error:\n", str(e))
        print("Resetting microcontroller in 10 seconds")
        time.sleep(10)
        microcontroller.reset()
    #  delay
    time.sleep(1)
    print(clock)
