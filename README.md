# Lumino

Lumino is an open and straightforward framework for the Open Source Microlight. The goal of this framework is to be flexible enough to allow for modes of differing structure. Live configuration of modes is not planned, so some familiarity with the code is required. The code is structured in such a way that configuration is simple enough for newbies and experienced developers alike.

Current Features:
* 13 Operating Modes
  * iNova Mode
  * TiltMorph - Strobe-based mode that changes color based on roll and blank time with pitch.
  * GeoMorph Only X - Strobe-based mode that blends between 3 user-defined colors based on pitch along the X axis.
  * GeoMorph Only Y - Strobe-based mode that blends between 3 user-defined colors based on pitch along the Y axis.
  * GeoMorph X and Y - Strobe-based mode that blends between 5 user-defined colors based on pitch along the X and Y axes.
  * Single Mode
  * Dual Mode Speed - Motion switches between two primes.
  * Tri Mode Speed - Slow / medium / fast switching between primes.
  * Dual Mode Tilt X - Primes switch when tilting hands up and down.
  * Dual Mode Tilt Y - Primes switch when hands are tilted to the left or right.
  * Dual Mode Flip - Primes switch when button is facing up and down.
  * Tri Mode Tilt X - Up / forward / down switches primes.
  * Tri Mode Tilt Y - Left / flat / right switches primes.

* 11 Primes, most with 8 color palettes, which are highly configurable
  * Strobe - Basic color blinking animation.
  * Tracer - One color fills the space between colors.
  * Blink-E - All colors in a row followed by a blank.
  * Morph - Colors blend into one another.
  * Fade - Color fades in, fades out, or fades in and then out.
  * Candy Strobe - Strobes a subset of colors for a time before switching out a color and continuing.
  * Chase - Strobes one color "chasing" another away.
  * TwoTime - Strobe with two times that play back to back.
  * Lego - Strobe with random (2ms, 8ms, or 16ms) times. Credit to Andrew Suchan.
  * Ember - Strobe increases and decreases in time. Credit to Andrew Suchan.
  * Rainbow - Strobes a color wheel rotation with configurable segments. No palette.

* 62 color configuable palette with 4 shades each + blank
* Conjuring mode toggle
* Inova mode toggle - Single color mode switches between always on, DOPS, and a hyperstrobe. I don't know the real timings of an Inova, so contact me if you happen to know.
* "Live" configuration and saving of palettes
* Will not overwrite existing NEO settings


# Installation

## From Source

