// ********************************************************************
// **** DO NOT EDIT ***************************************************
// ********************************************************************
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "elapsedMillis.h"
#include "modes.h"
// ********************************************************************


// ********************************************************************
// **** START EDITING HERE TO CUSTOMIZE YOUR MODES ********************
// ********************************************************************
// This number is the maximum number of modes you can have at once.
// There MUST be this many modes.
#define NUM_MODES 8
uint16_t mode_addrs[NUM_MODES];

// DEFINE MODES AND PRIMES HERE. Palettes go in setupModes()
TiltMorph mode0 = TiltMorph(0.05);

// Mode1
TriSpeed mode1 = TriSpeed(0.9);
RainbowPrime prime10 = RainbowPrime(3,  23, 2, 256,   2);
RainbowPrime prime11 = RainbowPrime(5,  8,  3, 128, 2);
RainbowPrime prime12 = RainbowPrime(17, 17, 5, 64,  2);

// Mode2
TriSpeed mode2 = TriSpeed(0.9);
StrobePrime prime20 = StrobePrime(3, 23);
StrobePrime prime21 = StrobePrime(5, 8);
StrobePrime prime22 = StrobePrime(17, 17);

// Mode3
DualMode mode3 = DualMode(A_SPEED, 0.75);
BlinkEPrime prime30 = BlinkEPrime(3, 23);
StrobePrime prime31 = StrobePrime(3, 23);

// Mode4
DualMode mode4 = DualMode(A_SPEED, 0.75);
StrobePrime prime40 = StrobePrime(5, 8);
StrobePrime prime41 = StrobePrime(3, 23);

// Mode5
DualMode mode5 = DualMode(A_SPEED, 0.75);
StrobePrime prime50 = StrobePrime(1, 10);
StrobePrime prime51 = StrobePrime(17, 17);

// Mode6
TriTilt mode6 = TriTilt(A_TILTY, 0.05);
StrobePrime prime60 = StrobePrime(3, 23);
StrobePrime prime61 = StrobePrime(3, 23);
StrobePrime prime62 = StrobePrime(3, 23);

// Mode7
DualMode mode7 = DualMode(A_TILTY, 0.05);
FadePrime prime70 = FadePrime(100, 50, 2);
FadePrime prime71 = FadePrime(100, 50, 2);

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

