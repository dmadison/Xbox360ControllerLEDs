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
 *  Example:      UserOutput
 *  Description:  Define a custom output class that uses Xbox controller LED
 *                animations. This example uses WS2812B LEDs driven by the
 *                FastLED library.
 */

#include <X360ControllerLEDs.h>
#include <FastLED.h>

// Sketch Options
const uint8_t NumLEDs = 4;  // Number of LEDs for the animations and output. Can be 4 or 1.
const uint8_t LedPin  = 6;  // Data pin for WS2812B LED strip
const uint8_t Brightness = 64;  // LED brightness (0 - 255)

// Custom Output Class
class MyCustomLEDClass 
	: public XboxControllerLEDs_Custom<NumLEDs> // Use 4-LED animations
{
public:
	/*  public void begin(void)
	 *  Required function to initialize the output system. Must be called
	 *  once by the user after MCU startup is complete. Takes no arguments.
	 *  
	 *  Can be empty if the output system requires no initialization.
	 */
	void begin() {
		FastLED.addLeds<WS2812B, LedPin, GRB>(addressableLEDs, NumLEDs);  // Initialize addressable output
		FastLED.setBrightness(Brightness);
		setPattern(XboxLEDPattern::Off);  // Set LEDs to "Off" pattern to start
	}

protected:
	/*  protected void setLEDs(uint8)
	 *  Required function to send the LED state to the LEDs. Called on every
	 *  frame update.
	 *  
	 *  uint8_t argument is a packed byte that contains the on/off states
	 *  for every LED in the current frame.
	 */ 
	void setLEDs(uint8_t ledStates) {
		for (uint8_t i = 0; i < NumLEDs; i++) {
			boolean thisLED = ledStates & (1 << i);  // Get extracted LED state

			if (thisLED == HIGH) {
				addressableLEDs[i] = ColorOn;
			}
			else {
				addressableLEDs[i] = ColorOff;
			}
		}
		FastLED.show();  // Send data to strip
	}

	const CRGB ColorOn  = CRGB::Green;
	const CRGB ColorOff = CRGB::Black;

	CRGB addressableLEDs[NumLEDs];  // FastLED - 4 LED array

};

MyCustomLEDClass leds;  // Create class instance

void setup() {
	leds.begin();  // Initialize output
	leds.setPattern(XboxLEDPattern::Rotating);  // Set the 'rotating' pattern
}

void loop() {
	leds.run();  // Evaluate the pattern and set the LEDs
}
