/*
 *  Project     Xbox 360 Controller LEDs Library
 *  @author     David Madison
 *  @link       github.com/dmadison/Xbox360ControllerLEDs
 *  @license    MIT - Copyright (c) 2019 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  Example:      USB-Receive
 *  Description:  Simple example for driving the Xbox LED animations from USB
 *                XInput control packets
 *
 *                Requires the Arduino XInput library and a compatible XInput
 *                boards package:
 *                https://www.github.com/dmadison/ArduinoXInput
 *
 */

#include <X360ControllerLEDs.h>
#include <XInput.h>  // Arduino XInput library

// Pin Definitions
const uint8_t LED_Pin_1 = 2;
const uint8_t LED_Pin_2 = 3;
const uint8_t LED_Pin_3 = 4;
const uint8_t LED_Pin_4 = 5;
const boolean InvertOutput = false;   // LED output is normal (high = LED on)
                                      //   Set to "true" if sinking current.

// Controller LED Object
XboxControllerLEDs<LED_Pin_1, LED_Pin_2, LED_Pin_3, LED_Pin_4> leds(InvertOutput);  // Declare four-LED object

boolean newPattern = false;  // flag for received data

void setup() {
	leds.begin();  // Initialize pins

	XInput.setReceiveCallback(ledCallback);  // set callback for LED packets
	XInput.begin();
}

void loop() {
	if (newPattern == true) {  // if there's a new LED packet...
		newPattern = false;  // disable flag
		leds.linkPattern((XboxLEDPattern) XInput.getLEDPattern());   // Set pattern and use linked pattern features
	}
	
	leds.run();
}

void ledCallback(uint8_t message_type) {
	if (message_type == (uint8_t) XInputReceiveType::LEDs) {
		newPattern = true;  // new LED packet!
	}
}
