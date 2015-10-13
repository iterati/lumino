#ifndef __MODES_H
#define __MODES_H

#include <Arduino.h>

#define A_SPEED 0
#define A_TILTX 1
#define A_TILTY 2
#define A_TILTZ 3

void unpackHue(uint16_t hue, uint8_t *r, uint8_t *g, uint8_t *b);
void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b);

/* Primes are used for the current two modes to define the animation. There are
 * currently 2 primes: strobe and tracer.
 *
 * Strobe takes color_time and blank_time arguments. Requires num_colors set
 * to the highest palette idx to use. Some popular timings are:
 *   - Strobe: 5, 8
 *   - Hyperstrobe: 17, 17
 *   - DOPS: 2, 10 (real DOPS uses 1.5, 10, but until I support half-ms it's 2)
 *   - Strobie: 3, 23
 *   - Chroma: 11
 *
 * Tracer takes color_time, tracer_time, and tracer_color arguments. Requires
 * num_colors set to the highest palette idx to use.
 *
 * BlinkE takes color_time and blank_time. Each color is shown for color_time
 * which is followed by blink_time.
 */
class Prime {
  public:
    Prime() : tick(0), color_r(0), color_g(0), color_b(0) {}

    virtual void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b) {}
    virtual void reset() {}
    virtual void incTick() {}

    uint16_t tick;
    uint8_t color_r, color_g, color_b;
};

// Popular timings
// Strobe; 5, 8
// Hyperstrobe: 17, 17
// DOPS: 1.5 (use 2), 10
// Strobie: 3, 23
// Chroma: 11, 0
class StrobePrime : public Prime {
  public:
    StrobePrime(uint16_t color_time, uint16_t blank_time) :
      Prime(), color_time(color_time), blank_time(blank_time),
      total_time(color_time + blank_time), num_colors(0), cur_color(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void incTick();

    uint16_t color_time, blank_time, total_time;
    uint8_t palette[8];
    uint8_t num_colors;
    uint8_t cur_color;
};

class TracerPrime : public Prime {
  public:
    TracerPrime(uint16_t color_time, uint16_t tracer_time, uint8_t tracer_color) :
      Prime(), color_time(color_time), tracer_time(tracer_time), tracer_color(tracer_color),
      total_time(color_time + tracer_time), num_colors(0), cur_color(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void incTick();

    uint16_t color_time, tracer_time, total_time;
    uint8_t tracer_color;
    uint8_t palette[8];
    uint8_t num_colors;
    uint8_t cur_color;
};

class BlinkEPrime : public Prime {
  public:
    BlinkEPrime(uint16_t color_time, uint16_t blank_time) :
      Prime(), color_time(color_time), blank_time(blank_time), total_time(0), num_colors(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void incTick();

    uint16_t color_time, blank_time, total_time;
    uint8_t palette[8];
    uint8_t num_colors;
};

class MorphPrime : public Prime {
  public:
    MorphPrime(uint16_t color_time, uint16_t blank_time) :
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

class RainbowPrime : public Prime {
  public:
    RainbowPrime(uint16_t color_time, uint8_t blank_time,
        uint16_t splits, uint16_t split_dist, uint16_t speed) :
      Prime(), color_time(color_time), blank_time(blank_time), total_time(color_time + blank_time),
      splits(splits), split_dist(split_dist), speed(speed) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void incTick();

    uint16_t color_time, blank_time, total_time;
    uint16_t splits, split_dist, speed;
    uint16_t split, hue_tick;
    uint16_t hue;
};

// Modes are what we can switch between. Currently there are 2 modes, SingleMode
// and DualMode. SingleMode uses one prime and doesn't use the accelerometer.
// DualMode uses two primes and switches between the current variant based on
// the acc sensitivity.
class Mode {
  public:
    Mode(float alpha) : alpha(alpha), tick(0), color_r(0), color_g(0), color_b(0) {}

    virtual void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b) {}
    virtual void reset() {}
    virtual void updateAcc(float fxg, float fyg, float fzg);

    float alpha;
    uint32_t tick;
    uint8_t color_r, color_g, color_b;
};

class SingleMode : public Mode {
  public:
    SingleMode() : Mode(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(float fxg, float fyg, float fzg);

    Prime *prime;
};

class DualMode : public Mode {
  // acc_mode: 0 - tiltX, 1 - tiltY, 2 - tiltZ, 3 - shake
  public:
    DualMode(uint8_t acc_mode, float alpha) : Mode(alpha), acc_mode(acc_mode), cur_variant(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(float fxg, float fyg, float fzg);

    uint8_t acc_mode;
    uint8_t cur_variant;
    int16_t acc_counter;
    Prime *prime[2];
    float maxg;
};


class TriMode : public Mode {
  public:
    TriMode(uint8_t tilt_axis, float alpha) : Mode(alpha), tilt_axis(tilt_axis) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(float fxg, float fyg, float fzg);

    uint8_t tilt_axis;
    uint8_t cur_variant;
    Prime *prime[3];
};

class TiltMorph : public Mode {
  public:
    TiltMorph(float alpha) : Mode(alpha) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(float fxg, float fyg, float fzg);

    uint16_t hue, hue_offset;
    uint8_t color_time;
};

class Speeder : public Mode {
  public:
    Speeder(float alpha) : Mode(alpha) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(float fxg, float fyg, float fzg);

    uint8_t cur_variant;
    int16_t acc_counter;
    float maxg;
    Prime *prime[3];
};

#endif
