#include "modes.h"

// idx from 00-1f, add 0x40 for each shading level
// e.g. 0x88 for 2nd dimmest red
// 0xff indicates first unused color
static const uint8_t color_palette[32][3] = {
  // Blank and whites
  {0, 0, 0},        // 0x00
  {255, 255, 255},  // 0x01
  {255, 223, 223},  // 0x02
  {255, 255, 223},  // 0x03
  {223, 255, 223},  // 0x04
  {223, 255, 255},  // 0x05
  {223, 223, 255},  // 0x06
  {255, 223, 255},  // 0x07

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

void morphColor(uint16_t tick, uint16_t morph_time,
                uint8_t r0, uint8_t g0, uint8_t b0,
                uint8_t r1, uint8_t g1, uint8_t b1,
                uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = r0 + (int)(r1 - r0) * (tick / (float)morph_time);
  *g = g0 + (int)(g1 - g0) * (tick / (float)morph_time);
  *b = b0 + (int)(b1 - b0) * (tick / (float)morph_time);
}


void StrobePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Show a color and then blank
  if (tick < color_time) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
}

void StrobePrime::reset() {
  tick = 0;
  cur_color = 0;
}

void StrobePrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void TracerPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Show a color and then the tracer color
  if (tick < color_time) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    unpackColor(tracer_color, &color_r, &color_g, &color_b);
    *r = color_r; *g = color_g; *b = color_b;
  }
}

void TracerPrime::reset() {
  tick = 0;
  cur_color = 0;
}

void TracerPrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void BlinkEPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < (num_colors * color_time)) {
    unpackColor(palette[(tick / color_time)], &color_r, &color_g, &color_b);
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


void MorphPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    uint8_t r0, g0, b0;
    uint8_t r1, g1, b1;

    unpackColor(palette[cur_color], &r0, &g0, &b0);
    unpackColor(palette[(cur_color + 1) % num_colors], &r1, &g1, &b1);
    morphColor(tick, color_time, r0, g0, b0, r1, g1, b1, &color_r, &color_g, &color_b);
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
}

void MorphPrime::reset() {
  tick = 0;
  cur_color = 0;
}

void MorphPrime::incTick() {
  tick++;
  if (tick >= total_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void FadePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    uint8_t rr, gg, bb;
    unpackColor(palette[cur_color], &rr, &gg, &bb);
    if (dir == 0) {
      morphColor(tick, color_time, 0, 0, 0, rr, gg, bb, &color_r, &color_g, &color_b);
    } else {
      morphColor(tick, color_time, rr, gg, bb, 0, 0, 0, &color_r, &color_g, &color_b);
    }
    *r = color_r; *g = color_g; *b = color_b;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
}

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


void SingleMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t rr, gg, bb;
  prime->render(&rr, &gg, &bb);
  *r = rr; *g = gg; *b = bb;
  prime->incTick();
}

void SingleMode::reset() {
  tick = 0;
  prime->reset();
}

void SingleMode::updateAcc(float fxg, float fyg, float fzg) {}


void DualMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t rr, gg, bb;
  prime[cur_variant]->render(&rr, &gg, &bb);
  *r = rr; *g = gg; *b = bb;
  prime[0]->incTick();
  prime[1]->incTick();
}

void DualMode::reset() {
  tick = 0;
  cur_variant = 0;
  acc_counter = 0;
  prime[0]->reset();
  prime[1]->reset();
}

void DualMode::updateAcc(float fxg, float fyg, float fzg) {
  float pitch;

  switch (acc_mode) {
    case A_SPEED:
      maxg = (max(max(abs(fxg), abs(fyg)), abs(fzg)) * 0.05) + (maxg * 0.95);
      if (cur_variant == 0) {
        if (maxg > 1.45) {
          acc_counter++;
        } else {
          acc_counter--;
        }
        if (acc_counter < 0) acc_counter = 0;
        if (acc_counter > 100) {
          cur_variant = 1;
        }
      } else {
        if (maxg > 1.1) {
          acc_counter++;
        } else {
          acc_counter--;
        }
        if (acc_counter <= 0) {
          cur_variant = 0;
        } else if (acc_counter > 200) {
          acc_counter = 200;
        }
      }
      break;

    case A_TILTX:
      pitch = (atan2(fxg, sqrt(fyg * fyg + fzg * fzg)) * 180.0) / M_PI;
      if (cur_variant == 0) {
        cur_variant = pitch < -75;
      } else {
        cur_variant = pitch < 75;
      }
      break;

    case A_TILTY:
      pitch = (atan2(fyg, sqrt(fxg * fxg + fzg * fzg)) * 180.0) / M_PI;
      if (cur_variant == 0) {
        cur_variant = pitch < -75;
      } else {
        cur_variant = pitch < 75;
      }

      break;
    default:  // TILTZ
      if (cur_variant == 0) {
        cur_variant = fzg < -0.9;
      } else {
        cur_variant = fzg < 0.9;
      }
      break;
  }
}


void TriMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Render using the cur_variant and increment the tick of the other anim to
  // keep them in sync.
  uint8_t rr, gg, bb;
  prime[cur_variant]->render(&rr, &gg, &bb);
  *r = rr; *g = gg; *b = bb;
  prime[0]->incTick();
  prime[1]->incTick();
  prime[2]->incTick();
}

void TriMode::reset() {
  tick = 0;
  cur_variant = 0;
  prime[0]->reset();
  prime[1]->reset();
  prime[2]->reset();
}

void TriMode::updateAcc(float fxg, float fyg, float fzg) {
  // Values come in 0-63 and get translated to -32-31
  float pitch;

  if (tilt_axis == A_TILTX) {
    pitch = (atan2(fxg, sqrt(fyg * fyg + fzg * fzg)) * 180.0) / M_PI;
  } else {
    pitch = (atan2(fyg, sqrt(fxg * fxg + fzg * fzg)) * 180.0) / M_PI;
  }

  switch (cur_variant) {
    case 0:
      if (pitch < -75) {
        cur_variant = 1;
      } else if (pitch > 75) {
        cur_variant = 2;
      }
      break;
    case 1:
      if (pitch > 15) {
        cur_variant = 0;
      }
      break;
    default:
      if (pitch < -15) {
        cur_variant = 0;
      }
      break;
  }
}


void TiltMorph::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    uint8_t rr, gg, bb;
    unpackHue(hue + hue_offset, &rr, &gg, &bb);
    *r = rr; *g = gg; *b = bb;
  } else {
    *r = 0; *g = 0; *b = 0;
  }
  tick++;
}

void TiltMorph::updateAcc(float fxg, float fyg, float fzg) {
  float pitch, roll;

  //Roll & Pitch Equations
  roll  = (atan2(-fyg, fzg) * 180.0) / M_PI;
  pitch = ((atan2(fxg, sqrt(fyg * fyg + fzg * fzg)) * 180.0) / M_PI) + 90;

  hue = (roll + 180) * 4.267;
  if (tick >= 60) {
    if (pitch < 15) {
      color_time = 60;
    } else if (pitch < 45) {
      color_time = 50;
    } else if (pitch < 75) {
      color_time = 40;
    } else if (pitch < 105) {
      color_time = 30;
    } else if (pitch < 135) {
      color_time = 20;
    } else if (pitch < 165) {
      color_time = 10;
    } else {
      color_time = 0;
    }
    hue_offset++;
    tick = 0;
  }
}

void TiltMorph::reset() {
  tick = 0;
  hue_offset = 0;
}


void Speeder::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Render using the cur_variant and increment the tick of the other anim to
  // keep them in sync.
  uint8_t rr, gg, bb;
  prime[cur_variant]->render(&rr, &gg, &bb);
  *r = rr; *g = gg; *b = bb;
  prime[0]->incTick();
  prime[1]->incTick();
  prime[2]->incTick();
}

void Speeder::reset() {
  tick = 0;
  cur_variant = 0;
  maxg = 0;
  prime[0]->reset();
  prime[1]->reset();
  prime[2]->reset();
}

void Speeder::updateAcc(float fxg, float fyg, float fzg) {
  float pitch;
  maxg = (max(max(abs(fxg), abs(fyg)), abs(fzg)) * 0.05) + (maxg * 0.95);
  if (maxg > 1.55) {
    acc_counter += 6;
  } else if (maxg > 1.3) {
    acc_counter += 4;
  } else if (maxg > 1.1) {
    acc_counter += 2;
  } else
  if (acc_counter < 0) acc_counter = 0;
  if (cur_variant == 0) {
    if (acc_counter > 1000) {
      cur_variant = 1;
    }
    acc_counter--;
  } else if (cur_variant == 1) {
    if (acc_counter < 500) {
      cur_variant = 0;
    } else if (acc_counter > 3000) {
      cur_variant = 2;
    }
    acc_counter -= 2;
  } else {
    if (acc_counter < 2000) {
      cur_variant = 1;
    } else if (acc_counter > 4000) {
      acc_counter = 4000;
    }
    acc_counter -= 3;
  }
}
