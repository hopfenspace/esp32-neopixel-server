import os
import json

config = {
	"host": "192.168.4.1",
	"num_pixel": 300,
	"brightness": 100,
	"sleep_sec": 0.05,
}

conffile = os.getenv("ESP32NEOPIXEL_CONFIG")
if conffile is not None:
	with open(conffile, "r") as fd:
		options = json.load(fd)
		for key in config:
			if key in options:
				config[key] = options[key]
