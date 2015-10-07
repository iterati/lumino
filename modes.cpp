#include <OSM_NEOv1-1-4.h>
#include "modes.h"

// Taken from OSM_NEO
int MMA_ar[64] = {
  0,   1,     2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
  16,  17,   18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17,
  -16, -15, -14, -13, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1
};

// idx from 00-1f, add 0x40 for each shading level
// e.g. 0x88 for 2nd dimmest red
// 0xff indicates first unused color
static const uint8_t color_palette[32][3] = {
  // Blank and whites
  {0, 0, 0},        // 0x00
  {255, 255, 255},  // 0x01
  {255, 223, 223},  // 0x02
  {223, 255, 223},  // 0x03
  {223, 223, 255},  // 0x04
  {255, 255, 223},  // 0x05
  {255, 223, 255},  // 0x06
  {223, 255, 255},  // 0x07

  // Red - green
  {255, 0, 0},      // 0x08
  {255, 63, 0},     // 0x09
  {255, 127, 0},    // 0x0a
  {255, 191, 0},    // 0x0b
  {255, 255, 0},    // 0x0c
  {191, 255, 0},    // 0x0d
  {127, 255, 0},    // 0x0e
  {63, 255, 0},     // 0x0f

  // Green - blue
  {0, 255, 0},      // 0x10
  {0, 255, 63},     // 0x11
  {0, 255, 127},    // 0x12
  {0, 255, 191},    // 0x13
  {0, 255, 255},    // 0x14
  {0, 191, 255},    // 0x15
  {0, 127, 255},    // 0x16
  {0, 63, 255},     // 0x17

  // Blue - red
  {0, 0, 255},      // 0x18
  {63, 0, 255},     // 0x19
  {127, 0, 255},    // 0x1a
  {191, 0, 255},    // 0x1b
  {255, 0, 255},    // 0x1c
  {255, 0, 191},    // 0x1d
  {255, 0, 127},    // 0x1e
  {255, 0, 63},     // 0x1f
};

/*
  // Red - green - 127 sat
  {255, 127, 127},  // 0x20
  {255, 159, 127},  // 0x21
  {255, 191, 127},  // 0x22
  {255, 223, 127},  // 0x23
  {255, 255, 127},  // 0x24
  {223, 255, 127},  // 0x25
  {191, 255, 127},  // 0x26
  {159, 255, 127},  // 0x27

  // Green - blue - 127 sat
  {127, 255, 127},  // 0x28
  {127, 255, 159},  // 0x29
  {127, 255, 191},  // 0x2a
  {127, 255, 223},  // 0x2b
  {127, 255, 255},  // 0x2c
  {127, 223, 255},  // 0x2d
  {127, 191, 255},  // 0x2e
  {127, 159, 255},  // 0x2f

  // Blue - green - 127 sat
  {63,  63, 255},   // 0x30
  {111, 63, 255},   // 0x31
  {159, 63, 255},   // 0x32
  {207, 63, 255},   // 0x33
  {255, 63, 255},   // 0x34
  {255, 63, 207},   // 0x35
  {255, 63, 159},   // 0x36
  {255, 63, 111}    // 0x37

  // Super dims
  {64, 64, 64},     // 0x38
  {64, 0, 0},       // 0x39
  {64, 64, 0},      // 0x3a
  {0, 64, 0},       // 0x3b
  {0, 64, 64},      // 0x3c
  {0, 0, 64},       // 0x3d
  {64, 0, 64},      // 0x3e
  {0, 0, 0},        // 0x3f
};
*/

void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t shade = color >> 6;
  uint8_t idx = color & 0b00111111;
  *r = color_palette[idx][0] - ((color_palette[idx][0] >> 2) * shade);
  *g = color_palette[idx][1] - ((color_palette[idx][1] >> 2) * shade);
  *b = color_palette[idx][2] - ((color_palette[idx][2] >> 2) * shade);
}

void StrobePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  }

  // Show a color and then blank
  if (tick < color_time) {
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
  tick++;
}

void StrobePrime::reset() {
  tick = 0;
  cur_color = 0;
  unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
}


void TracerPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  }

  // Show a color and then the tracer color
  if (tick < color_time) {
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = tracer_r; *g = tracer_g; *b = tracer_b;
  }
  tick++;
}

void TracerPrime::reset() {
  tick = 0;
  cur_color = 0;
  unpackColor(tracer_color, &tracer_r, &tracer_g, &tracer_b);
  unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
}


void SingleMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  prime->render(&color_r, &color_g, &color_b);
  *r = color_r; *g = color_g; *b = color_b;
}

void SingleMode::reset() {
  tick = 0;
  prime->reset();
}

void SingleMode::updateAcc(uint8_t x, uint8_t y, uint8_t z) {}


void DualMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Render using the cur_variant and increment the tick of the other anim to
  // keep them in sync.
  if (cur_variant == 0) {
    prime[0]->render(r, g, b);
    prime[1]->tick++;
  } else {
    prime[1]->render(r, g, b);
    prime[0]->tick++;
  }
}

void DualMode::reset() {
  tick = 0;
  acc_counter = 0;
  cur_variant = 0;
  prime[0]->reset();
  prime[1]->reset();
}

void DualMode::updateAcc(uint8_t x, uint8_t y, uint8_t z) {
  // For this mode we only care about the absolute value of the acceleration.
  // Values come in 0-63 and get translated to -32-31 and then absed.
  // Total acc is used for one part of shake calculation (maybe, it's an idea).
  acc_x = abs(MMA_ar[x]); acc_y = abs(MMA_ar[y]); acc_z = abs(MMA_ar[z]);
  uint8_t total_acc = acc_x + acc_y + acc_z;

  switch (acc_mode) {
    case A_SHAKE:
      if (acc_x > 24) acc_counter += acc_x - 24;
      if (acc_y > 24) acc_counter += acc_y - 24;
      if (acc_z > 24) acc_counter += acc_z - 24;
      acc_counter -= 10;
      break;
    case A_TILTX:
      if (acc_x > 18) {
        acc_counter += acc_x - 18;
      } else {
        acc_counter -= 18 - acc_x;
      }
      break;
    case A_TILTY:
      if (acc_y > 18) {
        acc_counter += acc_y - 18;
      } else {
        acc_counter -= 18 - acc_y;
      }
      break;
    case A_TILTZ:
      if (acc_z > 18) {
        acc_counter += acc_z - 18;
      } else {
        acc_counter -= 18 - acc_z;
      }
      break;
    default:
      break;
  }

  // Cap at 0 and 2 * acc_counter
  if (acc_counter < 0) {
    acc_counter = 0;
  } else if (acc_counter > acc_sens + acc_sens) {
    acc_counter = acc_sens + acc_sens;
  }

  // Use variant 1 is counter > sens
  cur_variant = (acc_counter > acc_sens) ? 1 : 0;
}
