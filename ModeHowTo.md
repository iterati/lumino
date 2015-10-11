# HowTo Make Your Own Prime

This is an advanced tutorial. I will try to keep the geek speak to a mininum, but I can't promise much as this is a programming tutorial. We're going to make a versatile Fade Prime that fades in or out over color_time and then is blank for blank_time.

## STEP 1: Define the Prime in modes.h

Our FadePrime class is basically a strobe, so we'll just copy over the StrobePrime class and rename all instances of Strobe with Fade.

```
class FadePrime : public Prime {
  public:
    FadePrime(uint16_t color_time, uint16_t blank_time) :
      Prime(), color_time(color_time), blank_time(blank_time),
      total_time(color_time + blank_time), num_colors(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void incTick();

    uint16_t color_time, blank_time, total_time;
    uint8_t palette[8];
    uint8_t num_colors;
    uint8_t cur_color;
};
```

Since we want to allow fading in and fading out, we need some way to define that. We could make 2 classes, but that's wasteful. Let's add a "dir" argument that allows us to define the direction. Let's say 0 is fade in and 1 is fade out. So we'll need to add that. 2 values fits into a byte, but I use uint8_t out of habit. Feel free to use "byte", "unsigned char", and "uint8_t" interchangably, but stick to the one you choose! It could also be a boolean flag, but this is more flexible if we want to add other fade patterns latter.

```
class FadePrime : public Prime {
  public:
    FadePrime(uint16_t color_time, uint16_t blank_time, uint8_t dir) :
      Prime(), color_time(color_time), blank_time(blank_time), dir(dir),
      total_time(color_time + blank_time), num_colors(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void incTick();

    uint16_t color_time, blank_time, total_time;
    uint8_t palette[8];
    uint8_t num_colors;
    uint8_t cur_color;
    uint8_t dir;
};
```

Notice the changes in the FadePrime(... line and the one below as well as the added "uint8_t dir;" line at the bottom. Now that the class is defined, let's make it do stuff.


## STEP 2: The render method

Copying StrobePrime's render method only gets us some of the way.

```
void FadePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    uint8_t rr, gg, bb;
    unpackColor(palette[cur_color], &rr, &gg, &bb);
    *r = rr; *g = gg; *b = bb;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
}
```

```
  if (tick < color_time) {
    uint8_t rr, gg, bb;
    unpackColor(palette[cur_color], &rr, &gg, &bb);

    if (dir == 0) {
    } else {
    }
```

This if/else block will allow us to do 2 different things depending on the dir we have set. So now we need to do the math for each of these settings. For fading in we're going to want to use similar math to morph, but we'll be fading from 0 to our color rather than from one color to the next.

```
  if (tick < color_time) {
    uint8_t rr, gg, bb;
    float dr, dg, db;
    unpackColor(palette[cur_color], &rr, &gg, &bb);

    if (dir == 0) {
      dr = 0 + (tick * ((float)rr / (float)color_time));
      dg = 0 + (tick * ((float)gg / (float)color_time));
      db = 0 + (tick * ((float)bb / (float)color_time));
    } else if (dir == 1) {
      dr = rr - (tick * ((float)rr / (float)color_time));
      dg = gg - (tick * ((float)gg / (float)color_time));
      db = bb - (tick * ((float)bb / (float)color_time));
    } else {

    *r = dr; *g = dg; *b = db;
  } else {
```

So first we need floats to hold these values since we're dividing and ints can only hold whole numbers. For fading in we go from 0 to rr, etc, over color_time steps. For fading out we go the other way. Let's finish it off.


## STEP 3: reset and incTick methods

Our copied and renamed StrobePrime leftovers are:

```
void FadePrime::reset() {
  tick = 0;
  cur_color = 0;
}

void FadePrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}
```

And that's actually enough. There's nothing else to reset, nothing else to incTick. Try changing a prime in lumino.ino to FadePrime and see how it goes!
