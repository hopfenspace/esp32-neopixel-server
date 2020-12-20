from ESP32NeopixelServer import ESP32NeopixelServer
import time

HOST = "10.42.0.222"
NUM_PIXEL = 300

colors = [
	0xff0000,
	0x00ff00,
	0xffff00,
	0xff00ff,
	0x00ffff,
	0x0000ff,
]

colors = (NUM_PIXEL // len(colors)) * colors
stripe = ESP32NeopixelServer(HOST)

while True:
	stripe.fillRange(0, NUM_PIXEL, 0xff0000)
	stripe.show()
	time.sleep(1)


	stripe.fillRange(0, NUM_PIXEL, 0x00ff00)
	stripe.show()
	time.sleep(1)


	stripe.fillRange(0, NUM_PIXEL, 0x0000ff)
	stripe.show()
	time.sleep(1)

	for i in range(0, 20):
		stripe.setMultiplePixel(0, colors)
		stripe.show()
		time.sleep(0.1)

		first = colors[0]
		colors = colors[1 : ] + [first]

	stripe.fillRange(0, NUM_PIXEL, 0)
	stripe.show()
	time.sleep(1)