// SETUP MODES HERE
void setupModes() {
  // Mode1
  mode1.prime[0] = &prime10;
  mode1.prime[1] = &prime11;
  mode1.prime[2] = &prime12;

  // Mode2
  prime20.num_colors = 8;
  prime20.palette[0] = 0x08 + 0x40;
  prime20.palette[1] = 0x0b + 0x40;
  prime20.palette[2] = 0x0e + 0x40;
  prime20.palette[3] = 0x11 + 0x40;
  prime20.palette[4] = 0x14 + 0x40;
  prime20.palette[5] = 0x17 + 0x40;
  prime20.palette[6] = 0x1a + 0x40;
  prime20.palette[7] = 0x1d + 0x40;

  prime21.num_colors = 8;
  prime21.palette[0] = 0x08 + 0x40;
  prime21.palette[1] = 0x0b + 0x40;
  prime21.palette[2] = 0x0e + 0x40;
  prime21.palette[3] = 0x11 + 0x40;
  prime21.palette[4] = 0x14 + 0x40;
  prime21.palette[5] = 0x17 + 0x40;
  prime21.palette[6] = 0x1a + 0x40;
  prime21.palette[7] = 0x1d + 0x40;

  prime22.num_colors = 8;
  prime22.palette[0] = 0x08 + 0x40;
  prime22.palette[1] = 0x0b + 0x40;
  prime22.palette[2] = 0x0e + 0x40;
  prime22.palette[3] = 0x11 + 0x40;
  prime22.palette[4] = 0x14 + 0x40;
  prime22.palette[5] = 0x17 + 0x40;
  prime22.palette[6] = 0x1a + 0x40;
  prime22.palette[7] = 0x1d + 0x40;

  mode2.prime[0] = &prime20;
  mode2.prime[1] = &prime21;
  mode2.prime[2] = &prime22;

  // Mode3
  prime30.num_colors = 7;
  prime30.palette[0] = 0x06 + 0x40;
  prime30.palette[1] = 0x3e + 0xc0;
  prime30.palette[2] = 0x3d + 0xc0;
  prime30.palette[3] = 0x3c + 0xc0;
  prime30.palette[4] = 0x3b + 0xc0;
  prime30.palette[5] = 0x3a + 0xc0;
  prime30.palette[6] = 0x39 + 0xc0;
  prime30.palette[7] = 0x00;

  prime31.num_colors = 8;
  prime31.palette[0] = 0x08 + 0x40;
  prime31.palette[1] = 0x0b + 0x40;
  prime31.palette[2] = 0x0e + 0x40;
  prime31.palette[3] = 0x11 + 0x40;
  prime31.palette[4] = 0x14 + 0x40;
  prime31.palette[5] = 0x17 + 0x40;
  prime31.palette[6] = 0x1a + 0x40;
  prime31.palette[7] = 0x1d + 0x40;

  mode3.prime[0] = &prime30;
  mode3.prime[1] = &prime31;

  // Mode4
  prime40.num_colors = 5;
  prime40.palette[0] = 0x08 + 0x40;
  prime40.palette[1] = 0x00;
  prime40.palette[2] = 0x11 + 0x40;
  prime40.palette[3] = 0x1a + 0x40;
  prime40.palette[4] = 0x00;
  prime40.palette[5] = 0x00;
  prime40.palette[6] = 0x00;
  prime40.palette[7] = 0x00;

  prime41.num_colors = 7;
  prime41.palette[0] = 0x16 + 0x40;
  prime41.palette[1] = 0x08 + 0xc0;
  prime41.palette[2] = 0x1a + 0x40;
  prime41.palette[3] = 0x08 + 0xc0;
  prime41.palette[4] = 0x0f + 0x40;
  prime41.palette[5] = 0x08 + 0xc0;
  prime41.palette[6] = 0x00;
  prime41.palette[7] = 0x00;

  mode4.prime[0] = &prime40;
  mode4.prime[1] = &prime41;

  // Mode5
  prime50.num_colors = 8;
  prime50.palette[0] = 0x08 + 0x40;
  prime50.palette[1] = 0x0b + 0x40;
  prime50.palette[2] = 0x0e + 0x40;
  prime50.palette[3] = 0x11 + 0x40;
  prime50.palette[4] = 0x14 + 0x40;
  prime50.palette[5] = 0x17 + 0x40;
  prime50.palette[6] = 0x1a + 0x40;
  prime50.palette[7] = 0x1d + 0x40;

  prime51.num_colors = 8;
  prime51.palette[0] = 0x08 + 0x40;
  prime51.palette[1] = 0x0b + 0x40;
  prime51.palette[2] = 0x0e + 0x40;
  prime51.palette[3] = 0x11 + 0x40;
  prime51.palette[4] = 0x14 + 0x40;
  prime51.palette[5] = 0x17 + 0x40;
  prime51.palette[6] = 0x1a + 0x40;
  prime51.palette[7] = 0x1d + 0x40;

  mode5.prime[0] = &prime50;
  mode5.prime[1] = &prime51;

  // Mode6
  prime60.num_colors = 8;
  prime60.palette[0] = 0x08 + 0x40;
  prime60.palette[1] = 0x09 + 0x40;
  prime60.palette[2] = 0x0a + 0x40;
  prime60.palette[3] = 0x0b + 0x40;
  prime60.palette[4] = 0x0c + 0x40;
  prime60.palette[5] = 0x0d + 0x40;
  prime60.palette[6] = 0x0e + 0x40;
  prime60.palette[7] = 0x0f + 0x40;

  prime61.num_colors = 8;
  prime61.palette[0] = 0x10 + 0x40;
  prime61.palette[1] = 0x11 + 0x40;
  prime61.palette[2] = 0x12 + 0x40;
  prime61.palette[3] = 0x13 + 0x40;
  prime61.palette[4] = 0x14 + 0x40;
  prime61.palette[5] = 0x15 + 0x40;
  prime61.palette[6] = 0x16 + 0x40;
  prime61.palette[7] = 0x17 + 0x40;

  prime62.num_colors = 8;
  prime62.palette[0] = 0x18 + 0x40;
  prime62.palette[1] = 0x19 + 0x40;
  prime62.palette[2] = 0x1a + 0x40;
  prime62.palette[3] = 0x1b + 0x40;
  prime62.palette[4] = 0x1c + 0x40;
  prime62.palette[5] = 0x1d + 0x40;
  prime62.palette[6] = 0x1e + 0x40;
  prime62.palette[7] = 0x1f + 0x40;

  mode6.prime[0] = &prime60;
  mode6.prime[1] = &prime61;
  mode6.prime[2] = &prime62;

  // Mode7
  prime70.num_colors = 8;
  prime70.palette[0] = 0x16;
  prime70.palette[1] = 0x00;
  prime70.palette[2] = 0x00;
  prime70.palette[3] = 0x00;
  prime70.palette[4] = 0x00;
  prime70.palette[5] = 0x00;
  prime70.palette[6] = 0x00;
  prime70.palette[7] = 0x00;

  prime71.num_colors = 1;
  prime71.palette[0] = 0x1f;
  prime71.palette[1] = 0x00;
  prime71.palette[2] = 0x00;
  prime71.palette[3] = 0x00;
  prime71.palette[4] = 0x00;
  prime71.palette[5] = 0x00;
  prime71.palette[6] = 0x00;
  prime71.palette[7] = 0x00;

  mode7.prime = &prime70;
  mode7.prime = &prime71;

  // DO NOT EDIT THIS FOR LOOP
  for (uint8_t i = 0; i < NUM_MODES; i++) {
    mode_addrs[i] = MEMORY_START + (i * MEMORY_SIZE);
  }
  // END
}

