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
 *                NOTE: This is NOT a custom USB mode and will not enable the
 *                      Arduino to function as an Xbox controller.
 */

#include <X360ControllerLEDs.h>

constexpr uint8_t LED_Pins[4] = { 2, 3, 4, 5 };  // LED pin numbers

XboxControllerLEDs<LED_Pins[0], LED_Pins[1], LED_Pins[2], LED_Pins[3]> leds;  // Declare four-LED object

volatile XboxLEDPattern pattern = XboxLEDPattern::Off;
XboxLEDPattern lastPattern = XboxLEDPattern::Off;
uint8_t playerNumber = 0;

void setup() {
	leds.begin();  // Initialize pins
}

void loop() {
	// Check if new pattern received
	if (pattern != lastPattern) {
		lastPattern = pattern;  // Save pattern for reference
		leds.receivePattern(lastPattern);  // Set pattern and use linked pattern features
		playerNumber = leds.getPlayerNumber();  // Update player number from LED pattern
	}
	
	leds.run();
}

// USB ISR Pseudocode
/*
 * ISR(USB_GEN_vect) {
 *     pattern = xinput.getPattern();  // Set pattern from USB packet
 * }
 */

