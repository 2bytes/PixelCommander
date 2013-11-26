/*
* Copyright (C) 2013 Hamid Elaosta
* Serial command controlled RGB pixel controller for Arduino [http://2byt.es]
*
* This file is part of Pixel Commander.
*
* Pixel Commander is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
*/


#include <Adafruit_NeoPixel.h>

#define DATA_PIN	9
#define ZERO	        0

#define BRIGHT_UP	0x55
#define BRIGHT_DN	0x44
#define BRIGHT_I	0x49
#define BRIGHT_O	0x4F

#define PIXEL_COUNT     9

#define BRIGHTNESS_MAX	255
#define BRIGHTNESS_MIN	ZERO

// Command characters
#define B	0x42
#define C	0x43
#define D	0x44
#define I	0x49
#define L	0x4C
#define P	0x50
#define S	0x53

int myBrightness = 255;
uint32_t myPixelColour = 0xFFFFFF;

boolean DEBUG = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
	pixels.begin();
	pixels.show();

	Serial.begin(115200);

	doInitCycle();

}

void loop()
{
	// Input buffer
	byte	ib[64];
	// Input count
	int		ic = 0;

	while(1)
	{
		if(Serial.peek() != -1)
		{
			ib[ic] = Serial.read();
			// Pixel Commander - Set Colour
			// 50 43 53 43 RR GG BB 00 00
			if(ib[ic-8] == P && ib[ic-7] == C && ib[ic-6] == S && ib[ic-5] == C && ib[ic-1] == ZERO && ib[ic] == ZERO)
			{
				uint32_t red = ib[ic-4];
				uint32_t green = ib[ic-3]; 
				uint32_t blue = ib[ic-2];

				uint32_t pixelColour = (red << 16) + (green << 8) + blue;
				
				if(DEBUG)
				{
					Serial.print("PixelColourConcat: ");Serial.println(pixelColour,DEC);

					Serial.println("");
					Serial.println("Setting Colour");
					Serial.print("Red: ");Serial.println(red,HEX);
					Serial.print("Green: ");Serial.println(green, HEX);
					Serial.print("Blue: ");Serial.println(blue, HEX);
				}


				setPixelColour(pixelColour);
				break;
			}
			else
			// Pixel Commander - Set Brightness
			// 50 43 53 42 XX 00 00
			if(ib[ic-6] == P && ib[ic-5] == C && ib[ic-4] == S && ib[ic-3] == B && ib[ic-1] == ZERO && ib[ic] == ZERO)
			{
				int newBright = ib[ic-2];
				
				fadeBrightness(newBright, false);
			}
			else
			// Pixel Commander - Brightness Level: UP, DOWN, ON (MAX), OFF (MIN)
			// 50 43 42 4C XX 00 00
			if(ib[ic-6] == P && ib[ic-5] == C && ib[ic-4] == B && ib[ic-3] == L && ib[ic-1] == ZERO && ib[ic] == ZERO)
			{
				int brightCode = ib[ic-2];

				switch (brightCode)
				{
				case BRIGHT_UP:
					if(DEBUG)
					{
						Serial.println("BRIGHT_UP");
					}
					increaseBrightness();
					break;
				case BRIGHT_DN:
					if(DEBUG)
					{
						Serial.println("BRIGHT_DN");
					}
					decreateBrightness();
					break;
				case BRIGHT_I:
					if(DEBUG)
					{
						Serial.println("BRIGHT_I");
					}
					setBrightnessImmediate(BRIGHTNESS_MAX);
					break;
				case BRIGHT_O:
					if(DEBUG)
					{
						Serial.println("BRIGHT_O");
					}
					setBrightnessImmediate(BRIGHTNESS_MIN);
					break;

				}
			}
			else
			// Pixel Commander - Identify
			// 50 43 49 44 00 00
			if(ib[ic-5] == P && ib[ic-4] == C && ib[ic-3] == I && ib[ic-2] == D && ib[ic-1] == ZERO && ib[ic] == ZERO)
			{
				if(DEBUG)
				{
					Serial.println("IDENTIFY");
				}
				identify();
			}

			ic++;
		}
	}

	delay(20);

}

