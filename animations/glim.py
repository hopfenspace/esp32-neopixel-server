from ESP32NeopixelServer import ESP32NeopixelServer
import time
import random

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]
brightness = config["brightness"]

GLIM_COLOR = 0xff7700
GLIM_COUNT = 50

stripe = ESP32NeopixelServer(host)
glimColor = ESP32NeopixelServer.adjustBrightness(GLIM_COLOR, brightness)

stripe.fillRange(0, num_pixel, 0x000000)

glimming = []
for i in range(GLIM_COUNT):
	index = random.randint(0, num_pixel)
	stripe.setPixel(index, glimColor)
	glimming.append(index)

while True:
	index = random.randint(0, num_pixel)

	stripe.setPixel(index, glimColor)
	stripe.setPixel(glimming[0], 0x000000)
	stripe.show()

	glimming.append(index)
	glimming.pop(0)

	time.sleep(sleep_sec)
