from ESP32NeopixelServer import ESP32NeopixelServer
import time

HOST = "10.42.0.222"
NUM_PIXEL = 300

colors = [
	0x220000,
    0x000000,
    0x000000,
    0x000000,
	0x002200,
    0x000000,
    0x000000,
    0x000000,
	0x222200,
    0x000000,
    0x000000,
    0x000000,
	0x220022,
    0x000000,
    0x000000,
    0x000000,
	0x002222,
    0x000000,
    0x000000,
    0x000000,
	0x000022,
    0x000000,
    0x000000,
    0x000000,
]

colors = (NUM_PIXEL // len(colors)) * colors
stripe = ESP32NeopixelServer(HOST)

while True:
    stripe.setMultiplePixel(0, colors)
    stripe.show()

    first = colors[0]
    colors = colors[1 : ] + [first]

    time.sleep(0.5)
