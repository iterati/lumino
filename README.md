# Lumino

Lumino is an open and straightforward framework for the Open Source Microlight. The goal of this framework is to be flexible enough to allow for modes of differing structure. Live configuration of modes is not planned, so some familiarity with the code is required. The code is structured in such a way that configuration is simple enough for newbies and experienced developers alike.

Current Features:
* 4 Modes (SingleMode, DualMode, TriMode, and TiltMorph)
* 4 Primes (Strobe, Tracer, Blink-E, and Morph) with 1 to 8 color palettes
* 32 (31 + blank) Color palette with 4 shades per color
* Conjuring mode


# Installation

* Download the latest release from the [releases tab](http://github.com/iterati/releases/). **Be sure to download Lumino.zip and not the source code!**
* Open the Arduino Editor.
* Select **Sketch** -> **Import Library...** -> **Add Library...** from the menu and import the Lumino.zip you downloaded.
* Select **File** -> **Examples** -> **Lumino** (will be near the bottom of the list) -> **lumino** from the menu.
* Select **Sketch** -> **Verify / Compile** from the menu and let it complete.
* Select **File** -> **Upload** to upload to you light.


# Operation

## Switching modes

Press the button to cycle between modes.

**Default Modes**

1. TiltMorph
2. 8 colors with red (level), green (tilt fingers up), or blue (tilt fingers down) tracer.
3. 8 colors with red (level), green (tilt hand right), blue (tilt hand left) tracer.
4. White with 7 color dim trail blink-e when slow, 8 color strobie when fast.
5. 8 color strobe at start and when fingers down, 8 color strobie when fingers up.
6. 8 color strobe at start and when hand tilted left, 8 color strobie when hand tilted right.
7. 8 color strobe when hand rightside up, 8 color strobie when hand upside down.
8. 8 color hyperstrobe morph.

## Turning off the light

Hold the button in for 1s, until the light flashes white, and upon release, the
light will turn off.

## Conjuring Mode

To enable conjuring mode, hold the button in for 3s until the light flashes blue. Upon release, conjuring mode will be enabled and the light will turn on and off with a single press. To reenable normal use, hold the button in for 3s again. Turning the light off in conjuring mode doesn't actually turn the light off, it only turns off the bulb. The CPU will still be cycling and taking up battery. To turn off the light from conjuring mode, just hold the button for 1s as usual. Turning the light off will disable conjuring mode the next time the light is turned on.


# Configuration

Currently all configuration must be done in code. You can configure the color palette and the modes you would like to use. The number of modes is entirely up to you and how much memory is available on the chip.

## Palette Config

You can configure your palette by editing the ```color_palette``` array located at the top of ```modes.cpp```. Values from 0 to 255 will work with 255 being full power. Lumino uses a gamma-curved palette to provide a more relative brightness. This means that a palette value of (64, 0, 0) should be red and about half as bright as (128, 0, 0).

Colors are listed in modes.cpp and have their hex number documented next to them. For instance, red (255, 0, 0) is marked as (0x08). Each level of dimness applied by adding 0x40 to the value. So the different dimnesses of red are 0x08, 0x48, 0x88, and 0xc8. Since the value is hex (base16) values for 10 - 15 (16 is 0x10), are a (10), b (11), c (12), d (13), e (14), and f (15). The dimmest blue is 0xd8. 0x18 (the hex value for blue) + 0xc0 (c(12) + 1 = d(13)).

If you wish to use less colors, you can leave them set as they are for later and simply change the num_colors value to the number of colors you wish to use.

## Mode Config

Each mode will have it's own configuration options. The best way to know what is configurable on a mode is to look at the header file ```modes.h```. Modes and their documentation are at the end of the file.  There are currently 3 modes:

* SingleMode - Uses a single prime with no accelerometer magic.
* DualMode - Can be configured to switch between primes based on speed or tilt along the X, Y, or Z axes.
* Tilted - Can be configured to switch between three primes based on tilt along the X or Y axes.
* TiltMorph - Hue slowly cycles along the color wheel. The light's roll alters the hue and the pitch changes the strobe timings.

### SingleMode()

Does not take any arguments, just requires a prime.

**Must be set in setupModes()**
* ```prime``` - Must be set to a Prime
* ```reset()``` - Sets up the mode

### DualMode(trigger type, sensitivity)

* Trigger type - What type of accelerometer action kicks off the change?
  * A_SHAKE - Moving too quickly changes variant. Recommended sensitivity is 0.5 or greater for quick changes and less for slow changes.
  * A_TILTX - Starts with the down variant and changes when your fingers point up. Changes back when you point your fingers down. Recommended sensitivity is 0.05 or lower.
  * A_TILTY - Starts with the left variant and changes when your hand tilts to the right. Changes back when your hand points to the left. Recommended sensitivity is 0.05 or lower.
  * A_TILTZ - Starts with the button up variant and changes when the button points down. Changes back when the button points up. Recommended sensitivity is 0.05 or lower.
* Sensitivity - The lower the less sensitive to change the accelerometer is.

**Must be set in setupModes()**
* ```prime[0]``` - Must be set to a Prime
* ```prime[1]``` - Must be set to a Prime
* ```reset()``` - Sets up the mode

### TriMode(axis, sensitivity)

* Trigger type - Either A_TILTX or A_TILTY. Changes when pointing up or down (left or right) and back when above or below flat again.
* Sensitivity - Recommended 0.05 or lower.

**Must be set in setupModes()**
* ```prime[0]``` - Must be set to a Prime
* ```prime[1]``` - Must be set to a Prime
* ```prime[2]``` - Must be set to a Prime
* ```reset()``` - Sets up the mode

### TiltMorph(sensitivity)

* Sensitivity - mostly used for smoothing transitions. Recommended 0.05.

**Must be set in setupModes()**
* ```reset()``` - Sets up the mode

## Prime Config

Some modes will use animation primes as a base for the mode. Primes, like Modes,
are defined and documented in ```modes.h```. Primes should be at the top of the
file above the modes. There are currently 3 Primes:

* Strobe - On/off through up to an 8 color palette
* Tracer - Tracer/color through up to an 8 color palette (plus tracer color)
* Blink-E - Color/color/color../blank through up to an 8 color palette
* Morph - Color blends to next color/blank time through an up to 8 color palette

### StrobePrime(color time, blank time)

* Color time - ms color is shown before blank and switching to next color
* Blank time - ms blank is shown

**Must be set in setupModes()**
* ```num_colors``` - Number of colors in your palette. 1 to 8.
* ```palette[#]``` - # (0-7) color in the palette (index starts at 0, fo first color is 0, third is 2, etc)

### TracerPrime(color time, tracer time, tracer color)

* Color time - ms color is shown before tracer and switching to next color
* Tracer time - ms tracer is shown
* Tracer color - Tracer color value. A dim color is recommended.

**Must be set in setupModes()**
* ```num_colors``` - Number of colors in your palette. 1 to 8.
* ```palette[#]``` - # (0-7) color in the palette

### BlinkEPrime(color time, blank time)

* Color time - ms color is shown before changing
* Blank time - ms blank is shown

**Must be set in setupModes()**
* ```num_colors``` - Number of colors in your palette. 1 to 8.
* ```palette[#]``` - # (0-7) color in the palette

### MorphPrime(color time, blank time)

* Color time - ms color is shown and blended before changing
* Blank time - ms blank is shown

**Must be set in setupModes()**
* ```num_colors``` - Number of colors in your palette. 1 to 8.
* ```palette[#]``` - # (0-7) color in the palette

## Number of Modes

The number of modes can be changed to any number only limited by what modes will
fit in memory. To do this, a number of things must happen in ```lumino.ino```:

* The line ```#define NUM_MODES``` must be set
* You must declare each mode (and it's primes) as shown
* You must configure each mode in the setupModes function
* You must add a pointer to the mode to the line beginning ```Mode *modes[NUM_MODES]```
(use & in front of the variable name e.g. &mode13)


# Reverting to NEO

* Open Arduino IDE.
* Select **File** -> **Examples** -> **OSM_NEO..** -> **_NEO..**
* Select **File** -> **Upload**

That's it! Lumino doesn't use EEPROM so your NEO settings will be untouched.
