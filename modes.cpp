#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "modes.h"

uint8_t color_r, color_g, color_b;

// idx from 00-1f, add 0x40 for each shading level
// e.g. 0x88 for 2nd dimmest red
// 0xff indicates first unused color
const PROGMEM uint8_t color_palette[64][3] = {
  // Blank and whites
  {0, 0, 0},        // 0x00
  {255, 255, 255},  // 0x01
  {255, 191, 191},  // 0x02
  {255, 255, 191},  // 0x03
  {191, 255, 191},  // 0x04
  {191, 255, 255},  // 0x05
  {191, 191, 255},  // 0x06
  {255, 191, 255},  // 0x07

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

  // Red - green saturated
  {255, 64,  64},   // 0x20
  {255, 112, 64},   // 0x21
  {255, 160, 64},   // 0x22
  {255, 208, 64},   // 0x23
  {255, 255, 64},   // 0x24
  {208, 255, 64},   // 0x25
  {160, 255, 64},   // 0x26
  {112, 255, 64},   // 0x27

  // Green - blue saturated
  {64, 255, 64},    // 0x28
  {64, 255, 112},   // 0x29
  {64, 255, 160},   // 0x2a
  {64, 255, 208},   // 0x2b
  {64, 255, 255},   // 0x2c
  {64, 208, 255},   // 0x2d
  {64, 160, 255},   // 0x2e
  {64, 112, 255},   // 0x2f

  // Blue - red saturated
  {64,  64, 255},   // 0x30
  {112, 64, 255},   // 0x31
  {160, 64, 255},   // 0x32
  {208, 64, 255},   // 0x33
  {255, 64, 255},   // 0x34
  {255, 64, 208},   // 0x35
  {255, 64, 160},   // 0x36
  {255, 64, 112},   // 0x37

  // Dim colors
  {32, 32, 32},     // 0x38
  {32, 0, 0},       // 0x39
  {32, 32, 0},      // 0x3a
  {0, 32, 0},       // 0x3b
  {0, 32, 32},      // 0x3c
  {0, 0, 32},       // 0x3d
  {32, 0, 32},      // 0x3e
};

void unpackHue(uint16_t hue, uint8_t *r, uint8_t *g, uint8_t *b) {
  hue %= 1536;            // Normalize hue
  uint8_t v = hue % 256;  // Get arc distance inside hexant
  switch (hue / 256) {
    case 0: *r = 255;     *g = v;        *b = 0;       break; // red -> yellow hexant
    case 1: *r = 255 - v; *g = 255;      *b = 0;       break; // yellow -> green hexant
    case 2: *r = 0;       *g = 255;      *b = v;       break; // green -> cyan hexant
    case 3: *r = 0;       *g = 255 - v;  *b = 255;     break; // cyan -> blue hexant
    case 4: *r = v;       *g = 0;        *b = 255;     break; // blue -> magenta hexant
    case 5: *r = 255;     *g = 0;        *b = 255 - v; break; // magenta -> red hexant
  }
}

void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t shade = color >> 6;                                   // shade is first 2 bits
  uint8_t idx = color & 0b00111111;                             // palette index is last 6 bits
  *r = pgm_read_byte(&color_palette[idx][0]); *r = *r >> shade; // get red value and shade
  *g = pgm_read_byte(&color_palette[idx][1]); *g = *g >> shade; // get green value and shade
  *b = pgm_read_byte(&color_palette[idx][2]); *b = *b >> shade; // get blue value and shade
}

void morphColor(uint16_t tick, uint16_t morph_time,
                uint8_t r0, uint8_t g0, uint8_t b0,
                uint8_t r1, uint8_t g1, uint8_t b1,
                uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = r0 + (int)(r1 - r0) * (tick / (float)morph_time);
  *g = g0 + (int)(g1 - g0) * (tick / (float)morph_time);
  *b = b0 + (int)(b1 - b0) * (tick / (float)morph_time);
}

void Prime::renderColor(uint8_t *r, uint8_t *g, uint8_t *b) {
  unpackColor(palette[edit_color], &color_r, &color_g, &color_b);
  *r = color_r; *g = color_g; *b = color_b;
}

