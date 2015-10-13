// ********************************************************************
// **** DO NOT EDIT THE INCLUDES OF THE FIRST BLOCK OF DEFINES ********
// ********************************************************************
#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "elapsedMillis.h"
#include "modes.h"

#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3
#define MMA7660_ADDRESS 0x4C
// ********************************************************************
// **** DO NOT EDIT THE INCLUDES OF THE FIRST BLOCK OF DEFINES ********
// ********************************************************************


// ********************************************************************
// **** START EDITING HERE TO CUSTOMIZE YOUR MODES ********************
// ********************************************************************

// This number is the maximum number of modes you can have at once.
// There MUST be this many modes.
#define NUM_MODES 8

// DEFINE MODES AND PRIMES HERE. Palettes go in setupModes()
// Mode0 - TiltMorph with low sensitivity
TiltMorph mode0 = TiltMorph(0.05);

// Mode1 - TriMode for fingers up and down with low sensitivity.
// 8 color full-spectrum palette with red, green, or blue tracer depending on the prime.
TriMode mode1 = TriMode(A_TILTX, 0.05);
TracerPrime prime10 = TracerPrime(3, 23, 0xc8); // Hand flat - red tracer
TracerPrime prime11 = TracerPrime(3, 23, 0xd0); // Tilt hand up - green tracer
TracerPrime prime12 = TracerPrime(3, 23, 0xd8); // Hand down - blue tracer

// Mode2 - Speeder with high sensitivity.
// Rainbow splits up and goes from strobie to strobe to hyperstrobe
Speeder mode2 = Speeder(0.9);
RainbowPrime prime20 = RainbowPrime(3, 23, 1, 256, 2);
RainbowPrime prime21 = RainbowPrime(5, 8, 2, 256, 2);
RainbowPrime prime22 = RainbowPrime(17, 17, 3, 256, 2);

// Mode3 - Speed DualMode with Blink-E for slow and full spectrum Strobie for fast
DualMode mode3 = DualMode(A_SPEED, 0.75);
BlinkEPrime prime30 = BlinkEPrime(5, 30); // Slow
StrobePrime prime31 = StrobePrime(3, 23); // Fast

// Mode4 - Down is fade out, up is morph
DualMode mode4 = DualMode(A_TILTX, 0.05);
FadePrime prime40 = FadePrime(50, 5, 1); // Tilt down - Fade out
MorphPrime prime41 = MorphPrime(50, 5);  // Tilt up - morph

// Mode5 - Half the color wheel for left and right
DualMode mode5 = DualMode(A_TILTY, 0.05);
StrobePrime prime50 = StrobePrime(5, 8); // Tilt left
StrobePrime prime51 = StrobePrime(5, 8); // Tilt right

// Mode6 - Half the color wheel for button up and down
DualMode mode6 = DualMode(A_TILTZ, 0.05);
StrobePrime prime60 = StrobePrime(5, 8); // Button up
StrobePrime prime61 = StrobePrime(5, 8); // Button down

// Mode7 - Morph hyperstrobe
SingleMode mode7 = SingleMode();
MorphPrime prime70 = MorphPrime(17, 17);

// MAKE SURE ALL MODES ARE IN HERE AND THERE ARE AT LEAST NUM_MODES (default is 8) ENTRIES
// THIS STORES A POINTER TO EACH OF YOUR MODES SO THEY CAN BE USED IN BUNDLES
Mode *modes[NUM_MODES] = {
  &mode0,
  &mode1,
  &mode2,
  &mode3,
  &mode4,
  &mode5,
  &mode6,
  &mode7,
};

// DEFINE YOUR BUNDLES HERE
// YOU MUST HAVE NUM_BUNDLES (default is 4) LINES
#define NUM_BUNDLES 4