void doInitCycle()
{
	delay(100);

	setPixelColourImmediate(0xFF0000);

	delay(100);

	setPixelColourImmediate(0x00FF00);

	delay(100);

	setPixelColourImmediate(0x0000FF);

	delay(100);

	setPixelColourImmediate(0xFFFFFF);

	delay(100);

	fadeBrightness(ZERO, false);

	delay(100);

	Serial.println("Ready");
}

void setPixelColour(uint32_t pixelColour)
{
	// Fade out
	//uint32_t tempCol = pixels.getPixelColor(0);
	int startBright = myBrightness;
	for(int i = startBright; i >= 0; i--)
	{
		if(DEBUG)
		{
			Serial.println(i,DEC);
		}

                setBrightnessImmediate(i);
		myBrightness = i;
		delay(5);
	}

        setPixelColourImmediate(pixelColour);
				
	// Fade in
	for(int i = 0; i <= startBright; i++)
	{
		if(DEBUG)
		{
			Serial.println(i,DEC);
		}

		setBrightnessImmediate(i);
		myBrightness = i;

		delay(5);
	}

}

void fadeBrightness(int newBright, boolean fast)
{
	int startBright = myBrightness;

	if(DEBUG)
	{
		Serial.print("New Brightness: ");Serial.println(newBright,DEC);
	}

	// Raise brightness
	if(newBright > startBright)
	{
		if(DEBUG)
		{
			Serial.println("Raising Brightness");
		}

		for(int i = startBright; i <= newBright; i++)
		{
			if(DEBUG)
			{
				Serial.println(i,DEC);
			}

			setBrightnessImmediate(i);
			
			if(!fast)
			{
				delay(5);				
			}
		}

	}
	else // Lower brightness
	if(newBright < startBright)
	{
		if(DEBUG)
		{
			Serial.println("Lowering Brightness");
		}

		for(int i = startBright; i >= newBright; i--)
		{
			if(DEBUG)
			{
				Serial.println(i,DEC);
			}

			setBrightnessImmediate(i);
			if(!fast)
			{
				delay(5);
			}
		}

	}
	else
	{
		if(DEBUG)
		{
			Serial.print("Resetting Brightness: ");Serial.println(newBright,DEC);
			Serial.print("Resetting Pixel Colour: ");Serial.println(myPixelColour,DEC);
		}

		setBrightnessImmediate(newBright);
	}
}

void setBrightnessImmediate(int brightness)
{
	pixels.setBrightness(brightness);

	for(uint16_t j = 0; j < pixels.numPixels(); j++)
	{
		pixels.setPixelColor(j, myPixelColour);
	}

	pixels.show();
	myBrightness = brightness;
}

void setPixelColourImmediate(uint32_t pixelColour)
{
	for(uint16_t j = 0; j < pixels.numPixels(); j++)
	{
		pixels.setPixelColor(j, pixelColour);
	}

	pixels.show();
	myPixelColour = pixelColour;
}

void increaseBrightness()
{
	if(myBrightness <= 240)
	{
		int newBright = myBrightness + 15;
		setBrightnessImmediate(newBright);
	}
}

void decreateBrightness()
{
	if(myBrightness >= 15)
	{
		int newBright = myBrightness - 15;
		setBrightnessImmediate(newBright);
	}
}

void identify()
{
	uint32_t tempCol = myPixelColour;
	int tempBright = myBrightness;

	setBrightnessImmediate(BRIGHTNESS_MIN);

	setPixelColourImmediate(0xFFFFFF);

	fadeBrightness(BRIGHTNESS_MAX, true);
	delay(300);
	fadeBrightness(BRIGHTNESS_MIN, true);
	delay(300);
	fadeBrightness(BRIGHTNESS_MAX, true);
	delay(300);
	fadeBrightness(BRIGHTNESS_MIN, true);
	delay(300);

	setPixelColour(tempCol);
	setBrightnessImmediate(tempBright);

}
