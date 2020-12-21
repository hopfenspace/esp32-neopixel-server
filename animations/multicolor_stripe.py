from ESP32NeopixelServer import ESP32NeopixelServer
import time

HOST = "10.42.0.222"
NUM_PIXEL = 300
SLEEP_SEC = 0.3
BLANK_COUNT = 3
BRIGHTNESS = 50

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
	color = ESP32NeopixelServer.adjustBrightness(color, BRIGHTNESS)
	allPixels.append(color)
	for i in range(BLANK_COUNT):
		allPixels.append(0x000000)

allPixels = (NUM_PIXEL // len(allPixels) + 1) * allPixels
stripe = ESP32NeopixelServer(HOST)

while True:
	stripe.setMultiplePixel(0, allPixels[0 : NUM_PIXEL])
	stripe.show()

	first = allPixels[0]
	allPixels = allPixels[1 : ] + [first]

	time.sleep(SLEEP_SEC)
