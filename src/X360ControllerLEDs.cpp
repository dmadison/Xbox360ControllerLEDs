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

//  --- 1 LED Animations -----------------------------------------------------

constexpr uint8_t XboxLEDAnimations<1>::On;
constexpr uint8_t XboxLEDAnimations<1>::Off;

const LED_Animation<1> XboxLEDAnimations<1>::Anim_Off({
	{ XboxLEDAnimations<1>::Off, 0 }
});

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Blinking({
	{ XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::BlinkTime },
	{ XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::BlinkTime }
});

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash1({
	{ XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime },
	{ XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime }},
	PlayerFlashTime,
	LED_Pattern::Player1  // After flashing, go to Player 1
);

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash2({
	{ XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime },
	{ XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime } },
	PlayerFlashTime,
	LED_Pattern::Player2  // After flashing, go to Player 2
);

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash3({
	{ XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime },
	{ XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime } },
	PlayerFlashTime,
	LED_Pattern::Player3  // After flashing, go to Player 3
);

const LED_Animation<2> XboxLEDAnimations<1>::Anim_Flash4({
	{ XboxLEDAnimations<1>::Off, XboxLEDAnimations<1>::FlashTime },
	{ XboxLEDAnimations<1>::On,  XboxLEDAnimations<1>::FlashTime } },
	PlayerFlashTime,
	LED_Pattern::Player4  // After flashing, go to Player 4
);

const LED_Animation<1> XboxLEDAnimations<1>::Anim_Player1({
	{ XboxLEDAnimations<1>::On, 0 },  // Always on, usual mode. Non-distracting.
});

const LED_Animation<4> XboxLEDAnimations<1>::Anim_Player2({
	// Flash 1
	{ XboxLEDAnimations<1>::Off, PlayerTime + PlayerLoopTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },

	// Flash 2
	{ XboxLEDAnimations<1>::Off, PlayerTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },
});

const LED_Animation<6> XboxLEDAnimations<1>::Anim_Player3({
	// Flash 1
	{ XboxLEDAnimations<1>::Off, PlayerTime + PlayerLoopTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },

	// Flash 2
	{ XboxLEDAnimations<1>::Off, PlayerTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },

	// Flash 3
	{ XboxLEDAnimations<1>::Off, PlayerTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },
});

const LED_Animation<8> XboxLEDAnimations<1>::Anim_Player4({
	// Flash 1
	{ XboxLEDAnimations<1>::Off, PlayerTime + PlayerLoopTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },

	// Flash 2
	{ XboxLEDAnimations<1>::Off, PlayerTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },

	// Flash 3
	{ XboxLEDAnimations<1>::Off, PlayerTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },

	// Flash 4
	{ XboxLEDAnimations<1>::Off, PlayerTime },
	{ XboxLEDAnimations<1>::On,  PlayerTime },
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
	{XboxLEDAnimations<4>::States_Off, 0 } 
});

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Blinking({
	{ XboxLEDAnimations<4>::States_Off, XboxLEDAnimations<4>::BlinkTime },
	{ XboxLEDAnimations<4>::States_On,  XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime * 2) * 4,   // Blink 4 times
	LED_Pattern::BlinkSlow  // After blinking fast, blink slow
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_BlinkOnce({
	{ XboxLEDAnimations<4>::States_Off, XboxLEDAnimations<4>::BlinkSlow },
	{ XboxLEDAnimations<4>::States_On,  XboxLEDAnimations<4>::BlinkTime }},
	(BlinkSlow + BlinkTime), // Blink once
	LED_Pattern::Previous  // After blinking, go back to previous
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_BlinkSlow({
	{ XboxLEDAnimations<4>::States_Off, XboxLEDAnimations<4>::BlinkSlow },
	{ XboxLEDAnimations<4>::States_On,  XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime + BlinkSlow) * 16,   // Blink 16 times
	LED_Pattern::Previous  // After blinking slow, go back to previous
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash1({
	{ XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime },
	{ XboxLEDAnimations<4>::States_Player1, XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime * 2) * PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player1  // After flashing, go to Player 1
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash2({
	{ XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime },
	{ XboxLEDAnimations<4>::States_Player2, XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime * 2) * PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player2  // After flashing, go to Player 2
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash3 ({
	{ XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime },
	{ XboxLEDAnimations<4>::States_Player3, XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime * 2) * PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player3  // After flashing, go to Player 3
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Flash4({
	{ XboxLEDAnimations<4>::States_Off,     XboxLEDAnimations<4>::BlinkTime },
	{ XboxLEDAnimations<4>::States_Player4, XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime * 2) * PlayerBlinkCount,  // Flash n times
	LED_Pattern::Player4  // After flashing, go to Player 4
);

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player1 ({
	{ XboxLEDAnimations<4>::States_Player1, 0 },
});

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player2 ({
	{ XboxLEDAnimations<4>::States_Player2, 0 },
});

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player3 ({
	{ XboxLEDAnimations<4>::States_Player3, 0 },
});

const LED_Animation<1> XboxLEDAnimations<4>::Anim_Player4 ({
	{ XboxLEDAnimations<4>::States_Player4, 0 },
});

const LED_Animation<4> XboxLEDAnimations<4>::Anim_Rotating ({
	{ XboxLEDAnimations<4>::States_Player1, XboxLEDAnimations<4>::RotateTime },
	{ XboxLEDAnimations<4>::States_Player2, XboxLEDAnimations<4>::RotateTime },
	{ XboxLEDAnimations<4>::States_Player3, XboxLEDAnimations<4>::RotateTime },
	{ XboxLEDAnimations<4>::States_Player4, XboxLEDAnimations<4>::RotateTime }},
	(RotateTime * 4) * 50,  // Rotate 50 times
	LED_Pattern::Previous  // After rotating, go back to previous
);

const LED_Animation<2> XboxLEDAnimations<4>::Anim_Alternating ({
	{ XboxLEDAnimations<4>::States_Op1, XboxLEDAnimations<4>::BlinkTime },
	{ XboxLEDAnimations<4>::States_Op2, XboxLEDAnimations<4>::BlinkTime }},
	(BlinkTime * 2) * 7,  // Alternate 7 times
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
