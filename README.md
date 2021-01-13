# Xbox 360 Controller LEDs Library
[![arduino-library-badge](https://www.ardu-badge.com/badge/Xbox%20360%20Controller%20LEDs.svg?)](https://www.ardu-badge.com/Xbox%20360%20Controller%20LEDs) [![Build Status](https://github.com/dmadison/Xbox360ControllerLEDs/workflows/build/badge.svg?branch=master)](https://github.com/dmadison/Xbox360ControllerLEDs/actions?query=workflow%3Abuild)

This is an Arduino library that attempts to mimic the LED patterns of the Xbox 360 controller. Both single-LED and quad-LED pattern sets are supported, and can be triggered directly by the user or from relevant USB data.

The animation timing uses the built-in Arduino `millis` function as a timer and does not require a dedicated ISR. The library's main `run` function can be called either in `loop()` or as part of another interrupt. Updates should be performed at least every 100 ms while a non-static animation is running.

See [this post](https://www.partsnotincluded.com/reverse-engineering/xbox-360-controller-led-animations/) on Parts Not Included for a list of the available animations.

## Getting Started

Download the library [here](../../archive/master.zip) and follow [the instructions on the Arduino website](https://www.arduino.cc/en/guide/libraries#toc4) for installing a library from a .zip file. You can also install the library via the [libraries manager](https://www.arduino.cc/en/guide/libraries#toc3) of the Arduino IDE.

Once the library has been installed, load one of the example sketches by going to `File -> Examples -> Xbox 360 Controller LEDs` and selecting one of the available examples. I recommend starting with the "Blink" example, which will use your Arduino's built-in LED. Examples that use animations with 4 LEDs will require 4 external LEDs with proper resistors.

**Note**: This library is only for emulating the Xbox 360 controller's **LEDs**. It will **not** make your Arduino work like an Xbox 360 controller. For that, you can use [the Arduino XInput library](https://github.com/dmadison/ArduinoXInput).

## Output Modes

This is an animation library first and foremost, and as such it's up to the user to define what they want to do with the LED data. The library includes a default output class that will write the animated LED states to the Arduino's pins using `digitalWrite`, but it also makes it possible to use a custom output class in order to write to shift registers, addressable LEDs, you name it.

The included ["UserOutput" example](examples/UserOutput/UserOutput.ino) demonstrates how this works by using [the FastLED library](https://github.com/FastLED/FastLED) to run the controller animations on a strip of addressable WS2812B LEDs ("NeoPixels"). Custom output modes can use either single-LED or quad-LED animations.

## Credits and Contributions

If you would like to submit any improvements to this library, pull requests are open and welcome!

Huge thank you to [Stephen Whittle](https://github.com/stephenwhittle) who helped me work out the kinks with the variadic template pin initialization and variable frame sizes. Thanks Stephen!

## License
Released under the terms of the [MIT license](https://opensource.org/licenses/MIT). See the [LICENSE](LICENSE) file for more information.
