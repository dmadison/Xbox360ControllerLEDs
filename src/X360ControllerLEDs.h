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
 */

#ifndef X360ControllerLEDs_h
#define X360ControllerLEDs_h

#include <Arduino.h>

namespace Xbox360Controller_LEDs {

	enum class LED_Pattern : uint8_t {
		// Driver defined options
		Off = 0x00,
		Blinking = 0x01,
		Flash1 = 0x02,
		Flash2 = 0x03,
		Flash3 = 0x04,
		Flash4 = 0x05,
		Player1 = 0x06,
		Player2 = 0x07,
		Player3 = 0x08,
		Player4 = 0x09,
		Rotating = 0x0A,
		BlinkOnce = 0x0B,
		BlinkSlow = 0x0C,
		Alternating = 0x0D,

		// Meta options
		Previous,  // After this, go back to previous pattern
		Null,      // Not a pattern
	};

	struct LED_Frame {
		static const unsigned long Timescale = 10;  // x ms = 1 tick

		constexpr LED_Frame(const uint8_t LED_Pack, const unsigned long length) :
			LEDs(LED_Pack),
			Duration(length / Timescale)
		{}

		const uint8_t LEDs;
		const uint8_t Duration;
	};

	class AnimationBase {
	public:
		constexpr AnimationBase(unsigned long t, LED_Pattern nxt)
			: Duration(t),
			Next(nxt)
		{}

		virtual LED_Frame getFrame(size_t n) const = 0;
		virtual size_t getNumFrames() const = 0;

		const unsigned long Duration;  // How long to run this pattern, ms
		const LED_Pattern Next;  // Next pattern to run
	};

	template<size_t nframes>
	class LED_Animation : public AnimationBase {
	public:
		constexpr LED_Animation(LED_Frame const (&Array)[nframes],
			unsigned long t = 0, LED_Pattern nxt = LED_Pattern::Null)
			: AnimationBase(t, nxt), Frames(Array)
		{}

		using AnimationBase::getFrame;
		using AnimationBase::getNumFrames;

		LED_Frame getFrame(size_t n) const {
			return Frames[n];
		}

		size_t getNumFrames() const {
			return nframes;
		}

	private:
		const LED_Frame Frames[nframes];
	};

	template <size_t nleds>
	class XboxLEDAnimations {
	public:
		constexpr XboxLEDAnimations() {
			static_assert(nleds == 4 || nleds == 1,
				"Error: Must set either 1 or 4 LED pins");
		}
		// Intentionally empty
	};

	template <>
	class XboxLEDAnimations<1> {
	public:
		static const LED_Animation<1> Anim_Off;
		static const LED_Animation<2> Anim_Blinking;

		static const LED_Animation<2> Anim_Flash1;
		static const LED_Animation<2> Anim_Flash2;
		static const LED_Animation<2> Anim_Flash3;
		static const LED_Animation<2> Anim_Flash4;

		static const LED_Animation<1> Anim_Player1;
		static const LED_Animation<4> Anim_Player2;
		static const LED_Animation<6> Anim_Player3;
		static const LED_Animation<8> Anim_Player4;

		static const AnimationBase & getAnimation(LED_Pattern pattern);

	protected:
		static constexpr uint32_t BlinkTime = 450;
		static constexpr uint32_t FlashTime = 80;

		static constexpr uint32_t PlayerFlashTime = 1000;
		static constexpr uint32_t PlayerTime = 200;
		static constexpr uint32_t PlayerLoopTime = 1500;

		static constexpr uint8_t On  = 1;
		static constexpr uint8_t Off = 0;
	};

	template <>
	class XboxLEDAnimations<4> {
	public:
		static const LED_Animation<1> Anim_Off;
		static const LED_Animation<2> Anim_Blinking;
		static const LED_Animation<2> Anim_BlinkOnce;
		static const LED_Animation<2> Anim_BlinkSlow;

		static const LED_Animation<2> Anim_Flash1;
		static const LED_Animation<2> Anim_Flash2;
		static const LED_Animation<2> Anim_Flash3;
		static const LED_Animation<2> Anim_Flash4;

		static const LED_Animation<1> Anim_Player1;
		static const LED_Animation<1> Anim_Player2;
		static const LED_Animation<1> Anim_Player3;
		static const LED_Animation<1> Anim_Player4;

		static const LED_Animation<4> Anim_Rotating;
		static const LED_Animation<2> Anim_Alternating;

		static const AnimationBase & getAnimation(LED_Pattern pattern);

	protected:
		static constexpr uint32_t BlinkTime = 300;
		static constexpr uint32_t BlinkSlow = 700;
		static constexpr uint32_t RotateTime = 100;

		static constexpr uint32_t PlayerBlinkCount = 3;

		static constexpr uint8_t States_Off = 0b0000;
		static constexpr uint8_t States_On  = 0b1111;

		static constexpr uint8_t States_Player1 = (1 << 0);
		static constexpr uint8_t States_Player2 = (1 << 1);
		static constexpr uint8_t States_Player3 = (1 << 2);
		static constexpr uint8_t States_Player4 = (1 << 3);

		static constexpr uint8_t States_Op1 = 0b1010;
		static constexpr uint8_t States_Op2 = 0b0101;
	};

	class XboxLEDBase {
	public:
		static const uint8_t NumPatterns = 14;

