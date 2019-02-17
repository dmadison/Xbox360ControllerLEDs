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

#include "X360ControllerLEDs.h"

namespace Xbox360Controller_LEDs {

static const boolean isPlayerFlash(LED_Pattern pattern) {
	switch (pattern) {
	case(LED_Pattern::Flash1):
	case(LED_Pattern::Flash2):
	case(LED_Pattern::Flash3):
	case(LED_Pattern::Flash4):
		return true;
	default:
		return false;
	}
}

static const boolean isPlayerSolid(LED_Pattern pattern) {
	switch (pattern) {
	case(LED_Pattern::Player1):
	case(LED_Pattern::Player2):
	case(LED_Pattern::Player3):
	case(LED_Pattern::Player4):
		return true;
	default:
		return false;
	}
}

// Dummy animation to populate the currentAnimation pointer
static const LED_Animation<1> Animation_Null(
	{ LED_Frame(0, 0) },  // No LEDs lit, infinite frame duration
	0,                    // Infinite animation repeat
	LED_Pattern::Null     // Next animation is no animation
);

//  --- LED Handler Class Base -----------------------------------------------

XboxLEDHandler::XboxLEDHandler() :
	currentPattern(LED_Pattern::Null),  // Not a pattern
	previousPattern(currentPattern),
	currentAnimation(&Xbox360Controller_LEDs::Animation_Null)
{}

void XboxLEDHandler::setPattern(LED_Pattern pattern) {
	if ((uint8_t)pattern >= XboxLEDHandler::NumPatterns) return;  // Meta pattern, ignore
	linkPatterns = false;  // Don't auto-link to the next pattern
	setPattern(pattern, true);  // Screw it, do the pattern NOW
}

void XboxLEDHandler::linkPattern(LED_Pattern pattern) {
	if ((uint8_t)pattern >= XboxLEDHandler::NumPatterns) return;  // Meta pattern, ignore
	linkPatterns = true;  // Auto-link to the next pattern if available
	setPattern(pattern, false);  // Set pattern, but don't run immediately if it is next pattern in queue
}

void XboxLEDHandler::setPattern(LED_Pattern pattern, boolean runNow) {
	if (currentPattern == pattern) return;  // No change
	if (runNow == false && pattern == currentAnimation->Next) return;  // That's the next pattern! We'll get there...
	if (linkPatterns && isPlayerFlash(pattern) && isPlayerSolid(currentPattern)) return;  // Don't go back to flashing if player is solid

	// If pattern says go back, load prevous pattern
	if (pattern == LED_Pattern::Previous) {
		pattern = previousPattern;
	}

	// If not currently running a temporary pattern, save as previous
	if (currentAnimation->Next != LED_Pattern::Previous) {
		previousPattern = currentPattern;  // Save current pattern as previous
	}

	currentPattern = pattern;  // Save pattern (enum)

	const Animation * newAnimation = &getAnimation(currentPattern);
	if (currentAnimation == newAnimation) return;  // Different pattern, same animation
	currentAnimation = newAnimation;  // Save animation (pointer)

	frameIndex = 0;  // Zero frame index (start of animation frames)
	cycleCount = 0;  // Zero cycle count (start of animation cycle)
	runFrame();  // Run once
}

void XboxLEDHandler::runFrame() {
	const LED_Frame & frame = currentAnimation->getFrame(frameIndex);
	time_frameDuration = frame.Duration * LED_Frame::Timescale;  // Save current frame duration as ms
	time_frameLast = millis();  // Save time
	lastLEDFrame = frame.LEDs;  // Save current frame

	if (writeOutput) {
		setLEDs(frame.LEDs);  // Set LEDs to current frame}
	}	
}

void XboxLEDHandler::run() {
	if (currentAnimation->getNumFrames() <= 1 || time_frameDuration == 0) return;  // No processing necessary
	if (millis() - time_frameLast < time_frameDuration) return;  // Not time yet

	frameIndex++;  // Go to next frame
	if (frameIndex >= currentAnimation->getNumFrames()) {  // End of animation cycle
		cycleCount++;  // Increase animation cycle counter (linking can be enabled at any point)
		// If linking enabled AND animation has an ending AN we're *at* that ending...
		if (linkPatterns && currentAnimation->NCycles != 0 && cycleCount >= currentAnimation->NCycles) {
				setPattern(currentAnimation->Next, true);    // Run next pattern + override "Next" check
			return;
		}
		frameIndex = 0;  // If at last frame, go to start
	}
	runFrame();  // Write current frame to LEDs
}

void XboxLEDHandler::pauseOutput() {
	writeOutput = false;
}

void XboxLEDHandler::resumeOutput() {
	if (writeOutput == false) {
		writeOutput = true;
		rewriteFrame();  // If unpausing, rewrite current LED frame
	}
}

LED_Pattern XboxLEDHandler::getPattern() const {
	return currentPattern;  // Current pattern as enum
}

uint8_t XboxLEDHandler::getLastFrame() const {
	return lastLEDFrame;
}

void XboxLEDHandler::rewriteFrame() {
	setLEDs(lastLEDFrame);  // Re-output with last LED data
}


//  --- 1 LED Animations -----------------------------------------------------

constexpr uint8_t XboxLEDAnimations<1>::On;
constexpr uint8_t XboxLEDAnimations<1>::Off;

const LED_Animation<1> XboxLEDAnimations<1>::Anim_Off({
	LED_Frame(XboxLEDAnimations<1>::Off, 0)
});

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Blinking({
	LED_Frame(XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::BlinkTime),
	LED_Frame(XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::BlinkTime),
});

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash1({
	LED_Frame(XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime),
	LED_Frame(XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime)},
	PlayerFlashCount,
	LED_Pattern::Player1  // After flashing, go to Player 1
);

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash2({
	LED_Frame(XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime),
	LED_Frame(XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime)},
	PlayerFlashCount,
	LED_Pattern::Player2  // After flashing, go to Player 2
);

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash3({
	LED_Frame(XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime),
	LED_Frame(XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime)},
	PlayerFlashCount,
	LED_Pattern::Player3  // After flashing, go to Player 3
);

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash4({
	LED_Frame(XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime),
	LED_Frame(XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime)},
	PlayerFlashCount,
	LED_Pattern::Player4  // After flashing, go to Player 4
);

const LED_Animation<1> XboxLEDAnimations<1>::Anim_Player1({
	LED_Frame(XboxLEDAnimations<1>::On, 0),  // Always on, usual mode. Non-distracting.
});

const LED_Animation<4> XboxLEDAnimations<1>::Anim_Player2({
	// Flash 1
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime + PlayerLoopTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),

	// Flash 2
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),
});

