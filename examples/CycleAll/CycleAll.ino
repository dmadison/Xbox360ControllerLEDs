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
 *  Example:      CycleAll
 *  Description:  Loop through all 14 available Xbox 360 controller LED
 *                animations using 4 LEDs:
 *                    0.  Off
 *                    1.  Blinking
 *                    2.  Flash1
 *                    3.  Flash2
 *                    4.  Flash3
 *                    5.  Flash4
 *                    6.  Player1
 *                    7.  Player2
 *                    8.  Player3
 *                    9.  Player4
 *                    10. Rotating
 *                    11. BlinkOnce
 *                    12. BlinkSlow
 *                    13. Alternating
 */

#include <X360ControllerLEDs.h>

// Define the pins for each LED, in order
const uint8_t Pin1 = 2;  // LED 1
const uint8_t Pin2 = 3;  // LED 2
const uint8_t Pin3 = 4;  // LED 3
const uint8_t Pin4 = 5;  // LED 4

XboxControllerLEDs<Pin1, Pin2, Pin3, Pin4> leds;  // Declare four-LED object

const unsigned long timePer = 3000;  // ms, time for each pattern
uint8_t patternNumber = 1;  // Current pattern ID
unsigned long lastChange;   // Timestamp of the last pattern update

void setup() {
	leds.begin();  // Initialize pins
	leds.setPattern((XboxLEDPattern) patternNumber);  // Set initial pattern
	lastChange = millis();  // Save current time
}

void loop() {
	// Check if it's time to change patterns
	if (millis() - lastChange >= timePer) {
		patternNumber++;  // Go to next pattern

		if (patternNumber >= leds.NumPatterns) {
			patternNumber = 0;  // Go back to start
		}
		lastChange = millis();  // Save current timestamp
		leds.setPattern((XboxLEDPattern) patternNumber);  // Set pattern
	}

	leds.run();  // Evaluate the pattern and set the LEDs
}
