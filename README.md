# Pixel Commander v1.0

Pixel Commander as a piece of Arduino code designed to allow control over a set of WS2812B RGB LEDs connected over UART.

## Requirements

* AdaFruit NeoPixel Library for Arduino [https://github.com/adafruit/Adafruit_NeoPixel]

## Commands (in hex)

Command    		| Hex Code		 			 | Notes
--------------- | -------------------------- | ------------------------------------------------------------
Set Colour 		| 50 43 53 43 RR GG BB 00 00 | RR = Red byte, GG = green byte, BB = blue byte
Set Brightness  | 50 43 53 42 XX 00 00		 | XX = brightness byte, from 00 to FF
Bright Level    | 50 43 42 4C YY 00 00		 | YY = Brightness code; Up = 55, Down = 44, Max = 49, Min = 4F

