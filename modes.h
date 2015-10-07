#ifndef __MODES_H
#define __MODES_H

#include <Arduino.h>

#define A_SHAKE 0
#define A_TILTX 1
#define A_TILTY 2
#define A_TILTZ 3

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
 */
class Prime {
  public:
    Prime() : tick(0), color_r(0), color_g(0), color_b(0) {}

    virtual void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b) {}
    virtual void reset() {}

    uint32_t tick;
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

    uint16_t color_time, tracer_time, total_time;
    uint8_t tracer_color;
    uint8_t palette[8];
    uint8_t num_colors;
    uint8_t cur_color;
    uint8_t tracer_r, tracer_g, tracer_b;
};


// Modes are what we can switch between. Currently there are 2 modes,
// SingleMode and DualMode. SingleMode uses one prime and doesn't use
// the accelerometer. DualMode uses two primes and switches between
// the current variant based on the acc sensitivity
class Mode {
  public:
    Mode() : tick(0), color_r(0), color_g(0), color_b(0), acc_x(0), acc_y(0), acc_z(0) {}

    virtual void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b) {}
    virtual void reset() {}
    virtual void updateAcc(uint8_t x, uint8_t y, uint8_t z) {}

    uint32_t tick;
    uint8_t color_r, color_g, color_b;
    uint8_t acc_x, acc_y, acc_z;
};

class SingleMode : public Mode {
  public:
    SingleMode() {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(uint8_t x, uint8_t y, uint8_t z);

    Prime *prime;
};

class DualMode : public Mode {
  // acc_mode: 0 - shake, 1 - tiltX, 2 - tiltY, 3 - tiltZ
  public:
    DualMode(uint8_t acc_mode, uint16_t acc_sens) :
      Mode(), acc_mode(acc_mode), acc_sens(acc_sens), cur_variant(0) {}

    void render(uint8_t *led_r, uint8_t *led_g, uint8_t *led_b);
    void reset();
    void updateAcc(uint8_t x, uint8_t y, uint8_t z);

    uint8_t acc_mode;
    uint16_t acc_sens;
    int16_t acc_counter;
    uint8_t cur_variant;
    Prime *prime[2];
};

#endif
