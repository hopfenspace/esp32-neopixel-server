from ESP32NeopixelServer import ESP32NeopixelServer
import time

from ESP32Config import config

host = config["host"]
num_pixel = config["num_pixel"]
sleep_sec = config["sleep_sec"]
brightness = config["brightness"]

RAINBOW_LENGTH = 32

def colorTransition(fromColor, toColor, position, bright):
	r1, g1, b1 = ESP32NeopixelServer.color2rgb(fromColor)
	r2, g2, b2 = ESP32NeopixelServer.color2rgb(toColor)

	dr = r2 - r1
	dg = g2 - g1
	db = b2 - b1

	r = int(r1 + position * dr) * bright // 100
	g = int(g1 + position * dg) * bright // 100
	b = int(b1 + position * db) * bright // 100
	return (r << 16) | (g << 8) | b

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

allPixels = []
for i in range(0, len(colors)):
	i2 = i + 1 if i < len(colors) - 1 else 0
	transitionLen = RAINBOW_LENGTH // len(colors)
	for j in range(transitionLen):
		color = colorTransition(colors[i], colors[i2], j / transitionLen, brightness)
		allPixels.append(color)

allPixels = (num_pixel // len(allPixels) + 1) * allPixels
stripe = ESP32NeopixelServer(host)

while True:
	stripe.setMultiplePixel(0, allPixels)
	stripe.show()

	first = allPixels[0]
	allPixels = allPixels[1 : ] + [first]

	time.sleep(sleep_sec)