const LED_Animation<6> XboxLEDAnimations<1>::Anim_Player3({
	// Flash 1
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime + PlayerLoopTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),

	// Flash 2
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),

	// Flash 3
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),
});

const LED_Animation<8> XboxLEDAnimations<1>::Anim_Player4({
	// Flash 1
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime + PlayerLoopTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),

	// Flash 2
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),

	// Flash 3
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),

	// Flash 4
	LED_Frame(XboxLEDAnimations<1>::Off, PlayerTime),
	LED_Frame(XboxLEDAnimations<1>::On,  PlayerTime),
});

const AnimationBase & XboxLEDAnimations<1>::getAnimation(LED_Pattern pattern) {
	switch (pattern) {
	case(LED_Pattern::Off):         return Anim_Off;
	case(LED_Pattern::Blinking):    return Anim_Blinking;
	case(LED_Pattern::Flash1):      return Anim_Flash1;
	case(LED_Pattern::Flash2):      return Anim_Flash2;
	case(LED_Pattern::Flash3):      return Anim_Flash3;
	case(LED_Pattern::Flash4):      return Anim_Flash4;
	case(LED_Pattern::Player1):     return Anim_Player1;
	case(LED_Pattern::Player2):     return Anim_Player2;
	case(LED_Pattern::Player3):     return Anim_Player3;
	case(LED_Pattern::Player4):     return Anim_Player4;
	case(LED_Pattern::Rotating):    // Intentionally
	case(LED_Pattern::BlinkOnce):   // Fall
	case(LED_Pattern::BlinkSlow):   // Through Cases
	case(LED_Pattern::Alternating): return Anim_Blinking;
	default: return Anim_Off;
	}
}

//  --- 4 LED Animations -----------------------------------------------------

constexpr uint8_t XboxLEDAnimations<4>::States_Off;
constexpr uint8_t XboxLEDAnimations<4>::States_On;

constexpr uint8_t XboxLEDAnimations<4>::States_Player1;
constexpr uint8_t XboxLEDAnimations<4>::States_Player2;
constexpr uint8_t XboxLEDAnimations<4>::States_Player3;
constexpr uint8_t XboxLEDAnimations<4>::States_Player4;

