# Lumino

Lumino is an open and straightforward framework for the Open Source Microlight.
The goal of this framework is to be flexible enough to allow for modes of
differing structure. Live configuration of modes is not planned, so some
familiarity with the code is required. The code is structured in such a way that
configuration is simple enough for newbies and experienced developers alike.

Current Features:
* 4 Modes (SingleMode, DualMode, TriMode, and TiltMorph)
* 4 Primes (Strobe, Tracer, Blink-E, and Morph) with 1 to 8 color palettes
* 32 (31 + blank) Color palette with 4 shades per color
* Conjuring mode


# Configuration

Currently all configuration must be done in code. You can configure the color
palette and the modes you would like to use. The number of modes is entirely up
to you and how much memory is available on the chip.

## Palette Config

You can configure your palette by editing the ```color_palette``` array located
at the top of ```modes.cpp```. Values from 0 to 255 will work with 255 being
full power. Lumino uses a gamma-curved palette to provide a more relative
brightness. This means that a palette value of (64, 0, 0) should be red and
about half as bright as (128, 0, 0).

## Mode Config

Each mode will have it's own configuration options. The best way to know what is
configurable on a mode is to look at the header file ```modes.h```. Modes and
their documentation are at the end of the file.  There are currently 3 modes:

* SingleMode - Uses a single prime with no accelerometer magic.
* DualMode - Can be configured to switch between primes based on speed or tilt
along the X, Y, or Z axes.
* Tilted - Can be configured to switch between three primes based on tilt along
the X or Y axes.
* TiltMorph - Hue slowly cycles along the color wheel. The light's roll alters
the hue and the pitch changes the strobe timings.

## Prime Config

Some modes will use animation primes as a base for the mode. Primes, like Modes,
are defined and documented in ```modes.h```. Primes should be at the top of the
file above the modes. There are currently 3 Primes:

* Strobe - On/off through up to an 8 color palette
* Tracer - Tracer/color through up to an 8 color palette (plus tracer color)
* Blink-E - Color/color/color../blank through up to an 8 color palette
* Morph - Color blends to next color/blank time through an up to 8 color palette

## Number of Modes

The number of modes can be changed to any number only limited by what modes will
fit in memory. To do this, a number of things must happen in ```lumino.ino```:

* The line ```#define NUM_MODES``` must be set
* You must declare each mode (and it's primes) as shown
* You must configure each mode in the setupModes function
* You must add a pointer to the mode to the line beginning ```Mode *modes[NUM_MODES]```
(use & in front of the variable name e.g. &mode13)

## Turning the light off

Hold the button in for 1s, until the light flashes white, and upon release, the
light will turn off.

## Conjuring Mode

To enable conjuring mode, hold the button in for 3s until the light flashes
blue. Upon release, conjuring mode will be enabled and the light will turn on
and off with a single press. To reenable normal use, hold the button in for 3s
again. Turning the light off in conjuring mode doesn't turn the light off, it
just turns off the bulb. To turn off the light, hold it in for 1s until the
light flashes white. Turning the light off will disble conjuring mode.


# Example Configuration

For this example we're going to make a Tilted mode where one of three colors
will go missing depending on which prime we're in. We'll use mode3 for this. All
of the code we need goes into ```lumino.ino```.

First declare the mode and it's primes.
```
/* For this part, replace these 2 lines with the code below:
SingleMode mode3 = SingleMode();
StrobePrime prime30 = StrobePrime(5, 10);
*/

// Declare the mode. TiltedMode takes two arguments - axis and sensitivity. We
// Want the colors to change when we point our fingers up and down, so we'll use
// 0 for the X axis.
TiltedMode mode3 = TiltedMode(0, 200);

// Decalre the three primes. We'll be using Strobe with Hyperstrobe-like timings.
StrobePrime prime30 = StrobePrime(17, 17);
StrobePrime prime31 = StrobePrime(17, 17);
StrobePrime prime32 = StrobePrime(17, 17);
```

Now configure the mode in stupModes.
```
/* For this part, replace these 5 lines with the code below:
  prime30.num_colors = 1;
  prime30.palette[0] = 0x08;
  prime30.reset();
  mode3.prime = &prime30;
  mode3.reset();
*/

// Initialize prime30. This is when our hand is level. We'll be using yellow
// (0x0c), cyan (0x14), and magenta (0x1c) with one color not used in each set.
// We'll use the 3rd brightest (2nd dimmest) shade for the colors.
// Let's start with yellow and magenta and dropping cyan.
prime30.num_colors = 3;
prime30.palette[0] = 0x0c + 0x80; // yellow
prime30.palette[1] = 0x00 + 0x80; // blank
prime30.palette[2] = 0x1c + 0x80; // magenta

// Initialize prime31. Same palette, but we're going to drop magenta this time.
prime30.num_colors = 3;
prime30.palette[0] = 0x0c + 0x80; // yellow
prime30.palette[1] = 0x14 + 0x80; // cyan
prime30.palette[2] = 0x00 + 0x80; // blank

// Add the primes to the mode.
mode3.primes[0] = prime30;
mode3.primes[1] = prime31;
mode3.primes[2] = prime32;

// Finally reset the mode to make everything ready to go
mode3.reset();
```

Now upload the software, switch to the 4th mode (everything starts with 0 in
computing), and enjoy your new creation.
