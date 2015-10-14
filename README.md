# Lumino

Lumino is an open and straightforward framework for the Open Source Microlight. The goal of this framework is to be flexible enough to allow for modes of differing structure. Live configuration of modes is not planned, so some familiarity with the code is required. The code is structured in such a way that configuration is simple enough for newbies and experienced developers alike.

Current Features:
* 8 Operating Modes
  * Single Mode - No accelerometer switching.
  * Dual Mode Speed - Motion switches between two primes.
  * Tri Mode Speed - Slow / medium / fast switching between primes.
  * Dual Mode Tilt X - Primes switch when tilting hands up and down.
  * Dual Mode Tilt Y - Primes switch when hands are tilted to the left or right.
  * Dual Mode Flip - Primes switch when button is facing up and down.
  * Tri Mode Tilt X - Up / forward / down switching between primes.
  * Tri Mode Tilt Y - Left / flat / right switching between primes.
  * TiltMorph - Primeless mode that changes color based on roll and blink speed with pitch.

* 8 Primes, most with 8 color palettes, which are highly configurable
  * Strobe - Basic color blinking animation.
  * Tracer - One color fills the space between colors.
  * Blink-E - All colors in a row followed by a blank.
  * Morph - Colors blend into one another.
  * Fade - Color fades in, fades out, or fades in and then out.
  * Candy Strobe - Strobes a subset of colors for a time before switching out a color and continuing.
  * Chase - Strobes one color "chasing" another away.
  * Rainbow - Strobes a color wheel rotation with configurable segments. No palette.

* 62 color configuable palette with 4 shades each + blank
* Conjuring mode toggle
* "Live" configuration and saving of palettes
* Will not overwrite existing NEO settings


# Installation

* Download the latest release from the [releases tab](http://github.com/iterati/lumino/releases/). **Be sure to download Lumino.zip and not the source code!**
* Open the Arduino Editor.
* Select **Sketch** -> **Import Library...** -> **Add Library...** from the menu and import the Lumino.zip you downloaded.
* Select **File** -> **Examples** -> **Lumino** (will be near the bottom of the list) -> **lumino** from the menu.
* Select **Sketch** -> **Verify / Compile** from the menu and let it complete.
* Select **File** -> **Upload** to upload to you light.


# Operation

## Play
This is the default mode to be in. You start here and exit to here when done editing.

**Play controls**
* single press - Go to next mode. Toggles light if in conjuring mode.
* hold for 1s to 3s - Turn off light.
* hold for 3s to 5s - Toggles conjuring mode.
* hold for 5s+ - Enter Palette Select mode.

## Palette Select
This is where you chose which palette to configure. For SingleMode, just blinks yellow for it's one configurable palette. For DualMode, blinks either cyan or magenta for which palette you are on. For Tri- modes, blinks red, green, or blue for the three palettes. TiltMorph just glows red as does any prime that doesn't have a palette (e.g. RainbowPrime).

**Palette Select controls**
* single press - Go to next palette. Does nothing on Single Mode.
* hold for 1s to 3s - Enter Color Select mode for first color of selected palette.
* hold for 3s+ - Exits to Play mode.

## Color Select
This is where you chose your color. You have a chance to preview and confirm or reject the color after selecting it.

**Color Select controls**
* single press - Go to next color index.
* double press - Go to previous color index.
* hold - Every 1s the shade increments until released. When released, the color is selected and and goes to Color Confirm mode.

## Color Confirm
This is where you confirm the color you just picked by watching a preview of the mode play. You can accept the color, reject the color, or accept color and finish editing. At this point, num_colors will be set on the Prime.

**Color Confirm controls**
* single press - Accepts color and moves to the next color in the palette. If on the last color, saves the current palette and returns to Palette Select mode.
* hold for 1-3s - Rejects color and move back to previous color. Goes to Color Confirm on previous color. If on the first color, goes directly to Color Select mode.
* hold for 3s+ - Accepts color and finish editing. Saves the current palette and returns to Palette Select mode.


# Default Modes and Customization

Modes and primes can be reconfigured in the code quite easilly by following the examples currently in the **lumino.ino**. Each type of mode is explained in the comments, but I will explain the idea behind each below.

* TiltMorph - Not customizable.
* Tri Mode Rainbow - Goes from a strobie with a double split to strobe with 3 splits hyperstrobe with 5 splits.
* Tri Mode Tilt X - Rainbow colors with red, green, and blue tracers for forward, down, and up.
* Dual Mode Speed - Ghostly blink-e with a cool white and the dimmest spectrum that morphs into a rainbow strobie.
* Dual Mode Tilt X - Chase for up and morph for down with a contrasting rainbow palette.
* Dual Mode Tilt Y - Rotates between two color triads on left and right with a hypnotic fade in/out.
* Dual Mode Flip - Candy strobe when button is up and normal strobe when down. With a rainbow palette.
* Single Mode - DOPS candy strobe with contrasting rainbow palette.


That's it! Lumino doesn't use EEPROM so your NEO settings will be untouched.