void saveModes() {
  for (uint8_t i = 0; i < NUM_MODES; i++) {
    modes[i]->save(mode_addrs[i]);
  }
}

void loadModes() {
  for (uint8_t i = 0; i < NUM_MODES; i++) {
    modes[i]->load(mode_addrs[i]);
  }
}

// Change this to a value between 1 and 255 to upload new settings to the EEPROM
uint8_t current_version = 42;

// ********************************************************************
// **** END OF MODE CUSTOMIZATION BLOCK. DO NOT EDIT UNDER HERE *******
// ********************************************************************



// ********************************************************************
// **** DO NOT EDIT ***************************************************
// ********************************************************************
#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3
#define MMA7660_ADDRESS 0x4C

uint8_t cur_mode = 0;
Mode *mode = modes[cur_mode];

elapsedMicros limiter = 0;
uint8_t accel_counter = 0;
float fxg, fyg, fzg = 0.0;
uint8_t button_state = 0;
uint16_t since_press = 0;
bool conjure = false;
bool conjure_toggle = false;

int8_t accel_ar[64] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -16, -16,
  -15, -14, -13, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,
};

/*
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
*/

void setup() {
  Serial.begin(57600);

  Serial.println(F("\nWelcome to Lumino!"));

  // Setup pins
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LDO, OUTPUT);
  digitalWrite(PIN_LDO, HIGH);

  setupModes();
  if (current_version != EEPROM.read(512)) {
    Serial.println(F("Version mismatch. Writing new EEPROM."));
    saveModes();
    EEPROM.update(512, current_version);
  } else {
    Serial.println(F("Version match. Reading saved settings."));
    loadModes();
  }

  // Reset all modes
  for (uint8_t i = 0; i < NUM_MODES; i++) {
    modes[i]->reset();
  }

  // Init accelerometer
  accInit();

  // Set up fast PWM mode on timer0 and timer1
  noInterrupts();
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  // Wait for everything to be ready
  delay(4000);
  limiter = 0;
}

void loop() {
  handlePress(digitalRead(PIN_BUTTON) == LOW);

  // Every 10ms we sample the accelerometer and update the mode's acc values.
  // DualMode uses this to switch variants and future modes could do other
  // things with the values
  if (accel_counter >= 10) accel_counter = 0;
  if (accel_counter == 0) {
    accUpdate();
    mode->updateAcc(fxg, fyg, fzg);
  }

  // Get the color to be rendered
  uint8_t r, g, b;
  mode->render(&r, &g, &b);

  // If conjuring and we toggled off, write blank. Otherwise write out the frame.
  if (conjure && conjure_toggle) {
    writeFrame(0, 0, 0);
  } else {
    writeFrame(r, g, b);
  }
}

void enterSleep() {
  Serial.println(F("Going to sleep"));

  // Attach interupt to awaken chip from sleep
  // Turn off LEDs
  // Put accelerometer to sleep
  // Disable volatage regulator
  // Delay a little so everything catches up
  attachInterrupt(0, pushInterrupt, LOW);
  writeFrame(0, 0, 0);
  accStandby();
  digitalWrite(PIN_LDO, LOW);
  delay(4000);

  // Set sleep mode and power down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();

  // Wait until button is releaed
  while (digitalRead(PIN_BUTTON) == LOW) {}

  // Wake up. Power on LDO before trying to access the accelerometer
  // Wait for the accelerometer to come back online before continuing
  Serial.println(F("Waking up now"));
  sleep_disable();
  digitalWrite(PIN_LDO, HIGH);
  accInit();
  setMode(0);
  conjure = conjure_toggle = false;
  delay(4000);
}

