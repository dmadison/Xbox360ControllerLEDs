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
		static const unsigned long Timescale = 100;  // x ms = 1 tick
		
		constexpr AnimationBase(unsigned long t, LED_Pattern nxt)
			: Duration(t / Timescale),
			Next(nxt)
		{}

		virtual LED_Frame getFrame(size_t n) const = 0;
		virtual size_t getNumFrames() const = 0;

		const uint8_t Duration;  // How long to run this pattern, # of ticks
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

		static constexpr uint8_t PlayerBlinkCount = 3;

		static constexpr uint8_t States_Off = 0b0000;
		static constexpr uint8_t States_On  = 0b1111;

		static constexpr uint8_t States_Player1 = (1 << 0);
		static constexpr uint8_t States_Player2 = (1 << 1);
		static constexpr uint8_t States_Player3 = (1 << 2);
		static constexpr uint8_t States_Player4 = (1 << 3);

		static constexpr uint8_t States_Op1 = 0b1010;
		static constexpr uint8_t States_Op2 = 0b0101;
	};

	class XboxLEDHandler {
	public:
		static const uint8_t NumPatterns = 14;
		using Animation = AnimationBase;

		XboxLEDHandler();

		virtual void begin() = 0;

		void setPattern(LED_Pattern pattern);
		void receivePattern(LED_Pattern pattern);

		void run();

		LED_Pattern getPattern() const;
		uint8_t getPlayerNumber() const;

	protected:
		void setPattern(LED_Pattern pattern, boolean runNow);

		virtual const Animation & getAnimation() const = 0;
		virtual void runFrame() = 0;

		// LED Information
		boolean linkPatterns = false;

		// Pattern Information (Enum)
		LED_Pattern currentPattern;
		LED_Pattern previousPattern;

		// Animation Information
		const Animation * currentAnimation;
		uint8_t frameIndex = 0;

		// Timestamps (ms)
		unsigned long time_frameLast = 0;
		unsigned long time_frameDuration = 0;
		unsigned long time_animationLast = 0;
		unsigned long time_animationDuration = 0;
	};

	template <uint8_t ...pins>
	class XboxLED_IndividualPins : public XboxLEDHandler {
	public:
		static const size_t NumLEDs = sizeof... (pins);  // # of pins = # of LEDs

		XboxLED_IndividualPins(const bool inv = false) :
			XboxLEDHandler(),
			Pins{ pins... },
			Inverted(inv)
		{
			currentAnimation = &getAnimation();
		}

		void begin() {  // Initialize LED outputs
			for (uint8_t i = 0; i < NumLEDs; i++) {
				pinMode(Pins[i], OUTPUT);
				digitalWrite(Pins[i], Inverted);  // Set 'Off'
			}
			runFrame();  // Set LEDs
		}

		constexpr uint8_t getNumLEDs() const {
			return NumLEDs;
		}

	protected:
		const Animation & getAnimation() const {
			return XboxLEDAnimations<NumLEDs>::getAnimation(currentPattern);
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
			time_frameDuration = animation->getFrame(frameIndex).Duration * LED_Frame::Timescale;  // Save current frame duration as ms
		}

		void setLEDs() {
			for (uint8_t i = 0; i < NumLEDs; i++) {
				digitalWrite(Pins[i], state[i] ^ Inverted);
			}
		}

		// LED Information
		const uint8_t Pins[NumLEDs];
		const boolean Inverted = false;  // Flag for inverted output
		boolean state[NumLEDs];
	};

}  // End namespace

// Library API
using XboxLEDPattern = Xbox360Controller_LEDs::LED_Pattern;

template<uint8_t ...pins>
using XboxControllerLEDs = Xbox360Controller_LEDs::XboxLED_IndividualPins<pins...>;

#endif
