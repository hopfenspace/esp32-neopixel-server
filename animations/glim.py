from ESP32NeopixelServer import ESP32NeopixelServer
import time
import random

HOST = "10.42.0.222"
NUM_PIXEL = 300

GLIM_COLOR = 0xff7700
GLIM_BRIGHTNESS = 50
GLIM_COUNT = 50
GLIM_SEC = 0.2

stripe = ESP32NeopixelServer(HOST)
glimColor = ESP32NeopixelServer.adjustBrightness(GLIM_COLOR, GLIM_BRIGHTNESS)

stripe.fillRange(0, NUM_PIXEL, 0x000000)

glimming = []
for i in range(GLIM_COUNT):
	index = random.randint(0, NUM_PIXEL)
	stripe.setPixel(index, glimColor)
	glimming.append(index)

while True:
	index = random.randint(0, NUM_PIXEL)

	stripe.setPixel(index, glimColor)
	stripe.setPixel(glimming[0], 0x000000)
	stripe.show()

	glimming.append(index)
	glimming.pop(0)

	time.sleep(GLIM_SEC)
