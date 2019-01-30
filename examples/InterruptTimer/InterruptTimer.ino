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
 *  Example:      InterruptTimer
 *  Description:  Blink using the library's LED animations without having to
 *                repeatedly call 'run' in loop().
 *                
 *                Only tested with Uno / Nano. Other boards will have different
 *                registers for their interrupt timers; read their datasheets
 *                to learn more. Requires use of TIMER2.
 */

#include <X360ControllerLEDs.h>

#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega168__)
#error Interrupt registers not defined! Please try setting board to either Uno or Nano.
#endif

const uint8_t LED_Pin = LED_BUILTIN;  // Use the built-in LED

XboxControllerLEDs<LED_Pin> leds;  // Declare single-LED object

void setup() {
	leds.begin();  // Initialize pins
	leds.setPattern(XboxLEDPattern::Blinking);  // Set the 'blinking' pattern

	setupTimer();  // Initialize TIMER2 interrupt
}

void loop() {
	// Empty!
}

/* Calculate clock value: 
 *     TimerGoal: value to trigger the timer, in ms
 *     CompareValue: comparison value for the TIMER to trigger at (0 - 255)
 *
 *         ((F_CPU / 1024) * TimerGoal) / 1000;
 *         ((Clock speed / prescaler) * goal value (ms)) / 1000 (ms to s)
 *
 * Will automatically adjust to the CPU clock until the 'goal' value is
 * unobtainable for the current prescaler value
 */

constexpr uint8_t  TimerGoal = 10;  // ms
constexpr uint16_t CompareValue = ((F_CPU / 1024) * TimerGoal) / 1000; 

void setupTimer() {
	static_assert(CompareValue <= 0xFF, "Timer is out of range! Lower the goal value");
	TCCR2B |= 0x07;       // Set TIMER2 prescaler to 1024
	OCR2A = CompareValue & 0xFF; // Set TIMER2 compare to fire at a count of 10 ms
	TIMSK2 |= (1 << 1);   // Enable TIMER2 Compare A interrupt
}

ISR(TIMER2_COMPA_vect) {
	leds.run();  // Execute LED animations
	TCNT2 = 0;  // Reset timer counter
}
