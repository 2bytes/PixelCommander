# Pixel Commander v2.0

Pixel Commander as a piece of Arduino code designed to allow control over a set of WS2812B RGB LEDs connected over UART.

## Requirements

* AdaFruit NeoPixel Library for Arduino [https://github.com/adafruit/Adafruit_NeoPixel]
* 2bytes SerialCommander v1.0 Library for Arduino [https://github.com/2bytes/SerialCommmander/tree/v1.0]

## Commands (spaces only for readability)

Command    	 	   | Command String	 			    | Notes
------------------ | ------------------------------ | ------------------------------------------------------------:
Set Colour 		   | PCSC RR GG BB [ENTER]          | RR = Red, GG = Green, BB = Blue
Set Brightness     | PCSB XX [ENTER]    		    | XX = brightness byte, from 00 to FF
Bright Level       | PCBL YY(Y) [ENTER]    		    | YY = code; "UP", "DN", "ON", "OFF"
Identify           | PCID [ENTER]                   | Pulses the LEDs white, twice, at full brightness.
