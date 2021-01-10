# ESP32 Neopixel Server

Controlling WS2812 LEDs connected to your ESP32 via UDP.

![demo gif](https://i.m4gnus.de/15aa1b.gif)

## Notes
- Setting and Filling pixels are not shown immediately, but after an `Update LEDs` packet.
- LED count, WiFi mode, WiFi SSID and WiFi Password are saved in non-volatile memory
- Set an empty password for open networks
- When the configured WiFi is not available the device will fall back to AP mode
- In AP mode the SSID is `NeopixelServer-XXXXXX` with `XXXXXX` being the last 3 byte of the mac address in hex
- In AP mode the Password is empty (open network)

## Packets

### Update LEDs (0x00)
- Layout: `0x00`
- Response: `"ok\n"`
- Example: `0x00`

### Set Pixel (0x01)
- Layout: `0x01 <16-bit offset> <8-bit red> <8-bit green> <8-bit blue>`
- Response: None
- Example: `0x01000100ff00` sets pixel 1 to green

### Fill Pixels (0x02)
- Layout: `0x02 <16-bit offset> <16-bit length> <8-bit red> <8-bit green> <8-bit blue>`
- Response: None
- Example: `0x010000000f0000ff` sets pixels 0 to 15 to blue

### Set multiple Pixels (0x03)
- Layout: `0x03 <16-bit offset> <16-bit length> <length times <8-bit red> <8-bit green> <8-bit blue>>`
- Reponse: None
- Example: `0x030010000200ff00ff0000` set led 0x10 to green and 0x11 to red

### Set LED Count (0x10)
- Layout: `0x10 <16-bit length>`
- Response: `"ok\n"`
- Example: `0x10003C` sets LED count to 60

### Select LED Pin (0x11)
- Layout: `0x10 <8-bit index>`
- Response: `"ok\n"` or `"error: pin index out of range\n"`
- Example: `0x1103` selects the third configured WS2812 pin (not GPIO3)

### Set WiFi Mode (0x20)
- Layout: `0x20 <either 0x00 for AP or 0x01 for STA>`
- Response: `"ok\n"`
- Example: `0x2001` sets mode to STA

### Set WiFi SSID (0x21)
- Layout: `0x21 <8-bit length> <length byte SSID>`
- Response: `"ok\n"`
- Example: `"\x21\x08Freifunk"` sets WiFi SSID to Freifunk

### Set WiFi Password (0x22)
- Layout: `0x21 <8-bit length> <length byte SSID>`
- Response: `"ok\n"`
- Example: `"\x21\x09hello1234"` sets WiFi Password to "hello1234"

### Set Digital Output (0x30)
- Layout: `0x30 <16-bit output state>`
- Response: `"ok\n"`
- Example: `30ffff` set all ports to HIGH

### Reboot (0xff)
- Layout: `0xff`
- Response: `"rebooting...\n"`
- Example: `0xff`