int8_t Prime::incIdx(int8_t v) {
  uint8_t rtn_code = 0;
  if (v < 0) {                                // When decrementing, return -1 if at beginning
    if (edit_color == 0) {
      rtn_code = -1;
    } else {                                  // Otherwise return 0 and dec color
      edit_color--;
    }
  } else if (v > 0) {                         // When incrementing, return -1 if at end
    if (edit_color == PALETTE_SIZE - 1) {
      rtn_code = -1;
    } else {                                  // Otherwise return 0 and inc color
      edit_color++;
    }
  } else if (v == 0) {                        // 0 is resetting
    edit_color = 0;
  }
  Serial.print(F("editing color ")); Serial.println(edit_color + 1);
  num_colors = edit_color + 1;
  return rtn_code;
}

void Prime::incColor(int8_t v) {
  uint8_t idx = palette[edit_color] & 0b00111111;
  palette[edit_color] = (palette[edit_color] & 0b11000000) + ((idx + v + 63) % 63);
}

void Prime::incShade() {
  palette[edit_color] += 0b01000000;
}

void Prime::save(uint16_t addr) {
  EEPROM.update(addr, num_colors);
  for (uint8_t i = 0; i < PALETTE_SIZE; i++) {
    EEPROM.update(addr + 2 + i, palette[i]);
  }
}

void Prime::load(uint16_t addr) {
  num_colors = EEPROM.read(addr);
  for (uint8_t i = 0; i < PALETTE_SIZE; i++) {
    palette[i] = EEPROM.read(addr + 2 + i);
  }
}


void StrobePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void StrobePrime::reset() {
  tick = 0;
  cur_color = 0;
}

void StrobePrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void TracerPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  // Show a color and then the tracer color
  if (tick < color_time) {
    unpackColor(palette[(cur_color + 1) % num_colors], &color_r, &color_g, &color_b);
  } else {
    unpackColor(palette[0], &color_r, &color_g, &color_b);
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void TracerPrime::reset() {
  tick = 0;
  cur_color = 0;
}

void TracerPrime::incTick() {
  tick++;
  if (tick >= color_time + tracer_time) {
    tick = 0;
    cur_color = (cur_color + 1) % (num_colors - 1);
  }
}


void BlinkEPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < (num_colors * color_time)) {
    unpackColor(palette[(tick / color_time)], &color_r, &color_g, &color_b);
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void BlinkEPrime::reset() {
  tick = 0;
}

void BlinkEPrime::incTick() {
  tick++;
  if (tick >= (num_colors * color_time) + blank_time) {
    tick = 0;
  }
}


void MorphPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    uint8_t r0, g0, b0;
    uint8_t r1, g1, b1;

    unpackColor(palette[cur_color],                     &r0, &g0, &b0);
    unpackColor(palette[(cur_color + 1) % num_colors],  &r1, &g1, &b1);

    morphColor(tick, color_time,        // morph
        r0, g0, b0,                     // from 0
        r1, g1, b1,                     // to 1
        &color_r, &color_g, &color_b);  // store here

  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void MorphPrime::reset() {
  tick = 0;
  cur_color = 0;
}

void MorphPrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void FadePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  uint16_t half_time = color_time >> 1;
  if (tick < color_time) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
    if (dir == 0) {
      morphColor(tick, color_time,
          0, 0, 0,
          color_r, color_g, color_b,
          &color_r, &color_g, &color_b);
    } else if (dir == 1) {
      morphColor(tick, color_time,
          color_r, color_g, color_b,
          0, 0, 0,
          &color_r, &color_g, &color_b);
    } else {
      if (tick < half_time) {
        morphColor(tick, half_time,
            0, 0, 0,
            color_r, color_g, color_b,
            &color_r, &color_g, &color_b);
      } else {
        morphColor(tick - half_time, color_time - half_time,
            color_r, color_g, color_b,
            0, 0, 0,
            &color_r, &color_g, &color_b);
      }
    }
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void FadePrime::reset() {
  tick = 0;
  cur_color = 0;
}

void FadePrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void CandyStrobePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    unpackColor(palette[(cur_color + cur_batch) % num_colors], &color_r, &color_g, &color_b);
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void CandyStrobePrime::reset() {
  tick = 0;
  cur_color = 0;
  cur_batch = 0;
}

void CandyStrobePrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    cur_batch++;
    if (cur_batch >= batch) {
      repeat++;
      if (repeat >= repeats) {
        cur_color = (cur_color + 1) % num_colors;
        repeat = 0;
      }
      cur_batch = 0;
    }
  }
}


void ChasePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t anim_step = steps * (tick / (float)color_time);
  if (tick < color_time) {
    if (cur_step == 0) {
      unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
    } else {
      if (anim_step < cur_step) {
        unpackColor(palette[(cur_color + 1) % num_colors], &color_r, &color_g, &color_b);
      } else if (anim_step == cur_step) {
        color_r = 0; color_g = 0; color_b = 0;
      } else {
        unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
      }
    }
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void ChasePrime::reset() {
  tick = 0;
  cur_color = 0;
  cur_step = 0;
}

void ChasePrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    cur_step++;
    if (cur_step >= steps - 1) {
      cur_color = (cur_color + 1) % num_colors;
      cur_step = 0;
    }
  }
}


void TwoTimePrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time_a) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  } else if (tick < color_time_a + blank_time_a) {
    color_r = 0; color_g = 0; color_b = 0;
  } else if (tick < color_time_a + blank_time_a + color_time_b) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void TwoTimePrime::reset() {
  tick = 0;
  cur_color = 0;
}

void TwoTimePrime::incTick() {
  tick++;
  if (tick >= color_time_a + blank_time_a + color_time_b + blank_time_b) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }
}


void LegoPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

uint8_t getColorTime() {
  switch (random(0, 3)) {
    case 0:
      return 2;
    case 1:
      return 8;
    default:
      return 16;
  }
}

void LegoPrime::reset() {
  tick = 0;
  cur_color = 0;
  color_time = getColorTime();
}

void LegoPrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
    color_time = getColorTime();
  }
}


void EmberPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < (color_time - extra_blank)) {
    unpackColor(palette[cur_color], &color_r, &color_g, &color_b);
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void EmberPrime::reset() {
  tick = 0;
  cur_color = 0;
  extra_blank = 0;
  dir = speed;
}

void EmberPrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    extra_blank += dir;
    if (extra_blank >= color_time) {
      dir = -speed;
    } else if (extra_blank <= 0) {
      dir = speed;
      cur_color = (cur_color + 1) % num_colors;
    }
  }
}


