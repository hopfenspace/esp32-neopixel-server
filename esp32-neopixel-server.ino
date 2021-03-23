#include <stdint.h>
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
#define SECONDS_UNTIL_FALLBACK 10

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
const uint8_t ws2812Pins[] = {WS2812_PINS};
#define WS2812_PIN_COUNT (sizeof(ws2812Pins) / sizeof(*ws2812Pins))

const uint8_t digitalPins[] = {DIGITAL_PINS};
#define DIGITAL_PIN_COUNT (sizeof(digitalPins) / sizeof(*digitalPins))
_Static_assert(DIGITAL_PIN_COUNT <= 16, "Too many digital output pins! Maximum 16 allowed.");

Preferences preferences;
#define PREFERENCE_WIFI_MODE "wifi-mode" // 0 == AP, 1 == STA
#define PREFERENCE_WIFI_SSID "wifi-ssid"
#define PREFERENCE_WIFI_PSK "wifi-psk"
#define PREFERENCE_LED_COUNT "led-count"
#define PREFERENCE_FALLBACK_INDEX "fallback-index"
#define PREFERENCE_FALLBACK_ARGUMENT "fallback-arg"

typedef void (*fallbackAnimation)(uint8_t *arg, size_t argLen, bool firstTime);
fallbackAnimation selectedFallback = NULL;
uint8_t fallbackArgLen = 0;
uint8_t fallbackArgs[256];

void doNothingAnimation(uint8_t *, size_t, bool);
void constColorAnimation(uint8_t *, size_t, bool);
void rainbowAnimation(uint8_t *, size_t, bool);
void tailAnimation(uint8_t *, size_t, bool);
void sparcleAnimation(uint8_t *, size_t, bool);
void randomSparcleAnimation(uint8_t *, size_t, bool);
void clockAnimation(uint8_t *, size_t, bool);
void swapAnimation(uint8_t *, size_t, bool);
fallbackAnimation fallbackAnimations[] = {
	doNothingAnimation,
	constColorAnimation,
	rainbowAnimation,
	tailAnimation,
	sparcleAnimation,
	randomSparcleAnimation,
	clockAnimation,
	swapAnimation,
};
#define FALLBACK_ANIMATION_COUNT (sizeof(fallbackAnimations) / sizeof(fallbackAnimation))

void setup()
{
	preferences.begin("ws2812server", false);

	pixels.setPin(ws2812Pins[0]);

	uint16_t pixelCount = preferences.getUShort(PREFERENCE_LED_COUNT, WS2812_DEFAULT_COUNT);
	if (pixelCount != WS2812_DEFAULT_COUNT)
		pixels.updateLength(pixelCount);

	pixels.begin();

	pixels.clear();
	for (int i = 0; i < BOOTCOLORS_COUNT; i++)
		pixels.setPixelColor(i, bootColors[i]);
	pixels.show();

	for (int i = 0; i < DIGITAL_PIN_COUNT; i++)
	{
		pinMode(digitalPins[i], OUTPUT);
		digitalWrite(digitalPins[i], DIGITAL_DEFAULT_STATE);
	}

	uint8_t wifiMode = preferences.getUChar(PREFERENCE_WIFI_MODE, 0);
	if (wifiMode == 1)
	{
		String ssid = preferences.getString(PREFERENCE_WIFI_SSID, "DefaultWiFi");
		String password = preferences.getString(PREFERENCE_WIFI_PSK, "");
		const char *pw;
		if (password == "")
			pw = NULL;
		else
			pw = password.c_str();

		WiFi.begin(ssid.c_str(), pw);

		for (int i = 0; i < 100; i++)
		{
			if (WiFi.status() == WL_CONNECTED)
				break;

			for (int j = 0; j < BOOTCOLORS_COUNT; j++)
				pixels.setPixelColor(j, bootColors[(i + j) % BOOTCOLORS_COUNT]);
			pixels.show();

			delay(100);
		}
	}

	if (wifiMode == 0 || WiFi.status() != WL_CONNECTED)
	{
		uint32_t mac = ESP.getEfuseMac() >> 24;
		char apSsid[64];
		sprintf(apSsid, "NeopixelServer-%06X", mac);

		WiFi.mode(WIFI_AP);
		WiFi.softAP(apSsid, NULL);
	}

	server.begin(1337);

	uint8_t index = preferences.getUChar(PREFERENCE_FALLBACK_INDEX, 1);
	fallbackArgLen = preferences.getBytes(PREFERENCE_FALLBACK_ARGUMENT, fallbackArgs, 256);
	selectedFallback = fallbackAnimations[index];
	selectedFallback(fallbackArgs, fallbackArgLen, true);
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
	for (int i = 0; i < len; i++)
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
	static uint32_t lastPacket = 0;
	static bool inFallbackMode = true;

	if (inFallbackMode)
		selectedFallback(fallbackArgs, fallbackArgLen, false);

	uint32_t now = millis();
	if (!inFallbackMode && selectedFallback && now - lastPacket > SECONDS_UNTIL_FALLBACK * 1000)
	{
		inFallbackMode = true;
		selectedFallback(fallbackArgs, fallbackArgLen, true);
	}

	server.parsePacket();
	while (server.available() > 0)
	{
		lastPacket = now;
		inFallbackMode = false;

		uint8_t packetType = server.read();
		uint32_t color;
		uint16_t offset;
		uint16_t length;
		switch (packetType)
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
			for (uint16_t i = offset; i < offset + length; i++)
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
			if (offset < 0 || offset >= WS2812_PIN_COUNT)
			{
				sendLine("error: pin index out of range");
				break;
			}
			pixels.setPin(ws2812Pins[offset]);
			sendOk();
			break;
		case 0x12: // set fallback animation
			offset = server.read();
			length = server.read();
			if (offset >= 0 && offset < FALLBACK_ANIMATION_COUNT && length <= 256)
			{
				selectedFallback = fallbackAnimations[offset];
				fallbackArgLen = length;
				for (int i = 0; i < length; i++)
					fallbackArgs[i] = server.read();

				preferences.putUShort(PREFERENCE_FALLBACK_INDEX, offset);
				preferences.putBytes(PREFERENCE_FALLBACK_ARGUMENT, fallbackArgs, length);

				inFallbackMode = true;
				selectedFallback(fallbackArgs, fallbackArgLen, true);
				sendOk();
			}
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
			for (int i = 0; i < DIGITAL_PIN_COUNT; i++)
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

void doNothingAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
}
void constColorAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	if (firstTime)
	{
		uint32_t color = 0;
		if (argLen == 3)
			color = Adafruit_NeoPixel::Color(args[0], args[1], args[2]);

		pixels.fill(color, 0, pixels.numPixels());
		pixels.show();
	}
}
void colorWheel(int firstPixel, int lastPixel, int stripeSize)
{

	double colorsteps = 255 / (stripeSize / 3);
	for (int idx = 0; idx < stripeSize / 3; idx++)
	{
		pixels.setPixelColor(idx, 255 - idx * colorsteps, idx * colorsteps, 0);						 //area red to green
		pixels.setPixelColor(idx + stripeSize / 3, 0, 255 - idx * colorsteps, idx * colorsteps);	 //area  green to blue
		pixels.setPixelColor(idx + stripeSize * 2 / 3, idx * colorsteps, 0, 255 - idx * colorsteps); //area blue to red
	}
}
void rainbowAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	if (firstTime)
	{
		uint16_t length = pixels.numPixels();
		pixels.fill(0, 0, length - 1); //stripe reset
		colorWheel(0, length - 1, length);
	}
	else
	{
		// first pixel gets color of the last pixel
		pixels.setPixelColor(0, pixels.getPixelColor(pixels.numPixels() - 1));
		for (uint16_t idx = pixels.numPixels(); idx > 0; idx--)
		{
			// every pixel gets the color of its predecessor
			pixels.setPixelColor(idx, pixels.getPixelColor(idx - 1));
		}
	}

	pixels.show();
}

void tailAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	static int position = 0;
	if (argLen != 5)
		return;
	uint32_t color = Adafruit_NeoPixel::Color(args[0], args[1], args[2]);
	uint16_t size = (args[3] << 8) | args[4];

	if (position >= pixels.numPixels())
	{ // reset at end of stripe
		firstTime = true;
		position = 0;
	}
	if (firstTime)
	{
		pixels.fill(0, 0, pixels.numPixels() - 1); //stripe reset
		for (int i = 0; i < size; i++)
		{ //initializing the stripe
			pixels.setPixelColor(i, color);
		}
	}
	else
	{
		pixels.setPixelColor(position, 0, 0, 0);	  //switchoff first led of the tail
		pixels.setPixelColor(position + size, color); // switch on led behind tail
		position++;
	}
	pixels.show();
}

void sparcleAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	if (argLen != 3)
		return;
	uint32_t color = Adafruit_NeoPixel::Color(args[0], args[1], args[2]);

	int on = random(0, pixels.numPixels());
	int off = random(0, pixels.numPixels());
	pixels.setPixelColor(on, color); // defined color random position
	pixels.setPixelColor(off, 0);
	pixels.show();
}

void randomSparcleAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	int on = random(0, pixels.numPixels());
	int red = random(0, 256);
	int blue = random(0, 256);
	int green = random(0, 256);
	pixels.setPixelColor(on, red, blue, green); // random color + random position
	int off = random(0, pixels.numPixels());
	pixels.setPixelColor(off, 0);
	pixels.show();
}

void clockAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	static int counter = 0;
	static int position = 0;
	if (firstTime)
	{
		position = pixels.numPixels() - 1;
	}
	if (counter >= pixels.numPixels())
	{
		counter = 0;
		pixels.fill(0, 0, pixels.numPixels() - 1);
	}
	if (argLen == 3)
	{
		uint32_t color = Adafruit_NeoPixel::Color(args[0], args[1], args[2]);
		if (position >= counter)
		{
			pixels.setPixelColor(position, color);
			pixels.setPixelColor(position + 1, 0);
			position--;
		}
		else
		{
			pixels.setPixelColor(counter, color);
			counter++;
			position = pixels.numPixels() - 1;
		}
	}
	pixels.show();
}
void swapAnimation(uint8_t *args, size_t argLen, bool firstTime)
{
	if (argLen == 4)
	{
		uint32_t color = Adafruit_NeoPixel::Color(args[0], args[1], args[2]);
		int middle = (pixels.numPixels() - 1) / 2;						   //middle of stripe
		static int counter = 0;											   // counts function calling
		uint8_t wave = args[3];											   // amount of "waves until reset"
		static int milestones = 1;										   // how many resetwaves were performed
		int border = (((pixels.numPixels() - 1) / 2) / wave) * milestones; // border for next blinkiblinki
		if (firstTime)
		{
			counter = 0;
			milestones = 1;
		}
		if (counter >= border) // reset counter when it hits the border and increases the bordersize
		{
			pixels.fill(color, 0, pixels.numPixels() - 1); // reset stripe
			counter = 0;
			milestones++;
		}
		if (milestones > wave) // reset the milestones when they are more than the waves
		{
			milestones = 1;
		}
		pixels.setPixelColor(middle + counter, color); // colourrizes the pixels left and right from middle
		pixels.setPixelColor(middle - counter, color);
		counter++;
		pixels.show();
	}
}