void pushInterrupt() {
  // Disable interrupt until next sleep
  detachInterrupt(0);
}

void setMode(uint8_t i) {
  cur_mode = i;
  mode = modes[cur_mode];
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
  analogWrite(PIN_R, r >> 1); //gamma_table[r >> 1]);
  analogWrite(PIN_G, g >> 1); //gamma_table[g >> 1]);
  analogWrite(PIN_B, b >> 1); //gamma_table[b >> 1]);
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
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(MMA7660_ADDRESS, 3);

  if (Wire.available()) {
    x = Wire.read();
    y = Wire.read();
    z = Wire.read();
  }

  // If the value is > 63, it's an error value and we just use previous
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
  accSend(0x07, 0x00);
  accSend(0x06, 0x10);
  accSend(0x08, 0x00);
  accSend(0x07, 0x01);
}

void accStandby() {
  Wire.begin();
  accSend(0x07, 0x10);
}


// ********************************************************************
// **** BUTTON CODE ***************************************************
// ********************************************************************
#define S_PLAY_OFF              0
#define S_PLAY_PRESSED          1
#define S_PLAY_SLEEP_WAIT       2
#define S_PLAY_CONJURE_WAIT     3
#define S_PLAY_CONFIG_WAIT      4

#define S_PSELECT_OFF           10
#define S_PSELECT_PRESSED       11
#define S_PSELECT_EDIT_WAIT     12
#define S_PSELECT_EXIT_WAIT     13

#define S_CSELECT_OFF           20
#define S_CSELECT_PRESSED       21
#define S_CSELECT_PRESS_WAIT    22
#define S_CSELECT_SHADE_WAIT    23
#define S_CSELECT_RELEASE_WAIT  24

#define S_CCONFIRM_OFF          30
#define S_CCONFIRM_PRESSED      31
#define S_CCONFIRM_REJECT_WAIT  32
#define S_CCONFIRM_EXIT_WAIT    33

