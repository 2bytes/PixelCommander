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

#include <SerialCommander.h>
#include <Adafruit_NeoPixel.h>

twobytes_SerialCommander cmdr = twobytes_SerialCommander(word('P','C'));

#define DATA_PIN	9
#define ZERO	        0

#define BRIGHT_UP	0x55
#define BRIGHT_DN	0x44
#define BRIGHT_I	0x49
#define BRIGHT_O	0x4F

#define PIXEL_COUNT     9

#define BRIGHTNESS_MAX	255
#define BRIGHTNESS_MIN	ZERO

int myBrightness = 255;
uint32_t myPixelColour = 0xFFFFFF;

boolean DEBUG = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
	pixels.begin();
	pixels.show();

	Serial.begin(115200);

        cmdr.addCommand("SB", setBrightnessCMD);
        cmdr.addCommand("SC", setColourCMD);
        cmdr.addCommand("BL", setBrightnessLevelCMD);
        cmdr.addCommand("ID", identifyCMD);
        cmdr.setDefaultHandler(unknownCommand);

	doInitCycle();

}

void loop()
{
  cmdr.readSerialLooper();
}

void doInitCycle()
{
	delay(100);

	setPixelColourImmediate(0xFF0000);

	delay(500);

	setPixelColourImmediate(0x00FF00);

	delay(500);

	setPixelColourImmediate(0x0000FF);

	delay(00);

	setPixelColourImmediate(0xFFFFFF);

	delay(200);

	fadeBrightness(ZERO, false);

	delay(200);

        fadeBrightness(0xFF, false);
        
        delay(100);
        
        
        // Red
        for(uint16_t j = 0; j < pixels.numPixels(); j++)
        {
           setSinglePixelColour(j, 0xFF0000);
           delay(200);
           setSinglePixelColour(j, 0xFFFFFF);
        }
        
        // Green
        for(uint16_t j = 0; j < pixels.numPixels(); j++)
        {
           setSinglePixelColour(j, 0x00FF00);
           delay(200);
           setSinglePixelColour(j, 0xFFFFFF);
        }
        
        // Blue
        for(uint16_t j = 0; j < pixels.numPixels(); j++)
        {
           setSinglePixelColour(j, 0x0000FF);
           delay(200);
           setSinglePixelColour(j, 0xFFFFFF);
        }
        
	Serial.println("Ready");
}

/*
-----------------------------------------------------------------------------------------

Serial Commander callbacks



-----------------------------------------------------------------------------------------
*/

void identifyCMD(char data[])
{
  if(DEBUG)
  {
    Serial.println("IDENTIFY");
  }
  identify();
}

void setBrightnessLevelCMD(char data[])
{ 
  int brightCode = data[2];

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

void setBrightnessCMD(char data[])
{
   char sBright[2] = {data[2], data[3]};
   Serial.print("Bright: ");Serial.println(sBright);
   byte bright = (byte)strtoul(sBright, NULL, 16);
   fadeBrightness(bright, false); 
}

void setColourCMD(char data[])
{ 
  char sRed[2] = {data[2], data[3]};
  Serial.print("R: ");Serial.println(sRed);
  byte red = (byte)strtoul(sRed, NULL, 16);
  
  char sGreen[2] = {data[4], data[5]};
  Serial.print("G: ");Serial.println(sGreen);
  byte green = (byte)strtoul(sGreen, NULL, 16);
  
  char sBlue[2] = {data[6], data[7]};
  Serial.print("B: ");Serial.println(sBlue);
  byte blue = (byte)strtoul(sBlue, NULL, 16);

  uint32_t pixelColour = ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
				
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
}

void unknownCommand(char data[])
{
  Serial.print("Unknown command, no handler defined: ");Serial.println(data);
}

/*
-----------------------------------------------------------------------------------------

Pixel Commander functions



-----------------------------------------------------------------------------------------
*/

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

void setSinglePixelColour(uint16_t pixel, uint32_t pixelColour)
{
    pixels.setPixelColor(pixel, pixelColour);
    pixels.show();
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
