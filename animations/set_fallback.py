import sys
from binascii import unhexlify
from ESP32NeopixelServer import ESP32NeopixelServer

from ESP32Config import config

host = config["host"]

if len(sys.argv) != 3:
	print("Usage: python3 set_fallback.py <id> <hex-arguments ...>")

index = int(sys.argv[1])
args = unhexlify(sys.argv[2])

stripe = ESP32NeopixelServer(host)
stripe.setFallback(index, args)

print("DONE!")
