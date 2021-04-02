from ESP32NeopixelServer import ESP32NeopixelServer
import time
import random

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]
brightness = config["brightness"]

BASE_COLOR = 0xff7700
SPARKE_COLOR = 0xffffff
SPARKLE_COUNT = 2

stripe = ESP32NeopixelServer(host)
baseColor = ESP32NeopixelServer.adjustBrightness(BASE_COLOR, brightness)

stripe.fillRange(0, num_pixel, baseColor)
while True:
	sparkles = []
	for i in range(SPARKLE_COUNT):
		index = random.randint(0, num_pixel)
		sparkles.append(index)

		stripe.setPixel(index, SPARKE_COLOR)

	stripe.show()
	time.sleep(0.05)

	for index in sparkles:
		stripe.setPixel(index, baseColor)
	stripe.show()

	time.sleep(random.randrange(0, sleep_sec * 100) / sleep_sec * 100)
