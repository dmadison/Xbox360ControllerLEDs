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
 *  Example:      Blink
 *  Description:  Make the built-in LED blink using the Xbox 360 controller's
 *                LED flash timing.
 */

#include <X360ControllerLEDs.h>

const uint8_t LED_Pin = LED_BUILTIN;  // Use the built-in LED
const boolean InvertOutput = false;   // LED output is normal (high = LED on)
                                      //   Set to "true" if sinking current.

XboxControllerLEDs<LED_Pin> leds(InvertOutput);  // Declare single-LED object

void setup() {
	leds.begin();  // Initialize pins
	leds.setPattern(XboxLEDPattern::Blinking);  // Set the 'blinking' pattern
}

void loop() {
	leds.run();  // Evaluate the pattern and set the LEDs
}