void handlePress(bool pressed) {
  int8_t rtn_code;
  switch (button_state) {
    //******************************************************
    //** PLAY **********************************************
    //******************************************************
    case S_PLAY_OFF:
      if (pressed) {
        Serial.println(F("pressed"));

        since_press = 0;
        button_state = S_PLAY_PRESSED;
      }
      break;

    case S_PLAY_PRESSED:
      if (!pressed) {
        if (conjure) {
          if (conjure_toggle) { Serial.println(F("conjure_toggle off")); }
          else {                Serial.println(F("conjure_toggle on")); }
          conjure_toggle = !conjure_toggle;
        } else {
          Serial.print(F("changing to mode ")); Serial.println(((cur_mode + 1) % NUM_MODES) + 1);
          setMode((cur_mode + 1) % NUM_MODES);
        }
        button_state = S_PLAY_OFF;
      } else if (since_press > 1000) {
        Serial.println(F("will sleep"));
        flash(128, 128, 128, 5); since_press += 500;
        button_state = S_PLAY_SLEEP_WAIT;
      }
      break;

    case S_PLAY_SLEEP_WAIT:
      if (!pressed) {
        enterSleep();
        button_state = S_PLAY_OFF;
      } else if (since_press > 3000) {
        Serial.println(F("will conjure"));
        flash(0, 0, 128, 5); since_press += 500;
        button_state = S_PLAY_CONJURE_WAIT;
      }
      break;

    case S_PLAY_CONJURE_WAIT:
      if (!pressed) {
        if (conjure) { Serial.println(F("conjure off")); }
        else {         Serial.println(F("conjure on")); }
        conjure = !conjure;
        button_state = S_PLAY_OFF;
      } else if (since_press > 5000) {
        Serial.println(F("will config"));
        flash(128, 128, 0, 5);
        button_state = S_PLAY_CONFIG_WAIT;
      }
      break;

    case S_PLAY_CONFIG_WAIT:
      if (!pressed) {
        mode->reset();
        mode->render_mode = 1;
        button_state = S_PSELECT_OFF;
      }
      break;

    //******************************************************
    //** PALETTE SELECT ************************************
    //******************************************************
    case S_PSELECT_OFF:
      if (pressed) {
        Serial.println(F("pressed"));
        since_press = 0;
        button_state = S_PSELECT_PRESSED;
      }
      break;

    case S_PSELECT_PRESSED:
      if (!pressed) {
        Serial.println(F("next palette"));
        mode->nextPalette();
        button_state = S_PSELECT_OFF;
      } else if (since_press > 1000) {
        Serial.println(F("will edit"));
        flash(128, 128, 0, 5); since_press += 500;
        button_state = S_PSELECT_EDIT_WAIT;
      }
      break;

    case S_PSELECT_EDIT_WAIT:
      if (!pressed) {
        Serial.println(F("edit"));
        mode->render_mode = 2;
        mode->incIdx(0);
        button_state = S_CSELECT_OFF;
      } else if (since_press > 3000) {
        Serial.println(F("will exit"));
        flash(128, 128, 128, 5);
        button_state = S_PSELECT_EXIT_WAIT;
      }
      break;

    case S_PSELECT_EXIT_WAIT:
      if (!pressed) {
        Serial.println(F("exit"));
        mode->reset();
        button_state = S_PLAY_OFF;
      }
      break;

    //******************************************************
    //** COLOR SELECT **************************************
    //******************************************************
    case S_CSELECT_OFF:
      if (pressed) {
        Serial.println(F("pressed"));
        since_press = 0;
        button_state = S_CSELECT_PRESSED;
      }
      break;

    case S_CSELECT_PRESSED:
      if (!pressed) {
        Serial.println(F("wait for dpress"));
        button_state = S_CSELECT_PRESS_WAIT;
      } else if (since_press > 1000) {
        Serial.println(F("will select"));
        flash(64, 64, 64, 5);
        since_press -= 1000;
        button_state = S_CSELECT_SHADE_WAIT;
      }
      break;

    case S_CSELECT_PRESS_WAIT:
      if (pressed) {
        Serial.println(F("dec color"));
        mode->incColor(-1);
        button_state = S_CSELECT_RELEASE_WAIT;
      } else if (since_press > 350) {
        Serial.println(F("inc color"));
        mode->incColor(1);
        button_state = S_CSELECT_OFF;
      }
      break;

    case S_CSELECT_SHADE_WAIT:
      if (!pressed) {
        Serial.println(F("confirm color"));
        mode->render_mode = 3;
        button_state = S_CCONFIRM_OFF;
      } else if (since_press > 500) {
        Serial.println(F("inc shade"));
        flash(64, 64, 64, 5);
        mode->incShade();
        since_press -= 500;
        button_state = S_CSELECT_SHADE_WAIT;
      }
      break;

    case S_CSELECT_RELEASE_WAIT:
      if (!pressed) {
        button_state = S_CSELECT_OFF;
      }
      break;

    //******************************************************
    //** COLOR CONFIRM *************************************
    //******************************************************
    case S_CCONFIRM_OFF:
      if (pressed) {
        Serial.println(F("pressed"));
        since_press = 0;
        button_state = S_CCONFIRM_PRESSED;
      }
      break;

    case S_CCONFIRM_PRESSED:
      if (!pressed) {
        Serial.println(F("accept color"));
        rtn_code = mode->incIdx(1);
        if (rtn_code == -1) {
          // finish this palette, co back to palette select
          Serial.println(F("finished editing"));
          flash(128, 128, 128, 5);
          mode->save(mode_addrs[cur_mode]);
          mode->render_mode = 1;
          button_state = S_PSELECT_OFF;
        } else {
          // edit next color
          flash(0, 0, 128, 5);
          mode->render_mode = 2;
          button_state = S_CSELECT_OFF;
        }
      } else if (since_press > 1000) {
        Serial.println(F("will reject"));
        flash(128, 0, 0, 5);
        since_press += 500;
        button_state = S_CCONFIRM_REJECT_WAIT;
      }
      break;

    case S_CCONFIRM_REJECT_WAIT:
      if (!pressed) {
        rtn_code = mode->incIdx(-1);
        if (rtn_code == -1) {
          Serial.println(F("reject first color, reselect"));
          mode->render_mode = 2;
          button_state = S_CSELECT_OFF;
        } else {
          Serial.println(F("reject color"));
          button_state = S_CCONFIRM_OFF;
        }
      } else if (since_press > 3000) {
        Serial.println(F("will exit"));
        flash(128, 128, 128, 5);
        button_state = S_CCONFIRM_EXIT_WAIT;
      }
      break;

    case S_CCONFIRM_EXIT_WAIT:
      if (!pressed) {
        Serial.println(F("exit to palette select and save"));
        mode->render_mode = 1;
        mode->save(mode_addrs[cur_mode]);
        button_state = S_PSELECT_OFF;
      }
      break;

    default:
      button_state = S_PLAY_OFF;
      break;
  }
  since_press++;
}
