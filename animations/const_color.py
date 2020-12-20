from ESP32NeopixelServer import ESP32NeopixelServer
import sys

HOST = "10.42.0.222"
NUM_PIXEL = 300

DEFAULT_COLOR = 0xffffff

stripe = ESP32NeopixelServer(HOST)

color = DEFAULT_COLOR
if len(sys.argv) > 1:
    color = int(sys.argv[1], 16)

stripe.fillRange(0, NUM_PIXEL, color)
stripe.show()