int8_t bundles[NUM_BUNDLES][16] = {
  {0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1},
  {3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 7, 0, 3, 0, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};

// SETUP MODES HERE
void setupModes() {
  // Set num_colors, mode primes, and prime palettes here
  // Mode0 - TiltMorph has no setup

  // Mode1
  prime10.num_colors = 8;
  prime10.palette[0] = 0x48;
  prime10.palette[1] = 0x4b;
  prime10.palette[2] = 0x4e;
  prime10.palette[3] = 0x51;
  prime10.palette[4] = 0x54;
  prime10.palette[5] = 0x57;
  prime10.palette[6] = 0x5a;
  prime10.palette[7] = 0x5d;
  mode1.prime[0] = &prime10;

  prime11.num_colors = 8;
  prime11.palette[0] = 0x48;
  prime11.palette[1] = 0x4b;
  prime11.palette[2] = 0x4e;
  prime11.palette[3] = 0x51;
  prime11.palette[4] = 0x54;
  prime11.palette[5] = 0x57;
  prime11.palette[6] = 0x5a;
  prime11.palette[7] = 0x5d;
  mode1.prime[1] = &prime11;

  prime12.num_colors = 8;
  prime12.palette[0] = 0x48;
  prime12.palette[1] = 0x4b;
  prime12.palette[2] = 0x4e;
  prime12.palette[3] = 0x51;
  prime12.palette[4] = 0x54;
  prime12.palette[5] = 0x57;
  prime12.palette[6] = 0x5a;
  prime12.palette[7] = 0x5d;
  mode1.prime[2] = &prime12;

  // Mode2
  /* prime20.num_colors = 8; */
  /* prime20.palette[0] = 0x17; */
  /* prime20.palette[1] = 0x16; */
  /* prime20.palette[2] = 0x15; */
  /* prime20.palette[3] = 0x14; */
  /* prime20.palette[4] = 0x13; */
  /* prime20.palette[5] = 0x12; */
  /* prime20.palette[6] = 0x11; */
  /* prime20.palette[7] = 0x10; */

  /* prime21.num_colors = 8; */
  /* prime21.palette[0] = 0x0f; */
  /* prime21.palette[1] = 0x0e; */
  /* prime21.palette[2] = 0x0d; */
  /* prime21.palette[3] = 0x0c; */
  /* prime21.palette[4] = 0x0f; */
  /* prime21.palette[5] = 0x0e; */
  /* prime21.palette[6] = 0x0d; */
  /* prime21.palette[7] = 0x0c; */

  /* prime22.num_colors = 8; */
  /* prime22.palette[0] = 0x0b; */
  /* prime22.palette[1] = 0x0a; */
  /* prime22.palette[2] = 0x09; */
  /* prime22.palette[3] = 0x08; */
  /* prime22.palette[4] = 0x1b; */
  /* prime22.palette[5] = 0x1a; */
  /* prime22.palette[6] = 0x19; */
  /* prime22.palette[7] = 0x18; */

  mode2.prime[0] = &prime20;
  mode2.prime[1] = &prime21;
  mode2.prime[2] = &prime22;

  // Mode3
  prime30.num_colors = 8;
  prime30.palette[0] = 0x06;
  prime30.palette[1] = 0xdb;
  prime30.palette[2] = 0xd9;
  prime30.palette[3] = 0xd6;
  prime30.palette[4] = 0xd2;
  prime30.palette[5] = 0xcf;
  prime30.palette[6] = 0xcc;
  prime30.palette[7] = 0xc8;
  mode3.prime[0] = &prime30;

  prime31.num_colors = 8;
  prime31.palette[0] = 0x88;
  prime31.palette[1] = 0x8b;
  prime31.palette[2] = 0x8e;
  prime31.palette[3] = 0x91;
  prime31.palette[4] = 0x95;
  prime31.palette[5] = 0x98;
  prime31.palette[6] = 0x9a;
  prime31.palette[7] = 0x9d;
  mode3.prime[1] = &prime31;

  // Mode4
  prime40.num_colors = 8;
  prime40.palette[0] = 0x08;
  prime40.palette[1] = 0x0b;
  prime40.palette[2] = 0x0e;
  prime40.palette[3] = 0x11;
  prime40.palette[4] = 0x14;
  prime40.palette[5] = 0x17;
  prime40.palette[6] = 0x1a;
  prime40.palette[7] = 0x1d;
  mode4.prime[0] = &prime40;

  prime41.num_colors = 8;
  prime41.palette[0] = 0x88;
  prime41.palette[1] = 0x8b;
  prime41.palette[2] = 0x8e;
  prime41.palette[3] = 0x91;
  prime41.palette[4] = 0x94;
  prime41.palette[5] = 0x97;
  prime41.palette[6] = 0x9a;
  prime41.palette[7] = 0x9d;
  mode4.prime[1] = &prime41;

  // Mode5
  prime50.num_colors = 4;
  prime50.palette[0] = 0x08;
  prime50.palette[1] = 0x0b;
  prime50.palette[2] = 0x0e;
  prime50.palette[3] = 0x11;
  mode5.prime[0] = &prime50;

  prime51.num_colors = 4;
  prime51.palette[0] = 0x14;
  prime51.palette[1] = 0x17;
  prime51.palette[2] = 0x1a;
  prime51.palette[3] = 0x1d;
  mode5.prime[1] = &prime51;

  // Mode6
  prime60.num_colors = 4;
  prime60.palette[0] = 0x1a;
  prime60.palette[1] = 0x1d;
  prime60.palette[2] = 0x08;
  prime60.palette[3] = 0x0b;
  mode6.prime[0] = &prime60;

  prime61.num_colors = 4;
  prime61.palette[0] = 0x0e;
  prime61.palette[1] = 0x11;
  prime61.palette[2] = 0x14;
  prime61.palette[3] = 0x17;
  mode6.prime[1] = &prime61;

  // Mode7
  prime70.num_colors = 8;
  prime70.palette[0] = 0x08;
  prime70.palette[1] = 0x0b;
  prime70.palette[2] = 0x0e;
  prime70.palette[3] = 0x11;
  prime70.palette[4] = 0x14;
  prime70.palette[5] = 0x17;
  prime70.palette[6] = 0x1a;
  prime70.palette[7] = 0x1d;
  mode7.prime = &prime70;
}

// ********************************************************************
// **** END OF MODE CUSTOMIZATION BLOCK. DO NOT EDIT UNDER HERE *******
// ********************************************************************



// ********************************************************************
// **** DO NOT EDIT UNDER HERE (unless you know what you're doing) ****
// ********************************************************************
uint8_t cur_bundle = 0;
uint8_t cur_bundle_idx = 0;
Mode *mode = modes[bundles[cur_bundle][cur_bundle_idx]];

elapsedMicros limiter = 0;
uint8_t accel_counter = 0;
float fxg, fyg, fzg = 0.0;
uint8_t button_state = 0;
uint16_t since_press = 0;
bool conjure = false;
bool conjure_off = false;

int8_t accel_ar[64] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -16, -16,
  -15, -14, -13, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,
};