void RainbowPrime::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (tick < color_time) {
    unpackHue(hue + (split * split_dist), &color_r, &color_g, &color_b);
    color_r = color_r >> 1; color_g = color_g >> 1; color_b = color_b >> 1;
  } else {
    color_r = 0; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void RainbowPrime::reset() {
  tick = 0;
  hue = 0;
  split = 0;
  hue_tick = speed;
}

void RainbowPrime::incTick() {
  tick++;
  if (tick >= color_time + blank_time) {
    tick = 0;
    split = (split + 1) % splits;
  }
  hue_tick--;
  if (hue_tick == 0) {
    hue = (hue + 1) % 1536;
    hue_tick = speed;
  }
}

//********************************************************************
//*** SINGLE MODE ****************************************************
//********************************************************************
void SingleMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (render_mode == 0 || render_mode == 3) {
    prime->render(&color_r, &color_g, &color_b);
    prime->incTick();
  } else if (render_mode == 1) {
    if (prime->canConfig()) {
      if (tick < 50) {
        color_r = 255; color_g = 128; color_b = 0;
      } else {
        color_r = 0; color_g = 0; color_b = 0;
      }
    } else {
      color_r = 255; color_g = 0; color_b = 0;
    }
    tick++;
    if (tick >= 100) { tick = 0; }
  } else if (render_mode == 2) {
    prime->renderColor(&color_r, &color_g, &color_b);
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void SingleMode::reset() {
  tick = 0;
  prime->reset();
  render_mode = 0;
}

void SingleMode::save(uint16_t addr) {
  prime->save(addr);
}

void SingleMode::load(uint16_t addr) {
  prime->load(addr);
}

void SingleMode::updateAcc(float fxg, float fyg, float fzg) {}
void SingleMode::nextPalette() {}
int8_t SingleMode::incIdx(int8_t v) { return prime->incIdx(v); }
void SingleMode::incColor(int8_t v) { prime->incColor(v); }
void SingleMode::incShade() { prime->incShade(); }


//********************************************************************
//*** DUAL MODE ******************************************************
//********************************************************************
void DualMode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (render_mode == 0 || render_mode == 3) {
    prime[cur_variant]->render(&color_r, &color_g, &color_b);
    prime[0]->incTick();
    prime[1]->incTick();
  } else if (render_mode == 1) {
    if (prime[cur_variant]->canConfig()) {
      if (tick < 50) {
        if (cur_variant == 0) {
          color_r = 255; color_g = 255; color_b = 0;
        } else {
          color_r = 0; color_g = 255; color_b = 0;
        }
      } else {
        color_r = 0; color_g = 0; color_b = 0;
      }
    } else {
      color_r = 255; color_g = 0; color_b = 0;
    }
    tick++;
    if (tick >= 100) { tick = 0; }
  } else if (render_mode == 2) {
    prime[cur_variant]->renderColor(&color_r, &color_g, &color_b);
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void DualMode::reset() {
  tick = 0;
  acc_counter = 0;
  cur_variant = 0;
  prime[0]->reset();
  prime[1]->reset();
  render_mode = 0;
}

void DualMode::updateAcc(float fxg, float fyg, float fzg) {
  float pitch;
  if (render_mode != 0) {
    return;
  }

  switch (acc_mode) {
    case A_SPEED:
      maxg = max(max(abs(fxg), abs(fyg)), abs(fzg));
      if (acc_counter < 0) acc_counter = 0;
      if (cur_variant == 0) {
        if (maxg > 1.5) {
          acc_counter += 3;
        } else if (maxg > 1.35) {
          acc_counter++;
        } else {
          acc_counter--;
        }
        if (acc_counter > 100) {
          cur_variant = 1;
        }
      } else {
        if (maxg > 1.35) {
          acc_counter += 3;
        } else if (maxg > 1.1) {
          acc_counter++;
        } else {
          acc_counter--;
        }
        if (acc_counter <= 0) {
          cur_variant = 0;
        } else if (acc_counter > 125) {
          acc_counter = 125;
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

void DualMode::save(uint16_t addr) {
  prime[0]->save(addr);
  prime[1]->save(addr + 10);
}

void DualMode::load(uint16_t addr) {
  prime[0]->load(addr);
  prime[1]->load(addr + 10);
}

void DualMode::nextPalette() { cur_variant = (cur_variant + 1) % 2; }
int8_t DualMode::incIdx(int8_t v) { return prime[cur_variant]->incIdx(v); }
void DualMode::incColor(int8_t v) { prime[cur_variant]->incColor(v); }
void DualMode::incShade() { prime[cur_variant]->incShade(); }


//********************************************************************
//*** TRITILT MODE ***************************************************
//********************************************************************
void TriTilt::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (render_mode == 0 || render_mode == 3) {
    prime[cur_variant]->render(&color_r, &color_g, &color_b);
    prime[0]->incTick();
    prime[1]->incTick();
    prime[2]->incTick();
  } else if (render_mode == 1) {
    if (prime[cur_variant]->canConfig()) {
      if (tick < 50) {
        if (cur_variant == 0) {
          color_r = 0; color_g = 255; color_b = 255;
        } else if (cur_variant == 1) {
          color_r = 0; color_g = 0; color_b = 255;
        } else {
          color_r = 255; color_g = 0; color_b = 255;
        }
      } else {
        color_r = 0; color_g = 0; color_b = 0;
      }
    } else {
      color_r = 255; color_g = 0; color_b = 0;
    }
    tick++;
    if (tick >= 100) {
      tick = 0;
    }
  } else if (render_mode == 2) {
    // display color
    prime[cur_variant]->renderColor(&color_r, &color_g, &color_b);
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void TriTilt::reset() {
  tick = 0;
  cur_variant = 0;
  prime[0]->reset();
  prime[1]->reset();
  prime[2]->reset();
  render_mode = 0;
}

void TriTilt::updateAcc(float fxg, float fyg, float fzg) {
  float pitch;
  if (render_mode != 0) {
    return;
  }

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

void TriTilt::save(uint16_t addr) {
  prime[0]->save(addr);
  prime[1]->save(addr + 10);
  prime[2]->save(addr + 20);
}

void TriTilt::load(uint16_t addr) {
  prime[0]->load(addr);
  prime[1]->load(addr + 10);
  prime[2]->load(addr + 20);
}

void TriTilt::nextPalette() { cur_variant = (cur_variant + 1) % 3; }
int8_t TriTilt::incIdx(int8_t v) { return prime[cur_variant]->incIdx(v); }
void TriTilt::incColor(int8_t v) { prime[cur_variant]->incColor(v); }
void TriTilt::incShade() { prime[cur_variant]->incShade(); }


//********************************************************************
//*** TRISPEED MODE **************************************************
//********************************************************************
void TriSpeed::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (render_mode == 0 || render_mode == 3) {
    prime[cur_variant]->render(&color_r, &color_g, &color_b);
    prime[0]->incTick();
    prime[1]->incTick();
    prime[2]->incTick();
  } else if (render_mode == 1) {
    if (prime[cur_variant]->canConfig()) {
      if (tick < 50) {
        if (cur_variant == 0) {
          color_r = 0; color_g = 255; color_b = 255;
        } else if (cur_variant == 1) {
          color_r = 0; color_g = 0; color_b = 255;
        } else {
          color_r = 255; color_g = 0; color_b = 255;
        }
      } else {
        color_r = 0; color_g = 0; color_b = 0;
      }
    } else {
      color_r = 255; color_g = 0; color_b = 0;
    }
    tick++;
    if (tick >= 100) { tick = 0; }
  } else if (render_mode == 2) {
    prime[cur_variant]->renderColor(&color_r, &color_g, &color_b);
  }
  *r = color_r; *g = color_g; *b = color_b;
}

void TriSpeed::reset() {
  tick = 0;
  maxg = 0;
  cur_variant = 0;
  prime[0]->reset();
  prime[1]->reset();
  prime[2]->reset();
  render_mode = 0;
}

void TriSpeed::updateAcc(float fxg, float fyg, float fzg) {
  if (render_mode != 0) {
    return;
  }

  float pitch;
  uint8_t level;
  maxg = max(max(abs(fxg), abs(fyg)), abs(fzg));
  if (maxg >= 1.5) {
    acc_counter += 8;
  } else if (maxg >= 1.3) {
    acc_counter += 5;
  } else if (maxg >= 1.1) {
    acc_counter += 2;
  } else {
    /* acc_counter--; */
  }

  if (acc_counter < 0) acc_counter = 0;

  if (cur_variant == 0) {
    if (acc_counter > 250) {
      cur_variant = 1;
    }
    acc_counter -= 1;
  } else if (cur_variant == 1) {
    if (acc_counter <= 0) {
      cur_variant = 0;
    } else if (acc_counter > 2000) {
      cur_variant = 2;
    }
    acc_counter -= 3;
  } else {
    if (acc_counter < 1500) {
      cur_variant = 1;
    } else if (acc_counter > 2250) {
      acc_counter = 2250;
    }
    acc_counter -= 5;
  }
}

void TriSpeed::save(uint16_t addr) {
  prime[0]->save(addr);
  prime[1]->save(addr + 10);
  prime[2]->save(addr + 20);
}

void TriSpeed::load(uint16_t addr) {
  prime[0]->load(addr);
  prime[1]->load(addr + 10);
  prime[2]->load(addr + 20);
}

void TriSpeed::nextPalette() { cur_variant = (cur_variant + 1) % 3; }
int8_t TriSpeed::incIdx(int8_t v) { return prime[cur_variant]->incIdx(v); }
void TriSpeed::incColor(int8_t v) { prime[cur_variant]->incColor(v); }
void TriSpeed::incShade() { prime[cur_variant]->incShade(); }


//********************************************************************
//*** TILTMORPH MODE *************************************************
//********************************************************************
void TiltMorph::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (render_mode == 0 || render_mode == 3) {
    if (tick < color_time) {
      unpackHue(hue + hue_offset, &color_r, &color_g, &color_b);
      color_r = color_r >> 1; color_g = color_g >> 1; color_b = color_b >> 1;
    } else {
      color_r = 0; color_g = 0; color_b = 0;
    }
    tick++;
  } else {
    color_r = 128; color_g = 0; color_b = 0;
  }
  *r = color_r; *g = color_g; *b = color_b;
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
  render_mode = 0;
}

void TiltMorph::save(uint16_t addr) {}
void TiltMorph::load(uint16_t addr) {}
void TiltMorph::nextPalette() {}
int8_t TiltMorph::incIdx(int8_t v) { return 0; }
void TiltMorph::incColor(int8_t v) {}
void TiltMorph::incShade() {}


//********************************************************************
//*** GEOMORPH MODE **************************************************
//********************************************************************
void GeoMorph::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  if (render_mode == 0 || render_mode == 3) {
    if (tick < color_time) {
      color_r = geo_r; color_g = geo_g; color_b = geo_b;
    } else {
      color_r = 0; color_g = 0; color_b = 0;
    }
    tick++;
  } else if (render_mode == 1) {
    if (tick < 50) {
      color_r = 128; color_g = 128; color_b = 0;
    } else {
      color_r = 0; color_g = 0; color_b = 0;
    }
    tick++;
    if (tick >= 100) { tick = 0; }
  } else {
    unpackColor(palette[edit_color], &color_r, &color_g, &color_b);
  }
  *r = color_r; *g = color_g; *b = color_b;
  tick++;
  if (tick >= color_time + blank_time) tick = 0;
}

void GeoMorph::updateAcc(float fxg, float fyg, float fzg) {
  float pitch_x = (atan2(fxg, sqrt(fyg * fyg + fzg * fzg)) * 2.0) / M_PI;
  float pitch_y = (atan2(fyg, sqrt(fxg * fxg + fzg * fzg)) * 2.0) / M_PI;
  uint8_t target_r, target_g, target_b;

  unpackColor(palette[0], &geo_r, &geo_g, &geo_b);
  if (op_mode == GM_ONLYX) {
    if (pitch_x >= 0) {
      unpackColor(palette[1], &target_r, &target_g, &target_b);
    } else {
      unpackColor(palette[2], &target_r, &target_g, &target_b);
    }
    morphColor(20 * abs(pitch_y), 20,
        geo_r, geo_g, geo_b,
        target_r, target_g, target_b,
        &geo_r, &geo_g, &geo_b);
  } else if (op_mode == GM_ONLYY) {
    if (pitch_y >= 0) {
      unpackColor(palette[1], &target_r, &target_g, &target_b);
    } else {
      unpackColor(palette[2], &target_r, &target_g, &target_b);
    }
    morphColor(20 * abs(pitch_y), 20,
        geo_r, geo_g, geo_b,
        target_r, target_g, target_b,
        &geo_r, &geo_g, &geo_b);
  } else {
    if (pitch_x >= 0) {
      unpackColor(palette[1], &target_r, &target_g, &target_b);
    } else {
      unpackColor(palette[2], &target_r, &target_g, &target_b);
    }
    morphColor(20 * abs(pitch_x), 20,
        geo_r, geo_g, geo_b,
        target_r, target_g, target_b,
        &geo_r, &geo_g, &geo_b);
    if (pitch_y >= 0) {
      unpackColor(palette[3], &target_r, &target_g, &target_b);
    } else {
      unpackColor(palette[4], &target_r, &target_g, &target_b);
    }
    morphColor(20 * abs(pitch_y), 20,
        geo_r, geo_g, geo_b,
        target_r, target_g, target_b,
        &geo_r, &geo_g, &geo_b);
  }
}

void GeoMorph::reset() {
  tick = 0;
}

void GeoMorph::save(uint16_t addr) {
  EEPROM.update(addr + 0, palette[0]);
  EEPROM.update(addr + 1, palette[1]);
  EEPROM.update(addr + 2, palette[2]);
  EEPROM.update(addr + 3, palette[3]);
  EEPROM.update(addr + 4, palette[4]);
}

void GeoMorph::load(uint16_t addr) {
  palette[0] = EEPROM.read(addr + 0);
  palette[1] = EEPROM.read(addr + 1);
  palette[2] = EEPROM.read(addr + 2);
  palette[3] = EEPROM.read(addr + 3);
  palette[4] = EEPROM.read(addr + 4);
}

void GeoMorph::nextPalette() {}

int8_t GeoMorph::incIdx(int8_t v) {
  uint8_t rtn_code = 0;
  if (v < 0) {                                // When decrementing, return -1 if at beginning
    if (edit_color == 0) {
      rtn_code = -1;
    } else {                                  // Otherwise return 0 and dec color
      edit_color--;
    }
  } else if (v > 0) {                         // When incrementing, return -1 if at end
    if ((op_mode == GM_ONLYX || op_mode == GM_ONLYY) && edit_color >= 2) {
        rtn_code = -1;
    } else if (op_mode == GM_XANDY && edit_color >= 4) {
        rtn_code = -1;
    } else {
      edit_color++;
    }
  } else if (v == 0) {                        // 0 is resetting
    edit_color = 0;
  }
  Serial.print(F("editing color ")); Serial.println(edit_color + 1);
  return rtn_code;
}

void GeoMorph::incColor(int8_t v) {
  uint8_t idx = palette[edit_color] & 0b00111111;
  palette[edit_color] = (palette[edit_color] & 0b11000000) + ((idx + v + 63) % 63);
}

void GeoMorph::incShade() {
  palette[edit_color] += 0b01000000;
}


//********************************************************************
//*** INOVA MODE *****************************************************
//********************************************************************
void iNova::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = color_r; *g = color_g, *b = color_b;
}

void iNova::reset() {
  tick = 0;
  op_mode = 0;
  counter = 0;
  go_off = false;
  button_state = 0;
}

int8_t iNova::handlePress(bool pressed) {
  switch (op_mode) {
    case INOVA_OFF:
      if (pressed) {
        Serial.println(F("inova pressed"));
        counter = 0;
        button_state = 1;
        op_mode = INOVA_ON_WAIT;
      } else if (counter > 300000) { // Sleep after 5 min idle
        return 3; // sleep
      }
      color_r = 0; color_g = 0; color_b = 0;
      break;

    case INOVA_ON_WAIT:
      // show color
      // if held for 1.5s - 3s, we're going to next mode on release
      // if held for more than 3s, we're going to config the color
      // if released before 1.5s, go to ON
      if (!pressed) {
        if (counter >= 5000) {
          Serial.println(F("inova config"));
          op_mode = INOVA_CONFIG;
        } else if (counter >= 1500) {
          Serial.println(F("inova next mode"));
          return 1; // next mode
        } else {
          Serial.println(F("inova on"));
          op_mode = INOVA_ON;
        }
        button_state = 0;
      } else {
        if (counter == 5000) {
          return 12; // flash yellow
        }
      }
      unpackColor(color, &color_r, &color_g, &color_b);
      break;

    case INOVA_ON:
      // solid color if not pressed, blank if pressed
      // if released after TIMEOUT, go to OFF
      // if released before, reset counter and go to DOPS
      if (button_state == 0) {
        if (pressed) {
          Serial.println(F("inova on pressed"));
          button_state = 1;
          go_off = (counter >= INOVA_TIMEOUT);
        }
        unpackColor(color, &color_r, &color_g, &color_b);
      } else {
        if (!pressed) {
          if (go_off) {
            Serial.println(F("inova off"));
            op_mode = INOVA_OFF;
            counter = 0;
          } else {
            Serial.print(F("inova dops ")); Serial.println(counter);
            op_mode = INOVA_DOPS;
            tick = 0;
            counter = 0;
          }
          button_state = 0;
        }
        color_r = 0; color_g = 0; color_b = 0;
      }
      break;

    case INOVA_DOPS:
      // dops color if not pressed, blank if pressed
      // if released after TIMEOUT, go to OFF
      // if released before, reset counter and go to SIGNAL
      if (button_state == 0) {
        if (pressed) {
          Serial.println(F("inova dops pressed"));
          button_state = 1;
          go_off = (counter >= INOVA_TIMEOUT);
        }

        // dops
        if (tick >= 10) tick = 0;
        if (tick < 1) {
          unpackColor(color, &color_r, &color_g, &color_b);
        } else {
          color_r = 0; color_g = 0; color_b = 0;
        }
      } else {
        if (!pressed) {
          if (go_off) {
            Serial.println(F("inova off"));
            op_mode = INOVA_OFF;
            counter = 0;
          } else {
            Serial.print(F("inova signal ")); Serial.println(counter);
            op_mode = INOVA_SIGNAL;
            tick = 0;
            counter = 0;
          }
          button_state = 0;
        }
        color_r = 0; color_g = 0; color_b = 0;
      }
      break;

    case INOVA_SIGNAL:
      // signal color if not pressed, blank if pressed
      // on release go to OFF
      if (button_state == 0) {
        if (pressed) {
          Serial.println(F("inova signal pressed"));
          button_state = 1;
        }

        // signal
        if (tick >= 300) tick = 0;
        if (tick < 10) {
          unpackColor(color, &color_r, &color_g, &color_b);
        } else {
          color_r = 0; color_g = 0; color_b = 0;
        }
      } else {
        if (!pressed) {
          Serial.println(F("inova off"));
          button_state = 0;
          op_mode = INOVA_OFF;
          counter = 0;
        }
        color_r = 0; color_g = 0; color_b = 0;
      }
      break;

    default: // INOVA_CONFIG
      // press goes forward a color
      // dpress goes back a color
      // hold selects shade
      // release from hold saves and exits to INOVA_OFF
      switch (button_state) {
        case 0:
          if (pressed) {
            Serial.println(F("inova config pressed"));
            counter = 0;
            button_state = 1;
          }
          break;
        case 1:
          if (!pressed) {
            button_state = 2;
          } else if (counter >= 1000) {
            Serial.println(F("inova config will pick"));
            counter -= 1000;
            button_state = 3;
            return 10; // flash white
          }
          break;
        case 2:
          if (pressed) {
            Serial.println(F("inova config prev color"));
            incColor(-1);
            button_state = 99;
          } else if (counter >= 350) {
            Serial.println(F("inova config next color"));
            incColor(1);
            button_state = 0;
          }
          break;
        case 3:
          if (!pressed) {
            Serial.println(F("inova config save"));
            op_mode = INOVA_OFF;
            counter = 0;
            return 2; // save
          } else if (counter > 500) {
            Serial.println(F("inova config next shade"));
            incShade();
            counter -= 500;
            return 10; // flash white
          }
          break;
        default:
          if (!pressed) {
            button_state = 0;
          }
          break;
      }
      unpackColor(color, &color_r, &color_g, &color_b);
  }
  tick++;
  counter++;
  return 0;
}

void iNova::updateAcc(float fxg, float fyg, float fzg) {}
void iNova::save(uint16_t addr) { EEPROM.update(addr, color); }
void iNova::load(uint16_t addr) { color = EEPROM.read(addr); }
void iNova::nextPalette() {}
int8_t iNova::incIdx(int8_t v) { return 0; }

void iNova::incColor(int8_t v) {
  uint8_t idx = color & 0b00111111;
  color = (color & 0b11000000) + ((idx + v + 63) % 63);
}

void iNova::incShade() {
  color += 0b01000000;
}
