from ESP32NeopixelServer import ESP32NeopixelServer
import time

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]
brightness = config["brightness"]

BLANK_COUNT = 9

colors = [
	0xff0000,
	0x00ff00,
	0x0000ff,
	0xffff00,
	0xff00ff,
	0x00ffff,
]

allPixels = []
for color in colors:
	color = ESP32NeopixelServer.adjustBrightness(color, brightness)
	allPixels.append(color)
	for i in range(BLANK_COUNT):
		allPixels.append(0x000000)

allPixels = (num_pixel // len(allPixels) + 1) * allPixels
stripe = ESP32NeopixelServer(host)

while True:
	stripe.setMultiplePixel(0, allPixels[0 : num_pixel])
	stripe.show()

	first = allPixels[0]
	allPixels = allPixels[1 : ] + [first]

	time.sleep(sleep_sec)