* Download the latest .zip from the [releases tab](http://github.com/iterati/lumino/releases/).
* Open the Arduino Editor.
* Select **Sketch** -> **Import Library...** -> **Add Library...** from the menu and import the Lumino.zip you downloaded.
* Select **File** -> **Examples** -> **Lumino** (will be near the bottom of the list) -> **lumino** from the menu.
* Select **Sketch** -> **Verify / Compile** from the menu and let it complete.
* Select **File** -> **Upload** to upload to you light.


## Packaged release on Mac OS X

* Download the latest .dmg from the [releases tab](http://github.com/iterati/lumino/releases/).
* Open the .dmg and double-click on "Upload .hex"


## Packaged release on Windows

* Download the latest .hex from the [releases tab](http://github.com/iterati/lumino/releases/).
* Using [XLoader](http://russemotto.com/xloader/), follow [this tutorial](http://kmtronic.com/upload-hex-files-to-arduino.html) on how to install.
  1. Make sure to use 115200 for the speed.
  2. Make sure to select "Uno(ATMega328)" from the Device dropdown.
  3. Be sure to select the correct .hex file.


# Operation

## Change Bundles
With the light off, hold the button for 1.5s. Every 1.5s, the light will switch to the next bundle and flash blue. The current bundles are:
* All 12 modes
* The 4 speed-switches-prime modes
* The 5 tilt-switched-prime modes

## Play
This is the default mode to be in. You start here and exit to here when done editing.

**Play controls**
* single press - Go to next mode. Toggles light if in conjuring mode.
* hold for 1s to 3s - Flashes white. Turn off light.
* hold for 3s to 5s - Flashes blue. Toggles conjuring mode.
* hold for 5s+ - Flashes orange. Enter Palette Select mode.

**iNova controls**
* from off
  * press - Go to high.
  * hold for 1.5s to 5s - Switch to next mode.
  * hold for 5s+ - Flashes yellow. Enter Color Config mode.
  * nothing for 5 minutes - Light turns off to save battery.
* from high
  * press before 2.85s - Turn off LED. Switch to low when released.
  * press after 2.85s - Turn off LED. Switch to off when released.
* from low
  * press before 2.85s - Turn off LED. Switch to blink when released.
  * press after 2.85s - Turn off LED. Switch to off when released.
* from blink
  * press - Turn off LED. Switch to off when released.


## Palette Select
This is where you chose which palette to configure. For SingleMode, just blinks orange for it's one configurable palette. For DualMode, blinks either yellow or green for which palette you are on. For Tri- modes, blinks cyan, blue, or magenta for the three palettes. TiltMorph just glows red as does any prime that doesn't have a palette (e.g. RainbowPrime).

**Palette Select controls**
* single press - Go to next palette. Does nothing on Single Mode.
* hold for 1s to 3s - Flashes orange. Enter Color Select mode for first color of selected palette.
* hold for 3s+ - Flashes white. Exits to Play mode.

## Color Select
This is where you chose your color. You have a chance to preview and confirm or reject the color after selecting it.

**Color Select controls**
* single press - Go to next color index.
* double press - Go to previous color index.
* hold - Flashes white every shade change. Every 1s the shade increments until released. When released, the color is selected and and goes to Color Confirm mode.

## Color Confirm
This is where you confirm the color you just picked by watching a preview of the mode play. You can accept the color, reject the color, or accept color and finish editing. At this point, num_colors will be set on the Prime.

**Color Confirm controls**
* single press - Accepts color and moves to the next color in the palette. If on the last color, saves the current palette and returns to Palette Select mode.
* hold for 1-3s - Flashes red. Rejects color and move back to previous color. Goes to Color Confirm on previous color. If on the first color, goes directly to Color Select mode.
* hold for 3s+ - Flashes white. Accepts color and finish editing. Saves the current palette and returns to Palette Select mode.


# Default Modes and Customization

Modes and primes can be reconfigured in the code quite easilly by following the examples currently in the **lumino.ino**. Each type of mode is explained in the comments, but I will explain the idea behind each below.

Bundles can also easily be configured in lumino.ino.

* iNova - Classic iNova functionality with a twist. Hold for 1.5 - 5s from off to switch to next mode. Hold for >5s from off to configure your iNova color.
* TiltMorph - Hue changes with roll, blank time of strobe with pitch. Not customizable.
* GeoMorph A and Y - Color blends red to yellow pointing up, red to purple pointing down. That blends to blue when pitch goes to the right and green when to the left. All 5 colors are customizable.
* Tri Mode Speed - Rainbow blend goes from a strobie with a double split to strobe with 3 splits DOPS with 5 splits.
* Dual Mode Speed - Ghostly blink-e with a cool white and the dimmest spectrum that morphs into a rainbow strobie.
* Dual Mode Speed (Teleport) - Rainbow strobe that fades out quickly when your hands are slow or idle. Blank when you're moving quickly.
* Dual Mode Speed (Reverse Teleport) - Rainbow Lego when your hands are moving quickly. Blank when your hands are slow or idle.
* Tri Mode Tilt Y - Red, green, and blue embers for flat, left, and right.
* Tri Mode Tilt X - Red, green, and blue tracers with a rainbow strobe for flat, up, and down.
* Dual Mode Flip - Pulsating blue when up and red when down.
* Dual Mode Tilt Y - Strobe when left, dash dop when right.
* Dual Mode Tilt X - Chase when up, morph when down.
