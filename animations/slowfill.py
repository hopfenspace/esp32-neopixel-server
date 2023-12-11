from ESP32NeopixelServer import ESP32NeopixelServer
import time
import random

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]
brightness = config["brightness"]

COLOR = 0xff7700
BRIGHTNESS_DECREASE = -1
FILL_COUNT = 4

stripe = ESP32NeopixelServer(host)
color = ESP32NeopixelServer.adjustBrightness(COLOR, brightness)
indices = [i for i in range(num_pixel)]

while True:
	random.shuffle(indices)
	stripe.fillRange(0, num_pixel, 0x000000)

	i = 0
	while i < num_pixel:
		for j in range(FILL_COUNT):
			stripe.setPixel(indices[i + j], color)

		i += FILL_COUNT
		stripe.show()
		time.sleep(sleep_sec)

	for i in range(brightness, 0, BRIGHTNESS_DECREASE):
		stripe.fillRange(0, num_pixel, ESP32NeopixelServer.adjustBrightness(COLOR, i))
		stripe.show()
		time.sleep(sleep_sec)
