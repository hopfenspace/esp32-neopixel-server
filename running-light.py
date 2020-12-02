import socket
import struct
import time

HOST = "10.42.0.222"
NUM_PIXEL = 60

def color2rgb(color):
	return (color >> 16), (color >> 8) & 0xff, color & 0xff

def packetShow():
	return bytes([0])

def packetSetPixel(offset, color):
	r, g, b = color2rgb(color)
	return struct.pack("!BHBBB", 0x01, offset, r, g, b)

def packetFill(offset, length, color):
	r, g, b = color2rgb(color)
	return struct.pack("!BHHBBB", 0x02, offset, length, r, g, b)

def runningLight(sock, color):
	for i in range(0, NUM_PIXEL):
		packet = bytes()
		packet += packetFill(0, NUM_PIXEL, 0x000000)
		packet += packetSetPixel(i, color)
		packet += packetSetPixel(i + 1, color)
		packet += packetSetPixel(i + 2, color)
		packet += packetShow()

		sock.send(packet)
		time.sleep(0.2)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
sock.connect((HOST, 1337))

runningLight(sock, 0xff0000)
runningLight(sock, 0x00ff00)
runningLight(sock, 0x0000ff)
