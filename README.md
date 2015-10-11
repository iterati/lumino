# Lumino

Lumino is an open and straightforward framework for the Open Source Microlight. The goal of this framework is to be flexible enough to allow for modes of differing structure. Live configuration of modes is not planned, so some familiarity with the code is required. The code is structured in such a way that configuration is simple enough for newbies and experienced developers alike.

Current Features:
* 5 Modes (SingleMode, DualMode, TriMode, TiltMorph, and Speeder)
* 5 Primes (Strobe, Tracer, Blink-E, Morph, and Fade) with 1 to 8 color palettes
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
2. TriMode with 8 color palettes with red (level), green (tilt fingers up), or blue (tilt fingers down) tracers.
3. Speeder with red->green (slow), green->blue (medium), blue->red (fast) strobie.
4. White with 7 color dim trail blink-e when slow, 8 color strobie when fast.
5. 8 color fade out (down), strobe morph (up).
6. 4 color palette red->cyan (left) and 4 color palette cyan->red (right) strobe.
7. 4 color palette purple->yellow (button up) and 4 color palette yellow->purple (button down) strobe.
8. 8 color hyperstrobe morph.

## Turning off the light

Hold the button in for 1s, until the light flashes white, and upon release, the
light will turn off.

## Conjuring Mode

To enable conjuring mode, hold the button in for 3s until the light flashes blue. Upon release, conjuring mode will be enabled and the light will turn on and off with a single press. To reenable normal use, hold the button in for 3s again. Turning the light off in conjuring mode doesn't actually turn the light off, it only turns off the bulb. The CPU will still be cycling and taking up battery. To turn off the light from conjuring mode, just hold the button for 1s as usual. Turning the light off will disable conjuring mode the next time the light is turned on.

## Bundles

With the light turned off, hold the button in for 1.5s and it will flash red and cycle to the next bundle. The current mode when next turned on will be the first mode of the selected bundle.


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

## Primes

Some modes will use animation primes as a base for the mode. Primes, like Modes, are defined and documented in ```modes.h```. Primes should be at the top of the file above the modes. There are currently 3 Primes:

* Strobe - On/off through up to an 8 color palette
* Tracer - Tracer/color through up to an 8 color palette (plus tracer color)
* Blink-E - Color/color/color../blank through up to an 8 color palette
* Morph - Color blends to next color/blank time through an up to 8 color palette

### StrobePrime(color time, blank time)

* Color time - ms color is shown before blank and switching to next color
* Blank time - ms blank is shown

### TracerPrime(color time, tracer time, tracer color)

* Color time - ms color is shown before tracer and switching to next color
* Tracer time - ms tracer is shown
* Tracer color - Tracer color value. A dim color is recommended.

### BlinkEPrime(color time, blank time)

* Color time - ms color is shown before changing
* Blank time - ms blank is shown

### MorphPrime(color time, blank time)

* Color time - ms color is shown and blended before changing
* Blank time - ms blank is shown

### FadePrime(color time, blank time, direction)

* Color time - ms color is shown before changing
* Blank time - ms blank is shown
* Direction - 0 for fade in, 1 for fade out

## Examples

### SingleMode()

```
// DECLARE MODES HERE. Palettes go in setupModes()
SingleMode mode0 = SingleMode();          // Create a single mode called mode0.
StrobePrime prime00 = StrobePrime(2, 10); // Create DOPS-like prime called prime00.

<snip>

// SETUP MODES HERE
void setupModes() {
    prime00.num_colors = 3;           // We're going to use 3 colors.
                                      // Always set this first.
    prime00.palette[0] = 0x16;        // Look up colors in modes.cpp.
                                      // 0x16 is blue with a little green in it.
    prime00.palette[1] = 0x1f + 0x80; // 0x1f is red with a hint of blue.
                                      // + 0x80 dims the shade 2 levels.
    prime00.palette[2] = 0x00;        // 0x00 (and 0x40, 0x80, 0xc0) indicates blank.
    mode0.prime = &prime00;           // Point mode0.prime to prime00.
    mode0.reset();                    // Last step after setting up mode.

    <snip>
```

### DualMode(trigger type, sensitivity)

* Trigger type - What type of accelerometer action kicks off the change?
  * A_SPEED - Moving too quickly changes variant. Recommended sensitivity is 0.5 or greater for quick changes and less for slow changes.
  * A_TILTX - Starts with the down variant and changes when your fingers point up. Changes back when you point your fingers down. Recommended sensitivity is 0.05 or lower.
  * A_TILTY - Starts with the left variant and changes when your hand tilts to the right. Changes back when your hand points to the left. Recommended sensitivity is 0.05 or lower.
  * A_TILTZ - Starts with the button up variant and changes when the button points down. Changes back when the button points up. Recommended sensitivity is 0.05 or lower.
* Sensitivity - The lower the less sensitive to change the accelerometer is.

