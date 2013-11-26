# Pixel Commander v1.0

Pixel Commander as a piece of Arduino code designed to allow control over a set of WS2812B RGB LEDs connected over UART.

## Requirements

* AdaFruit NeoPixel Library for Arduino [https://github.com/adafruit/Adafruit_NeoPixel]

## Commands (in hex)

'''
Set Colour: 		50 43 53 43 RR GG BB 00 00	(Where RR is red byte, GG is green byte and BB is blue byte)
Set Brightness: 	50 43 53 42 XX 00 00		(Where XX is the brightness byte from 00 to FF)
Brightness Level:	50 43 42 4C	YY 00 00		(Where YY is the Brightness Level Code)
'''

### Brightness Level Codes
'''
Brightness Up:		55
Brightness Down:	44
Brightness On:		49	(Max brightness)
Brightness Off:		4F	(Zero brightness, Off)
'''