// Gamma table ensures a smoother fade. Based on gamma=2.2
static const uint8_t gamma_table[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,
    2,   2,   3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,
    6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  11,  11,  11,
   12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,
   19,  20,  21,  21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,
   29,  30,  31,  31,  32,  33,  34,  34,  35,  36,  37,  37,  38,  39,  40,  41,
   41,  42,  43,  44,  45,  46,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,
   56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
   72,  73,  75,  76,  77,  78,  79,  80,  82,  83,  84,  85,  86,  88,  89,  90,
   91,  93,  94,  95,  96,  98,  99, 100, 102, 103, 104, 106, 107, 109, 110, 111,
  113, 114, 116, 117, 118, 120, 121, 123, 124, 126, 127, 129, 130, 132, 134, 135,
  137, 138, 140, 141, 143, 145, 146, 148, 150, 151, 153, 155, 156, 158, 160, 161,
  163, 165, 167, 168, 170, 172, 174, 175, 177, 179, 181, 183, 185, 186, 188, 190,
  192, 194, 196, 198, 200, 202, 204, 205, 207, 209, 211, 213, 215, 217, 219, 221,
  224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 245, 247, 249, 251, 253, 255,
};


void setup() {
  Serial.begin(57600);
  setupModes();

  // Setup pins
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_LDO, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);

  // Turn on LDO
  digitalWrite(PIN_LDO, HIGH);

  // Set up fast PWM mode on timer0 and timer1
  noInterrupts();
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  // Init accelerometer
  accInit();

  // Reset all modes
  for (uint8_t i = 0; i < NUM_MODES; i++) {
    modes[i]->reset();
  }
}

void loop() {
  // Handle the button to see if we should switch modes or sleep.
  // In the future config could be entered through here as well.
  handlePress(digitalRead(PIN_BUTTON) == LOW);

  // Every 10ms we sample the accelerometer and update the mode's acc values.
  // DualMode uses this to switch variants and future modes could do other
  // things with the values
  if (accel_counter == 0) {
    accUpdate();
    mode->updateAcc(fxg, fyg, fzg);
  }
  if (accel_counter >10) {
    accel_counter = 0;
  }

  // Get the color to be rendered
  uint8_t r, g, b;
  mode->render(&r, &g, &b);

  if (conjure && conjure_off) {
    writeFrame(0, 0, 0);
  } else {
    writeFrame(r, g, b);
  }
}

void enterSleep() {
  Serial.println(F("Going to sleep"));

  // Attach interupt to awaken chip from sleep
  attachInterrupt(0, pushInterrupt, LOW);

  // Turn off LEDs
  writeFrame(0, 0, 0);

  // Put accelerometer to sleep
  accStandby();

  // Disable volatage regulator
  digitalWrite(PIN_LDO, LOW);

  // Delay a little so everything catches up
  delay(500);

  // Set sleep mode and power down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();

  // Wait until button is releaed
  uint16_t held_count = 0;
  while (digitalRead(PIN_BUTTON) == LOW) {
    if (limiter >= 64000) {
      limiter = 0;
      held_count++;
    }
    if (held_count > 1500) {
      flash(128, 0, 0, 5);
      cur_bundle = (cur_bundle + 1) % NUM_BUNDLES;
      Serial.print(F("next bundle "));
      Serial.println(cur_bundle);
      held_count -= 1000;
    }
  }

  Serial.println(held_count);
  Serial.println(F("Waking up now"));

  // Wake up. Power on LDO before trying to access the accelerometer
  sleep_disable();
  digitalWrite(PIN_LDO, HIGH);
  accInit();
  delay(4000);

  if (held_count > 3000) {
  }

  resetMode();
  conjure = conjure_off = false;
}