```
// DECLARE MODES HERE. Palettes go in setupModes()
DualMode mode0 = DualMode(A_SPEED, 0.9);  // Create a speed dual mode called mode0.
                                          // 0.9 is a high sensitivity setting.
BlinkEPrime prime00 = BlinkEPrime(3, 23); // Create a blink-e prime called prime00.
                                          // This is your slow variant.
MorphPrime prime01 = MorphPrime(17, 17);  // Create a morph hyperstrobe called prime01.
                                          // This is your fast variant.

<snip>

// SETUP MODES HERE
void setupModes() {
    prime00.num_colors = 3;           // We're going to use 3 colors.
                                      // Always set this first.
                                      // Look up colors in modes.cpp.
    prime00.palette[0] = 0x16;        // 0x16 is blue with a little green in it.
    prime00.palette[1] = 0x1a;        // 0x1a is purple.
    prime00.palette[2] = 0x11;        // 0x11 is green with some blue.

    prime01.num_colors = 3;           // We're just going to reuse the same palette.
    prime01.palette[0] = 0x16;
    prime01.palette[1] = 0x1a;
    prime01.palette[2] = 0x11;

    mode0.prime[0] = &prime00;        // Point mode0.prime[0] to prime00.
    mode0.prime[1] = &prime01;        // Point mode0.prime[1] to prime01.
    mode0.reset();                    // Last step after setting up mode.

    <snip>
```

### TriMode(axis, sensitivity)

* Trigger type - Either A_TILTX or A_TILTY. Changes when pointing up or down (left or right) and back when above or below flat again.
* Sensitivity - Recommended 0.05 or lower.

```
// DECLARE MODES HERE. Palettes go in setupModes()
TriMode mode0 = TriMode(A_TILTX, 0.05);         // Create a TriMode using the X axis for switching.
                                                // 0.05 is a low sensitivity setting.
TracerPrime prime00 = TracerPrime(3, 23, 0xc8); // Create a tracer prime called prime00.
                                                // 0xc8 is the dimmest red setting.
                                                // This is your middle variant.
TracerPrime prime00 = TracerPrime(3, 23, 0xd0); // Create a tracer prime called prime01.
                                                // 0xd0 is the dimmest green setting.
                                                // This is your up variant.
TracerPrime prime00 = TracerPrime(3, 23, 0xd8); // Create a tracer prime called prime02.
                                                // 0xd8 is the dimmest blue setting.
                                                // This is your down variant.
<snip>

// SETUP MODES HERE
void setupModes() {
    prime00.num_colors = 2;           // We're going to use 2 colors.
                                      // Always set this first.
                                      // Look up colors in modes.cpp.
    prime00.palette[0] = 0x10;        // 0x10 is green.
    prime00.palette[1] = 0x18;        // 0x18 is blue.

    prime01.num_colors = 2;
    prime01.palette[0] = 0x08;        // 0x08 is red.
    prime01.palette[1] = 0x18;

    prime02.num_colors = 2;
    prime02.palette[0] = 0x08;        // 0x08 is red.
    prime02.palette[1] = 0x10;

    mode0.prime[0] = &prime00;        // Point mode0.prime[0] to prime00.
    mode0.prime[1] = &prime01;        // Point mode0.prime[1] to prime01.
    mode0.prime[2] = &prime02;        // Point mode0.prime[2] to prime02.
    mode0.reset();                    // Last step after setting up mode.

    <snip>
```

### TiltMorph(sensitivity)

* Sensitivity - mostly used for smoothing transitions. Recommended 0.05.

```
// DECLARE MODES HERE. Palettes go in setupModes()
TiltMorph mode0 = TiltMorph();  // Create a tiltmorph mode named mode0

<snip>

// SETUP MODES HERE
void setupModes() {
    mode0.reset();  // Be sure to reset even the TiltMorph mode.

    <snip>
```

## Number of Modes

The number of modes can be changed to any number only limited by what modes will fit in memory. To do this, a number of things must happen in ```lumino.ino```:

* The line ```#define NUM_MODES``` must be set
* You must declare each mode (and it's primes) as shown
* You must configure each mode in the setupModes function
* You must add a pointer to the mode to the line beginning ```Mode *modes[NUM_MODES]```
(use & in front of the variable name e.g. &mode13)

## Bundles

The number of bundles are defined in the line ```#define NUM_BUNDLES```. You must have that number of bundles defined in the bundles array.

### Example Bundle Config

```
#define NUM_MODES 8
#define NUM_BUNDLES 4

int bundles[NUM_BUNDLES][NUM_MODES] = {
  {0, 1, 2, 3, 4, 5, 6, 7},         // Bundle with all modes in order
  {3, 4, 5, -1, -1, -1, -1, -1},    // Bundle with only modes 3, 4, and 5
  {0, 7, 0, 3, 0, 2, -1, -1},       // Mode 0 every other mode and modes 7, 3, and 2
  {3, -1, -1, -1, -1, -1, -1, -1},  // Only mode 3. No mode switching.
};
```

# Reverting to NEO

* Open Arduino IDE.
* Select **File** -> **Examples** -> **OSM_NEO..** -> **_NEO..**
* Select **File** -> **Upload**

That's it! Lumino doesn't use EEPROM so your NEO settings will be untouched.
