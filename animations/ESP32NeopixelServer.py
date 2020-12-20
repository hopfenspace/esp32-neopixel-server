import socket
import struct

class ESP32NeopixelServer:
	def __init__(self, host, port=1337):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
		self.sock.connect((host, 1337))

		self.buffer = bytes()

	def sendBytes(self, buff, flush=False):
		if len(buff) > 1024:
			raise Exception("Given too large buffer to sendBytes ({} > 1024)".format(len(buff)))

		if len(self.buffer) + len(buff) < 1024:
			self.buffer += buff
		else:
			self.sock.send(self.buffer)
			self.buffer = buff

		if flush:
			self.sock.send(self.buffer)
			self.buffer = bytes()

	def color2rgb(self, color):
		return (color >> 16), (color >> 8) & 0xff, color & 0xff

	def show(self):
		self.sendBytes(bytes([0]), True)

	def setPixel(self, index, color):
		r, g, b = self.color2rgb(color)
		packet = struct.pack("!BHBBB", 0x01, index, r, g, b)
		self.sendBytes(packet)

	def fillRange(self, startIndex, length, color):
		r, g, b = self.color2rgb(color)
		packet = struct.pack("!BHHBBB", 0x02, startIndex, length, r, g, b)
		self.sendBytes(packet)

	def setMultiplePixel(self, startIndex, colors):
		packet = struct.pack("!BHH", 0x03, startIndex, len(colors))
		for color in colors:
			r, g, b = self.color2rgb(color)
			packet += struct.pack("!BBB", r, g, b)

		self.sendBytes(packet)


	# TODO... settings commands
