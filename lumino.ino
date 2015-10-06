#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <OSM_NEOv1-1-4.h>
#include "elapsedMillis.h"

#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3

#define A_OFF 0
#define A_SHAKE 1
#define A_TILTX 2
#define A_TILTY 3
#define A_TILTZ 4

#define P_STROBE 0
#define P_HYPERSTROBE 1

static const uint8_t gamma_table[256] = {
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3,   3,   3,   3,
  4,   4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   8,
  9,   9,   9,   10,  10,  11,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,
  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  22,  22,  23,  23,  24,
  25,  25,  26,  27,  27,  28,  29,  29,  30,  31,  31,  32,  33,  33,  34,  35,
  36,  36,  37,  38,  39,  40,  40,  41,  42,  43,  44,  44,  45,  46,  47,  48,
  49,  50,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  79,  80,
  81,  82,  83,  84,  85,  87,  88,  89,  90,  91,  93,  94,  95,  96,  97,  99,
  100, 101, 102, 104, 105, 106, 108, 109, 110, 112, 113, 114, 116, 117, 118, 120,
  121, 122, 124, 125, 127, 128, 129, 131, 132, 134, 135, 137, 138, 140, 141, 143,
  144, 146, 147, 149, 150, 152, 153, 155, 156, 158, 160, 161, 163, 164, 166, 168,
  169, 171, 172, 174, 176, 177, 179, 181, 182, 184, 186, 188, 189, 191, 193, 195,
  196, 198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218, 220, 222, 224,
  225, 227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255,
};

// idx from 00-1f, add 0x40 for each shading level
// e.g. 0x88 for 2nd dimmest red
// 0xff indicates first unused color
static const uint8_t color_palette[32][3] = {
  // Blank and whites
  {0, 0, 0},        // 0bXX000000  0x00
  {255, 255, 255},  // 0bXX000001  0x01
  {255, 191, 191},  // 0bXX000010  0x02
  {191, 255, 191},  // 0bXX000011  0x03
  {191, 191, 255},  // 0bXX000100  0x04
  {255, 255, 191},  // 0bXX000101  0x05
  {255, 191, 255},  // 0bXX000110  0x06
  {191, 255, 255},  // 0bXX000111  0x07

  // Red - green
  {255, 0, 0},      // 0bXX001000  0x08
  {255, 63, 0},     // 0bXX001001  0x09
  {255, 127, 0},    // 0bXX001010  0x0a
  {255, 191, 0},    // 0bXX001011  0x0b
  {255, 255, 0},    // 0bXX001100  0x0c
  {191, 255, 0},    // 0bXX001101  0x0d
  {127, 255, 0},    // 0bXX001110  0x0e
  {63, 255, 0},     // 0bXX001111  0x0f

  // Green - blue
  {0, 255, 0},      // 0bXX010000  0x10
  {0, 255, 63},     // 0bXX010001  0x11
  {0, 255, 127},    // 0bXX010010  0x12
  {0, 255, 191},    // 0bXX010011  0x13
  {0, 255, 255},    // 0bXX010100  0x14
  {0, 191, 255},    // 0bXX010101  0x15
  {0, 127, 255},    // 0bXX010110  0x16
  {0, 63, 255},     // 0bXX010111  0x17

  // Blue - red
  {0, 0, 255},      // 0bXX011000  0x18
  {63, 0, 255},     // 0bXX011001  0x19
  {127, 0, 255},    // 0bXX011010  0x1a
  {191, 0, 255},    // 0bXX011011  0x1b
  {255, 0, 255},    // 0bXX011100  0x1c
  {255, 0, 191},    // 0bXX011101  0x1d
  {255, 0, 127},    // 0bXX011110  0x1e
  {255, 0, 63},     // 0bXX011111  0x1f
};

// Mode array structure 8 * 24 = 192 bytes
// 0 - pattern A
// 1 - color A0
// 2 - color A1
// 3 - color A2
// 4 - color A3
// 5 - color A4
// 6 - color A5
// 7 - color A6
// 8 - color A7
// 9 - pattern B
// 10 - color B0
// 11 - color B1
// 12 - color B2
// 13 - color B3
// 14 - color B4
// 15 - color B5
// 16 - color B6
// 17 - color B7
// 18 - accel_mode: 0 off, 1 shake, 2 tiltX, 3 tiltY, 4 tiltZ
// 19 - accel_sensitivity
uint8_t mode_bank[8][20] = {
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
  {P_STROBE,      0x08, 0x10, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff,
   P_HYPERSTROBE, 0x08, 0x0b, 0x0d, 0x10, 0x14, 0x18, 0x1a, 0x1d,
   A_SHAKE, 100},
};

bool go_to_sleep = false;
elapsedMicros limiter = 0;
uint32_t tick = 0;

uint8_t color = 0;
uint8_t led_r, led_g, led_b;
uint8_t buf_r, buf_g, buf_b;
int acc_x, acc_y, acc_z;

uint8_t cur_mode = 0;
uint8_t mode_state = 0;
uint8_t button_state = 0;
uint16_t since_press = 0;

uint8_t accel_counter = 0;
bool use_variant = false;

Mode mode;

void setup() {
  Serial.begin(57600);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_LDO, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  digitalWrite(PIN_LDO, HIGH);

  // Set up fast PWM mode on timer0 and timer1
  noInterrupts();
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  MMA7660.init();
  limiter = 0;
}

void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t shade = color >> 6;
  uint8_t idx = color & 0b00111111;
  *r = color_palette[idx][0] - ((color_palette[idx][0] >> 2) * shade);
  *g = color_palette[idx][1] - ((color_palette[idx][1] >> 2) * shade);
  *b = color_palette[idx][2] - ((color_palette[idx][2] >> 2) * shade);
}

void loop() {
  if (tick % 10 == 0) {
    MMA7660.getValues(&acc_x, &acc_y, &acc_z);
  }
  if (tick % 1000 == 0) {
    unpackColor(color + 192, &led_r, &led_g, &led_b);
    color = (color + 1) % 32;
  }
  analogWrite(PIN_R, gamma_table[led_r]);
  analogWrite(PIN_G, gamma_table[led_g]);
  analogWrite(PIN_B, gamma_table[led_b]);

  // micros returns 64 * micros due to timer0 frequency
  while (limiter < 64000) {}
  limiter = 0;
  tick++;
}

void enterSleep() {
  Serial.println(F("Going to sleep!"));
  attachInterrupt(0, pushInterrupt, LOW);
  analogWrite(PIN_R, 0);
  analogWrite(PIN_G, 0);
  analogWrite(PIN_B, 0);
  delay(100);

  MMA7660.standby();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();

  while (digitalRead(PIN_BUTTON) == LOW) {}

  sleep_disable();
  Serial.println(F("Waking up now"));
}

void pushInterrupt() {
  detachInterrupt(0);
}

void handlePress(bool pressed) {
  switch (button_state) {
    case 0:
      if (pressed) {
        since_press = 0;
        button_state = 1;
      }
      break;
    case 1:
      if (!pressed) {
        cur_mode = (cur_mode + 1) % 8;
        button_state = 0;
      } else if (since_press > 1500) {
        button_state = 0;
        cur_mode = 0;
        enterSleep();
      }
      break;
    default:
      button_state = 0;
      handlePress(pressed);
      break;
  }
  since_press++;
}
