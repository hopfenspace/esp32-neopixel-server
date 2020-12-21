from ESP32NeopixelServer import ESP32NeopixelServer
import time
import random

HOST = "10.42.0.222"
NUM_PIXEL = 300

BRIGHTNESS = 10
BASE_COLOR = 0xff7700
SPARKE_COLOR = 0xffffff
SPARKLE_COUNT = 5

stripe = ESP32NeopixelServer(HOST)
baseColor = ESP32NeopixelServer.adjustBrightness(BASE_COLOR, BRIGHTNESS)

stripe.fillRange(0, NUM_PIXEL, baseColor)
while True:
	sparkles = []
	for i in range(SPARKLE_COUNT):
		index = random.randint(0, NUM_PIXEL)
		sparkles.append(index)

		stripe.setPixel(index, SPARKE_COLOR)

	stripe.show()
	time.sleep(0.2)

	for index in sparkles:
		stripe.setPixel(index, baseColor)
	stripe.show()

	time.sleep(random.randrange(0, 50) / 50)
