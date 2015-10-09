#include <OSM_NEOv1-1-4.h>
#include "modes.h"

// Taken from OSM_NEO
int8_t MMA_ar[64] = {
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

void unpackHue(uint16_t hue, uint8_t *r, uint8_t *g, uint8_t *b) {
  hue %= 1536;
  uint8_t v = hue % 256;
  switch (hue / 256) {
    case 0: *r = 255;     *g = v;        *b = 0;       break;
    case 1: *r = 255 - v; *g = 255;      *b = 0;       break;
    case 2: *r = 0;       *g = 255;      *b = v;       break;
    case 3: *r = 0;       *g = 255 - v;  *b = 255;     break;
    case 4: *r = v;       *g = 0;        *b = 255;     break;
    case 5: *r = 255;     *g = 0;        *b = 255 - v; break;
  }
}

void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t shade = color >> 6;
  uint8_t idx = color & 0b00111111;
  *r = color_palette[idx][0] - ((color_palette[idx][0] >> 2) * shade);
  *g = color_palette[idx][1] - ((color_palette[idx][1] >> 2) * shade);
  *b = color_palette[idx][2] - ((color_palette[idx][2] >> 2) * shade);
}

void StrobePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Show a color and then blank
  if (tick < color_time) {
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
}

void StrobePrime::reset() {
  tick = 0;
  cur_color = 0;
  unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
}

void StrobePrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  }
}


void TracerPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Show a color and then the tracer color
  if (tick < color_time) {
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = tracer_r; *g = tracer_g; *b = tracer_b;
  }
}

void TracerPrime::reset() {
  tick = 0;
  cur_color = 0;
  unpackColor(tracer_color, &tracer_r, &tracer_g, &tracer_b);
  unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
}

void TracerPrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  }
}


void BlinkEPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < (num_colors * color_time)) {
    unpackColor(palette[tick % (num_colors * color_time)], &color_r, &color_g, &color_b);
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
}

void BlinkEPrime::reset() {
  tick = 0;
  total_time = (num_colors * color_time) + blank_time;
}

void BlinkEPrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
  }
}


void SingleMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  prime->render(&color_r, &color_g, &color_b);
  prime->incTick();
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
  prime[cur_variant]->render(r, g, b);
  prime[0]->incTick();
  prime[1]->incTick();
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
  acc_x = abs(MMA_ar[x]); acc_y = abs(MMA_ar[y]); acc_z = abs(MMA_ar[z]);

  switch (acc_mode) {
    case A_SHAKE:
      acc_x = max(max(acc_x, acc_y), acc_z);
      if (acc_x > 24) acc_counter += acc_x;
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
  } else if (acc_counter > acc_sens << 1) {
    acc_counter = acc_sens << 1;
  }

  // Use variant 1 is counter > sens
  cur_variant = (acc_counter > acc_sens) ? 1 : 0;
}


void TiltedMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Render using the cur_variant and increment the tick of the other anim to
  // keep them in sync.
  prime[cur_variant]->render(&color_r, &color_g, &color_b);
  prime[0]->incTick();
  prime[1]->incTick();
  prime[2]->incTick();
  *r = color_r; *g = color_g; *b = color_b;
}

void TiltedMode::reset() {
  tick = 0;
  acc_counter = 0;
  cur_variant = 1;
  prime[0]->reset();
  prime[1]->reset();
  prime[2]->reset();
}

void TiltedMode::updateAcc(uint8_t x, uint8_t y, uint8_t z) {
  // Values come in 0-63 and get translated to -32-31
  int8_t acc;
  if (acc_axis == 0) {
    acc = MMA_ar[x];
  } else if (acc_axis == 1) {
    acc = MMA_ar[y];
  } else {
    acc = MMA_ar[z];
  }

  if (acc > 12) {
    cur_variant = 1;
  } else if (acc < -12) {
    cur_variant = 2;
  } else {
    cur_variant = 0;
  }
}


void TiltMorph::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick > color_time) {
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
  tick++;
}

void TiltMorph::updateAcc(uint8_t x, uint8_t y, uint8_t z) {
  float pitch, roll, xg, yg, zg, alpha;
  alpha = 0.2;
  xg = MMA_ar[x] / 20.0; yg = MMA_ar[y] / 20.0; zg = MMA_ar[z] / 20.0;

  //Low Pass Filter
  fxg = xg * alpha + (fxg * (1.0 - alpha));
  fyg = yg * alpha + (fyg * (1.0 - alpha));
  fzg = zg * alpha + (fzg * (1.0 - alpha));

  //Roll & Pitch Equations
  roll  = (atan2(-fyg, fzg) * 180.0) / M_PI;
  pitch = (atan2(fxg, sqrt(fyg * fyg + fzg*fzg)) * 180.0) / M_PI;

  hue = (roll + 180) * 4.267;
  if (tick >= 40) {
    color_time = (pitch + 80) / 4;
    Serial.print(F("r: "));
    Serial.print(roll);
    Serial.print(F("\tp: "));
    Serial.print(pitch);
    Serial.print(F("\tcolor_time: "));
    Serial.print(color_time);
    Serial.println();
    tick = 0;
  }

  unpackHue(hue, &color_r, &color_g, &color_b);
}

void TiltMorph::reset() {
  tick = 0;
  fxg = fyg = fzg = 0.0;
}
