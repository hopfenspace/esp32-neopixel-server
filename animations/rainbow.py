from ESP32NeopixelServer import ESP32NeopixelServer
import time

HOST = "10.42.0.222"
NUM_PIXEL = 300
SLEEP_SEC = 0.3
RAINBOW_LENGTH = 32
BRIGHTNESS = 5

def colorTransition(fromColor, toColor, position, brightness):
	r1, g1, b1 = ESP32NeopixelServer.color2rgb(fromColor)
	r2, g2, b2 = ESP32NeopixelServer.color2rgb(toColor)

	dr = r2 - r1
	dg = g2 - g1
	db = b2 - b1

	r = int(r1 + position * dr) * brightness // 100
	g = int(g1 + position * dg) * brightness // 100
	b = int(b1 + position * db) * brightness // 100
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
		color = colorTransition(colors[i], colors[i2], j / transitionLen, BRIGHTNESS)
		allPixels.append(color)

allPixels = (NUM_PIXEL // len(allPixels)) * allPixels
stripe = ESP32NeopixelServer(HOST)

while True:
	stripe.setMultiplePixel(0, allPixels)
	stripe.show()

	first = allPixels[0]
	allPixels = allPixels[1 : ] + [first]

	time.sleep(SLEEP_SEC)
