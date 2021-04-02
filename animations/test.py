from ESP32NeopixelServer import ESP32NeopixelServer
import time

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]

colors = [
	0xff0000,
	0x00ff00,
	0xffff00,
	0xff00ff,
	0x00ffff,
	0x0000ff,
]

colors = (num_pixel // len(colors)) * colors
stripe = ESP32NeopixelServer(host)

while True:
	stripe.fillRange(0, num_pixel, 0xff0000)
	stripe.show()
	time.sleep(1)


	stripe.fillRange(0, num_pixel, 0x00ff00)
	stripe.show()
	time.sleep(1)


	stripe.fillRange(0, num_pixel, 0x0000ff)
	stripe.show()
	time.sleep(1)

	for i in range(0, 20):
		stripe.setMultiplePixel(0, colors)
		stripe.show()
		time.sleep(0.1)

		first = colors[0]
		colors = colors[1 : ] + [first]

	stripe.fillRange(0, num_pixel, 0)
	stripe.show()
	time.sleep(sleep_sec)
