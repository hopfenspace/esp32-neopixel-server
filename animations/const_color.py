from ESP32NeopixelServer import ESP32NeopixelServer
import sys

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]

DEFAULT_COLOR = 0xffffff

stripe = ESP32NeopixelServer(host)

color = DEFAULT_COLOR
if len(sys.argv) > 1:
    color = int(sys.argv[1], 16)

stripe.fillRange(0, num_pixel, color)
stripe.show()