		virtual void begin() = 0;
		virtual void setPattern(LED_Pattern) = 0;
		virtual void receivePattern(LED_Pattern) = 0;
		virtual void run() = 0;
	};

	template <uint8_t ...pins>
	class XboxLEDHandler : public XboxLEDBase {
	public:
		static const size_t NumLEDs = sizeof... (pins);  // # of pins = # of LEDs
		using Animation = AnimationBase;

		XboxLEDHandler(const bool inv = false) :
			Pins{ pins... },
			Inverted(inv),
			currentPattern(LED_Pattern::Off),
			previousPattern(currentPattern),
			currentAnimation(&XboxLEDAnimations<NumLEDs>::getAnimation(currentPattern))
		{}

		void begin() {  // Initialize LED outputs
			for (uint8_t i = 0; i < NumLEDs; i++) {
				pinMode(Pins[i], OUTPUT);
				digitalWrite(Pins[i], Inverted);  // Set 'Off'
			}
			runFrame();  // Set LEDs
		}

		void setPattern(LED_Pattern pattern) {
			if ((uint8_t)pattern >= XboxLEDBase::NumPatterns) return;  // Meta pattern, ignore
			linkPatterns = false;  // Don't auto-link to the next pattern
			setPattern(pattern, true);  // Screw it, do the pattern NOW
		}

		void receivePattern(LED_Pattern pattern) {
			if ((uint8_t)pattern >= XboxLEDBase::NumPatterns) return;  // Meta pattern, ignore
			linkPatterns = true;  // Auto-link to the next pattern if available
			setPattern(pattern, false);  // Set pattern, but don't run immediately if it is next pattern in queue
		}

		LED_Pattern getPattern() const {
			return currentPattern;  // Current pattern as enum
		}

		uint8_t getPlayerNumber() const {
			switch (currentPattern) {
			case(LED_Pattern::Flash1):
			case(LED_Pattern::Player1):
				return 1;
			case(LED_Pattern::Flash2):
			case(LED_Pattern::Player2):
				return 2;
			case(LED_Pattern::Flash3):
			case(LED_Pattern::Player3):
				return 3;
			case(LED_Pattern::Flash4):
			case(LED_Pattern::Player4):
				return 4;
			default: return 0;
			}
		}

		void run() {
			if (currentAnimation->getNumFrames() <= 1 || currentAnimation->getFrame(frameIndex).Duration == 0) return;  // No processing necessary
			if (millis() - time_frameLast < currentAnimation->getFrame(frameIndex).Duration * LED_Frame::Timescale) return;  // Not time yet

			// Check if it's time to switch to the next pattern
			if (linkPatterns && currentAnimation->Duration != 0 && millis() - time_animationLast >= currentAnimation->Duration) {
				setPattern(currentAnimation->Next, true);  // Run next pattern + override "Next" check
				return;
			}

			time_frameLast = millis();  // Save time
			frameIndex++;  // Go to next frame
			if (frameIndex >= currentAnimation->getNumFrames()) frameIndex = 0;  // If at last frame, go to start
			runFrame();  // Write current frame to LEDs
		}

	private:
		void setPattern(LED_Pattern pattern, boolean runNow) {
			if (currentPattern == pattern) return;  // No change
			if (runNow == false && pattern == currentAnimation->Next) return;  // That's the next pattern! We'll get there...

			// If pattern says go back, load prevous pattern
			if (pattern == LED_Pattern::Previous) {
				pattern = previousPattern;
			}

			// If not currently running a temporary pattern, save as previous
			if (currentAnimation->Next != LED_Pattern::Previous) {
				previousPattern = currentPattern;  // Save current pattern as previous
			}

			currentPattern = pattern;  // Save pattern (enum)

			const Animation * newAnimation = &XboxLEDAnimations<NumLEDs>::getAnimation(currentPattern);
			if (currentAnimation == newAnimation) return;  // Different pattern, same animation
			currentAnimation = newAnimation;  // Save animation (pointer)

			frameIndex = 0;
			time_animationLast = time_frameLast = millis();  // Now
			runFrame();  // Run once
		}

		void runFrame() {
			parseFrame(currentAnimation, frameIndex);
			setLEDs();
		}

		void parseFrame(const Animation * animation, uint32_t index) {
			if (index >= animation->getNumFrames()) return;  // Out of range

			for (uint8_t i = 0; i < NumLEDs; i++) {
				state[i] = animation->getFrame(index).LEDs & (1 << i);
			}
		}

		void setLEDs() {
			for (uint8_t i = 0; i < NumLEDs; i++) {
				digitalWrite(Pins[i], state[i] ^ Inverted);
			}
		}

		// LED Information
		const uint8_t Pins[NumLEDs];
		const boolean Inverted = false;
		boolean state[NumLEDs];
		boolean linkPatterns = false;

		// Current Pattern Information
		LED_Pattern currentPattern;
		LED_Pattern previousPattern;
		const Animation * currentAnimation;
		uint8_t frameIndex = 0;
		unsigned long time_frameLast = 0;
		unsigned long time_animationLast = 0;
	};

}  // End namespace

// Library API
using XboxLEDPattern = Xbox360Controller_LEDs::LED_Pattern;

template<uint8_t ...pins>
using XboxControllerLEDs = Xbox360Controller_LEDs::XboxLEDHandler<pins...>;

#endif
