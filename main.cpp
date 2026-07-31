#include <Arduino.h>

#define DBG(x) Serial.println(x)

class Display3Digit
{
public:
	Display3Digit(const byte (&seg)[7], const byte (&dig)[3], byte dp) : pinDp(dp)
	{
		for (byte i = 0; i < 7; i++)
			pinSegment[i] = seg[i];

		for (byte i = 0; i < 3; i++)
			pinDigit[i] = dig[i];
	}

	void init()
	{
		for (int i = 0; i < 7; i++)
		{
			pinMode(pinSegment[i], OUTPUT);
			digitalWrite(pinSegment[i], LOW);
		}

		pinMode(pinDp, OUTPUT);
		digitalWrite(pinDp, LOW);

		for (int i = 0; i < 3; i++)
		{
			pinMode(pinDigit[i], OUTPUT);
			digitalWrite(pinDigit[i], LOW); // gemeinsame Kathode aus
		}
	}
	void update()
	{
		if (millis() - lastRefresh < refreshTime)
			return;

		lastRefresh = millis();

		disableDigits();

		byte n = values[currentDigit];

		for (int i = 0; i < 7; i++)
			digitalWrite(pinSegment[i], font[n][i]);

		digitalWrite(pinDp, dpState[currentDigit]);

		// gemeinsame Kathode:
		digitalWrite(pinDigit[currentDigit], HIGH);

		currentDigit++;
		if (currentDigit >= 3)
			currentDigit = 0;
	}

	void show(int value)
	{
		if (value > 999)
			value = 999;

		if (value < -99)
			value = -99;

		if (value < 0)
		{
			value = -value;

			values[0] = 11; // '-'
			values[1] = value / 10;
			values[2] = value % 10;

			if (value < 10)
				values[1] = 10; // führende Stelle leer
		}
		else
		{
			values[0] = value / 100;
			values[1] = (value / 10) % 10;
			values[2] = value % 10;

			if (value < 100)
				values[0] = 10; // leer
			if (value < 10)
				values[1] = 10; // leer
		}
	}
	void clear()
	{
		values[0] = 10;
		values[1] = 10;
		values[2] = 10;

		dpState[0] = false;
		dpState[1] = false;
		dpState[2] = false;
	}

	void setDP(byte digit, bool on)
	{
		if (digit < 3)
			dpState[digit] = on;
	}

private:
	byte pinSegment[7];
	byte pinDigit[3];
	byte pinDp;

	byte currentDigit = 0;
	byte values[3] = {10, 10, 10}; // 10 = leer
	bool dpState[3] = {false, false, false};

	unsigned long lastRefresh = 0;
	const unsigned int refreshTime = 2; // ms

	// static const bool font[11][7];

	void disableDigits()
	{
		for (int i = 0; i < 3; i++)
			digitalWrite(pinDigit[i], LOW);
	}

	const bool font[12][7] =
		{
			// A B C D E F G
			{1, 1, 1, 1, 1, 1, 0}, // 0
			{0, 1, 1, 0, 0, 0, 0}, // 1
			{1, 1, 0, 1, 1, 0, 1}, // 2
			{1, 1, 1, 1, 0, 0, 1}, // 3
			{0, 1, 1, 0, 0, 1, 1}, // 4
			{1, 0, 1, 1, 0, 1, 1}, // 5
			{1, 0, 1, 1, 1, 1, 1}, // 6
			{1, 1, 1, 0, 0, 0, 0}, // 7
			{1, 1, 1, 1, 1, 1, 1}, // 8
			{1, 1, 1, 1, 0, 1, 1}, // 9
			{0, 0, 0, 0, 0, 0, 0}, // leer
			{0, 0, 0, 0, 0, 0, 1}  // -
	};
};

const byte pinSeg[7] = {21, 20, 19, 18, 17, 16, 15};
const byte pinDig[3] = {7, 8, 9};
const byte pinDp = 14;

Display3Digit display(pinSeg, pinDig, pinDp);

void setup()
{
	Serial.begin(9600);
	DBG("Serial online!");

	// int zahl = 123;

	// int h = zahl / 100;
	// int z = (zahl / 10) % 10;
	// int e = zahl % 10;

	// DBG(zahl);
	// DBG(h);
	// DBG(z);
	// DBG(e);

	display.init();
}

void loop()
{
	display.show(millis() / 1000);	// 

	display.update();
}
