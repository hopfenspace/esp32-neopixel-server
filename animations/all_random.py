from ESP32NeopixelServer import ESP32NeopixelServer
import time, random

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]
brightness = config["brightness"]

colors = [
	0x7f00ff,
	0x3f00ff,
	0x0000ff,
	0x00ff00,
	0xffff00,
	0xff7f00,
	0xff0000,
	0x000000,
]

for i in range(len(colors)):
	colors[i] = ESP32NeopixelServer.adjustBrightness(colors[i], brightness)

stripe = ESP32NeopixelServer(host)

while True:
	pixels = [random.choice(colors) for _ in range(num_pixel)]
	stripe.setMultiplePixel(0, pixels)
	stripe.show()

	time.sleep(sleep_sec)
