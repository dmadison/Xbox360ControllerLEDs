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

	/* 
	 * LED patterns are enumerated based on the pattern index sent 
	 * by the host driver over USB.
	 *
	 * See https://www.partsnotincluded.com/reverse-engineering/xbox-360-controller-led-animations/
	 */

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
		Max = Alternating,  // # of patterns, indexed at 0
		NumPatterns = Max + 1  // # of patterns, indexed at 1
	};

	// --------------------------------------------------------
	// Animation Base Classes                                 |
	// --------------------------------------------------------

	struct LED_Frame {
		static const unsigned long Timescale = 10;  // x ms = 1 tick

		constexpr LED_Frame(const uint8_t LED_Pack, const unsigned long length) :
			LEDs(LED_Pack),
			Duration(length / Timescale)  // Converts ms to # of ticks
		{}

		const uint8_t LEDs;  // Packed byte, 8 LED states max
		const uint8_t Duration;  // In ticks (see timescale above)
	};

	class AnimationBase {
	public:
		constexpr AnimationBase(uint8_t n, LED_Pattern nxt)
			: NCycles(n), Next(nxt)
		{}

		virtual LED_Frame getFrame(size_t n) const = 0;
		virtual size_t getNumFrames() const = 0;

		const uint8_t NCycles;   // How long to run this pattern, # of cycles
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

	// --------------------------------------------------------
	// Animation Lists                                        |
	//     All of the animations for a given # of LEDs        |
	// --------------------------------------------------------

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
		static constexpr uint32_t FlashTime = 100;


		static constexpr uint32_t PlayerTime = 100;
		static constexpr uint32_t PlayerLoopTime = 1500;
		static constexpr uint8_t  PlayerFlashCount = 10;

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

	// --------------------------------------------------------
	// LED Animation Handler                                  |
	//     Handles frame parsing and animation timing         |
	// --------------------------------------------------------

	class XboxLEDHandler {
	public:
		static const uint8_t NumPatterns = (uint8_t) LED_Pattern::NumPatterns;
		using Animation = AnimationBase;

		XboxLEDHandler();

		virtual void begin() = 0;

		void setPattern(LED_Pattern pattern);
		void linkPattern(LED_Pattern pattern);

		LED_Pattern getPattern() const;

		uint8_t getLastFrame() const;
		void rewriteFrame();
		
		void pauseOutput();
		void resumeOutput();

		void run();

	protected:
		virtual const Animation & getAnimation(LED_Pattern pattern) const = 0;
		virtual void setLEDs(uint8_t ledStates) = 0;

	private:
		void setPattern(LED_Pattern pattern, boolean runNow);
		void runFrame();

		// LED Information
		boolean linkPatterns = false;
		boolean writeOutput = true;
		uint8_t lastLEDFrame = 0x00;  // Bitmap of last LED states

		// Pattern Information (Enum)
		LED_Pattern currentPattern;
		LED_Pattern previousPattern;

		// Animation Information
		const Animation * currentAnimation;
		uint8_t frameIndex = 0;
		uint8_t cycleCount = 0;

		// Timestamps (ms)
		unsigned long time_frameLast = 0;
		unsigned long time_frameDuration = 0;
	};

	// --------------------------------------------------------
	// LED Animation Output                                   |
	//     For a given animation handler, links the animation |
	//     output states to hardware                          |
	// --------------------------------------------------------

	template <uint8_t ...pins>
	class XboxLED_IndividualPins : public XboxLEDHandler {
	public:
		static const size_t NumLEDs = sizeof... (pins);  // # of pins = # of LEDs

		XboxLED_IndividualPins(const bool inv = false) :
			XboxLEDHandler(),
			Pins{ pins... },
			Inverted(inv)
		{}

		void begin() {  // Initialize LED outputs
			for (uint8_t i = 0; i < NumLEDs; i++) {
				pinMode(Pins[i], OUTPUT);
				digitalWrite(Pins[i], Inverted);  // Set 'off' immediately
			}
			setPattern(LED_Pattern::Off);
		}

		constexpr uint8_t getNumLEDs() const {
			return NumLEDs;
		}

	protected:
		const Animation & getAnimation(LED_Pattern pattern) const {
			return XboxLEDAnimations<NumLEDs>::getAnimation(pattern);
		}

		void setLEDs(uint8_t ledStates) {
			for (uint8_t i = 0; i < NumLEDs; i++) {
				// Invert both variables and use != to get logical XOR
				digitalWrite(Pins[i], !(ledStates & (1 << i)) != !Inverted);
			}
		}

		// LED Information
		const uint8_t Pins[NumLEDs];
		const boolean Inverted = false;  // Flag for inverted output
	};

	template <uint8_t nleds>
	class XboxLED_CustomOutput : public XboxLEDHandler {
	public:
		constexpr uint8_t getNumLEDs() const {
			return nleds;
		}

	protected:
		const Animation & getAnimation(LED_Pattern pattern) const {
			static_assert(nleds == 1 || nleds == 4,
				"Error: Must use animations for either 1 or 4 LEDs");
			return XboxLEDAnimations<nleds>::getAnimation(pattern);
		}
	};

}  // End namespace

// Library API
using XboxLEDPattern = Xbox360Controller_LEDs::LED_Pattern;

template<uint8_t ...pins>
using XboxControllerLEDs = Xbox360Controller_LEDs::XboxLED_IndividualPins<pins...>;

template<uint8_t nleds>
using XboxControllerLEDs_Custom = Xbox360Controller_LEDs::XboxLED_CustomOutput<nleds>;

#endif