constexpr uint8_t XboxLEDAnimations<4>::States_Op1;
constexpr uint8_t XboxLEDAnimations<4>::States_Op2;

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Off ({ 
	LED_Frame(XboxLEDAnimations<4>::States_Off, 0)
});

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Blinking({
	LED_Frame(XboxLEDAnimations<4>::States_Off, XboxLEDAnimations<4>::BlinkTime),
	LED_Frame(XboxLEDAnimations<4>::States_On,  XboxLEDAnimations<4>::BlinkTime)},
	4,   // Blink 4 times
	LED_Pattern::BlinkSlow  // After blinking fast, blink slow
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_BlinkOnce({
	LED_Frame(XboxLEDAnimations<4>::States_Off, XboxLEDAnimations<4>::BlinkSlow),
	LED_Frame(XboxLEDAnimations<4>::States_On,  XboxLEDAnimations<4>::BlinkTime)},
	1, // Blink once
	LED_Pattern::Previous  // After blinking, go back to previous
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_BlinkSlow({
	LED_Frame(XboxLEDAnimations<4>::States_Off, XboxLEDAnimations<4>::BlinkSlow),
	LED_Frame(XboxLEDAnimations<4>::States_On,  XboxLEDAnimations<4>::BlinkTime)},
	16,   // Blink 16 times
	LED_Pattern::Previous  // After blinking slow, go back to previous
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash1({
	LED_Frame(XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player1, XboxLEDAnimations<4>::BlinkTime)},
	PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player1  // After flashing, go to Player 1
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash2({
	LED_Frame(XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player2, XboxLEDAnimations<4>::BlinkTime)},
	PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player2  // After flashing, go to Player 2
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash3 ({
	LED_Frame(XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player3, XboxLEDAnimations<4>::BlinkTime)},
	PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player3  // After flashing, go to Player 3
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash4({
	LED_Frame(XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player4, XboxLEDAnimations<4>::BlinkTime)},
	PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player4  // After flashing, go to Player 4
);

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player1 ({
	LED_Frame(XboxLEDAnimations<4>::States_Player1, 0),
});

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player2 ({
	LED_Frame(XboxLEDAnimations<4>::States_Player2, 0),
});

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player3 ({
	LED_Frame(XboxLEDAnimations<4>::States_Player3, 0),
});

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player4 ({
	LED_Frame(XboxLEDAnimations<4>::States_Player4, 0),
});

const LED_Animation<4> XboxLEDAnimations<4>::Anim_Rotating ({
	LED_Frame(XboxLEDAnimations<4>::States_Player1, XboxLEDAnimations<4>::RotateTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player2, XboxLEDAnimations<4>::RotateTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player4, XboxLEDAnimations<4>::RotateTime),
	LED_Frame(XboxLEDAnimations<4>::States_Player3, XboxLEDAnimations<4>::RotateTime)},
	50,  // Rotate 50 times
	LED_Pattern::Previous  // After rotating, go back to previous
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Alternating ({
	LED_Frame(XboxLEDAnimations<4>::States_Op1, XboxLEDAnimations<4>::BlinkTime),
	LED_Frame(XboxLEDAnimations<4>::States_Op2, XboxLEDAnimations<4>::BlinkTime)},
	7,  // Alternate 7 times
	LED_Pattern::Previous  // After alternating, go back to previous
);

const AnimationBase & XboxLEDAnimations<4>::getAnimation(LED_Pattern pattern) {
	switch (pattern) {
	case(LED_Pattern::Off):         return Anim_Off;
	case(LED_Pattern::Blinking):    return Anim_Blinking;
	case(LED_Pattern::Flash1):      return Anim_Flash1;
	case(LED_Pattern::Flash2):      return Anim_Flash2;
	case(LED_Pattern::Flash3):      return Anim_Flash3;
	case(LED_Pattern::Flash4):      return Anim_Flash4;
	case(LED_Pattern::Player1):     return Anim_Player1;
	case(LED_Pattern::Player2):     return Anim_Player2;
	case(LED_Pattern::Player3):     return Anim_Player3;
	case(LED_Pattern::Player4):     return Anim_Player4;
	case(LED_Pattern::Rotating):    return Anim_Rotating;
	case(LED_Pattern::BlinkOnce):   return Anim_BlinkOnce;
	case(LED_Pattern::BlinkSlow):   return Anim_BlinkSlow;
	case(LED_Pattern::Alternating): return Anim_Alternating;
	default: return Anim_Off;
	}
}

}  // End Namespace