void pushInterrupt() {
  // Disable interrupt until next sleep
  detachInterrupt(0);
}

void handlePress(bool pressed) {
  switch (button_state) {
    // Starts off, when pressed, reset since_press and go to "on" state
    case 0:
      if (pressed) {
        since_press = 0;
        button_state = 1;
        Serial.println(F("pressed"));
      }
      break;

    // When on, if released before 1000ms, switch modes and go to "off"
    // If held for 1000ms, go to "wait" state before sleeping
    case 1:
      since_press++;
      if (!pressed) {
        if (conjure) {
          conjure_off = !conjure_off;
          if (conjure_off) {
            Serial.println(F("conjure_off on"));
          } else {
            Serial.println(F("conjure_off off"));
          }
          button_state = 0;
        } else {
          Serial.println(F("changing mode"));
          incMode();
        }
        button_state = 0;
      } else if (since_press > 1000) {
        Serial.println(F("will sleep"));
        flash(128, 128, 128, 5);
        button_state = 2;
      }
      break;

    // Once button is released, sleep and go to "off" state
    case 2:
      since_press++;
      if (!pressed) {
        enterSleep();
        button_state = 0;
      } else if (since_press > 2500) {  // 500ms taken up during first flash
        Serial.println(F("will conjure"));
        flash(0, 0, 64, 3);
        button_state = 3;
      }
      break;

    case 3:
      if (!pressed) {
        conjure = !conjure;
        button_state = 0;
        if (conjure) {
          Serial.println(F("conjure on"));
        } else {
          Serial.println(F("conjure off"));
        }
      }
      break;
    default:
      break;
  }
}

void resetMode() {
  cur_bundle_idx = 0;
  mode = modes[bundles[cur_bundle][cur_bundle_idx]];
  mode->reset();
  fxg = fyg = fzg = 0.0;
}

void incMode() {
  int8_t new_idx;
  cur_bundle_idx = (cur_bundle_idx + 1) % NUM_MODES;
  new_idx = bundles[cur_bundle][cur_bundle_idx];

  if (new_idx == -1) {
    cur_bundle_idx = 0;
    new_idx = bundles[cur_bundle][cur_bundle_idx];
  }

  mode = modes[new_idx];
  mode->reset();
  fxg = fyg = fzg = 0.0;
}

void flash(uint8_t r, uint8_t g, uint8_t b, uint8_t flashes) {
  for (uint8_t i = 0; i < flashes; i++) {
    for (uint8_t j = 0; j < 100; j++) {
      if (j < 50) {
        writeFrame(r, g, b);
      } else {
        writeFrame(0, 0, 0);
      }
    }
  }
}

void writeFrame(uint8_t r, uint8_t g, uint8_t b) {
  // micros returns 64 * micros due to timer0 frequency
  // Once 1ms passes, continue and reset limiter
  while (limiter < 64000) {}
  limiter = 0;

  // Write out the gamma-corrected color to the LED
  analogWrite(PIN_R, gamma_table[r]);
  analogWrite(PIN_G, gamma_table[g]);
  analogWrite(PIN_B, gamma_table[b]);
}


void accSend(uint8_t reg_address, uint8_t data) {
  Wire.beginTransmission(MMA7660_ADDRESS);
  Wire.write(reg_address);
  Wire.write(data);
  Wire.endTransmission();
}

void accUpdate() {
  uint8_t x, y, z;
  float xg, yg, zg;

  Wire.beginTransmission(MMA7660_ADDRESS);
  Wire.write(0x00);  // register to read
  Wire.endTransmission();
  Wire.requestFrom(MMA7660_ADDRESS, 3); // read a byte

  if (Wire.available()) {
    x = Wire.read();
    y = Wire.read();
    z = Wire.read();
  }
  xg = (x < 64) ? accel_ar[x] / 20.0 : fxg;
  yg = (y < 64) ? accel_ar[y] / 20.0 : fyg;
  zg = (z < 64) ? accel_ar[z] / 20.0 : fzg;

  //Low Pass Filter
  fxg = xg * mode->alpha + (fxg * (1.0 - mode->alpha));
  fyg = yg * mode->alpha + (fyg * (1.0 - mode->alpha));
  fzg = zg * mode->alpha + (fzg * (1.0 - mode->alpha));
}

void accInit() {
  Wire.begin();
  accSend(0x07,0x00);
  accSend(0x06,0x10);
  accSend(0x08,0x00);
  accSend(0x07,0x01);
}

void accStandby() {
  Wire.begin();
  accSend(0x07,0x10);
}
