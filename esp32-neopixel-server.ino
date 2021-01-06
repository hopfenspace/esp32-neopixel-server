#include <WiFi.h>
#include <Preferences.h>
#include <Adafruit_NeoPixel.h>

//
// config parameter
//
#define WS2812_PINS 15, 17, 18, 21
#define WS2812_DEFAULT_COUNT 512
#define DIGITAL_PINS 1, 2, 3, 4, 5, 6
#define DIGITAL_DEFAULT_STATE LOW

const uint32_t bootColors[] = {
	0x000000,
	0xFF0000,
	0x00FF00,
	0x0000FF,
	0xFFFF00,
	0xFF00FF,
	0x00FFFF,
};
#define BOOTCOLORS_COUNT (sizeof(bootColors) / sizeof(*bootColors))
// end config





WiFiUDP server;

Adafruit_NeoPixel pixels(WS2812_DEFAULT_COUNT, 1, NEO_GRB + NEO_KHZ800);
const uint8_t ws2812Pins[] = { WS2812_PINS };
#define WS2812_PIN_COUNT (sizeof(ws2812Pins) / sizeof(*ws2812Pins))

const uint8_t digitalPins[] = { DIGITAL_PINS };
#define DIGITAL_PIN_COUNT (sizeof(digitalPins) / sizeof(*digitalPins))
_Static_assert(DIGITAL_PIN_COUNT <= 16, "Too many digital output pins! Maximum 16 allowed.");

Preferences preferences;
#define PREFERENCE_WIFI_MODE "wifi-mode" // 0 == AP, 1 == STA
#define PREFERENCE_WIFI_SSID "wifi-ssid"
#define PREFERENCE_WIFI_PSK "wifi-psk"
#define PREFERENCE_LED_COUNT "led-count"

void setup()
{
	pixels.setPin(ws2812Pins[0]);
	pixels.begin();

	uint16_t pixelCount = preferences.getUShort(PREFERENCE_LED_COUNT, WS2812_DEFAULT_COUNT);
	if(pixelCount != WS2812_DEFAULT_COUNT)
		pixels.updateLength(pixelCount);

	pixels.clear();
	for(int i = 0; i < BOOTCOLORS_COUNT; i++)
		pixels.setPixelColor(i, bootColors[i]);
	pixels.show();

	for(int i = 0; i < DIGITAL_PIN_COUNT; i++)
	{
		pinMode(digitalPins[i], OUTPUT);
		digitalWrite(digitalPins[i], DIGITAL_DEFAULT_STATE);
	}

	preferences.begin("ws2812server", false);

	uint8_t wifiMode = preferences.getUChar(PREFERENCE_WIFI_MODE, 0);
	if(wifiMode == 1)
	{
		String ssid = preferences.getString(PREFERENCE_WIFI_SSID, "DefaultWiFi");
		String password = preferences.getString(PREFERENCE_WIFI_PSK, "");
		const char *pw;
		if(password == "")
			pw = NULL;
		else
			pw = password.c_str();

		WiFi.begin(ssid.c_str(), pw);

		for(int i = 0; i < 100; i++)
		{
			if(WiFi.status() == WL_CONNECTED)
				break;

			for(int j = 0; j < BOOTCOLORS_COUNT; j++)
				pixels.setPixelColor(j, bootColors[(i + j) % BOOTCOLORS_COUNT]);
			pixels.show();

			delay(100);
		}
	}

	if(wifiMode == 0 || WiFi.status() != WL_CONNECTED)
	{
		uint32_t mac = ESP.getEfuseMac() >> 24;
		char apSsid[64];
		sprintf(apSsid, "NeopixelServer-%06X", mac);

		WiFi.mode(WIFI_AP);
		WiFi.softAP(apSsid, NULL);
	}

	server.begin(1337);
}

uint16_t readUint16BE()
{
	return (server.read() << 8) | server.read();
}
uint32_t readColor()
{
	uint8_t red = server.read();
	uint8_t green = server.read();
	uint8_t blue = server.read();
	return Adafruit_NeoPixel::Color(red, green, blue);
}
String readString()
{
	char buff[256];
	uint8_t len = server.read();
	for(int i = 0; i < len; i++)
		buff[i] = server.read();

	buff[len] = 0;
	return String(buff);
}
void sendLine(const char *msg)
{
	server.beginPacket(); // will answer to ip/port last received a packet from
	server.print(msg);
	server.write('\n');
	server.endPacket();
}
void sendOk()
{
	sendLine("ok");
}

void loop()
{
	server.parsePacket();
	while(server.available() > 0)
	{
		uint8_t packetType = server.read();
		uint32_t color;
		uint16_t offset;
		uint16_t length;
		switch(packetType)
		{
			case 0x00: // update
				pixels.show();
				sendOk();
				break;
			case 0x01: // set pixel color
				offset = readUint16BE();
				color = readColor();
				pixels.setPixelColor(offset, color);
				break;
			case 0x02: // fill pixel range
				offset = readUint16BE();
				length = readUint16BE();
				color = readColor();
				pixels.fill(color, offset, length);
				break;
			case 0x03: // set multiple
				offset = readUint16BE();
				length = readUint16BE();
				for(uint16_t i = offset; i < offset + length; i++)
					pixels.setPixelColor(i, readColor());
				break;

			case 0x10: // set LED count
				length = readUint16BE();
				pixels.updateLength(length);
				preferences.putUShort(PREFERENCE_LED_COUNT, length);
				sendOk();
				break;
			case 0x11: // set LED Pin
				offset = server.read();
				if(offset < 0 || offset >= WS2812_PIN_COUNT)
				{
					sendLine("error: pin index out of range");
					break;
				}
				pixels.setPin(ws2812Pins[offset]);
				sendOk();
				break;

			case 0x20: // set wifi mode
				preferences.putUChar(PREFERENCE_WIFI_MODE, server.read() ? 0x01 : 0x00);
				sendOk();
				break;
			case 0x21: // set wifi ssid
				preferences.putString(PREFERENCE_WIFI_SSID, readString());
				sendOk();
				break;
			case 0x22: // set wifi password
				preferences.putString(PREFERENCE_WIFI_PSK, readString());
				sendOk();
				break;

			case 0x30: // set digital output
				offset = readUint16BE();
				for(int i = 0; i < DIGITAL_PIN_COUNT; i++)
					digitalWrite(digitalPins[i], (offset & (1 << i)) ? HIGH : LOW);
				sendOk();
				break;

			case 0xff: // reboot
				sendLine("rebooting...");
				ESP.restart();
				break;
		}
	}

	delay(5);
}
