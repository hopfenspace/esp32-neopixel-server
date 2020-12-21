from ESP32NeopixelServer import ESP32NeopixelServer
import time
import random

HOST = "10.42.0.222"
NUM_PIXEL = 300

COLOR = 0xff7700
BRIGHTNESS = 10
SLEEP_SEC = 0.1
FILL_COUNT = 2

stripe = ESP32NeopixelServer(HOST)
color = ESP32NeopixelServer.adjustBrightness(COLOR, BRIGHTNESS)
indices = [i for i in range(NUM_PIXEL)]

while True:
	random.shuffle(indices)
	stripe.fillRange(0, NUM_PIXEL, 0x000000)

	i = 0
	while i < NUM_PIXEL:
		for j in range(FILL_COUNT):
			stripe.setPixel(indices[i + j], color)

		i += FILL_COUNT
		stripe.show()
		time.sleep(SLEEP_SEC)

	for i in range(BRIGHTNESS, 0, -1):
		stripe.fillRange(0, NUM_PIXEL, ESP32NeopixelServer.adjustBrightness(COLOR, i))
		stripe.show()
		time.sleep(0.1)
