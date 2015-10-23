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

#define DEBUG
// ********************************************************************


// ********************************************************************
// **** START EDITING HERE TO CUSTOMIZE YOUR MODES ********************
// ********************************************************************
// This number is the maximum number of modes you can have at once.
// There MUST be this many modes.
#define NUM_MODES 12
uint16_t mode_addrs[NUM_MODES];

// DEFINE MODES AND PRIMES HERE. Palettes go in setupModes()
// Mode0
iNova mode0 = iNova();

// Mode1
TiltMorph mode1 = TiltMorph(0.05);

// Mode2
GeoMorph mode2 = GeoMorph(GM_XANDY, 5, 8, 0.05);

// Mode3
TriSpeed mode3 = TriSpeed(0.75);
RainbowPrime prime30 = RainbowPrime(3, 23, 2, 256, 52);
RainbowPrime prime31 = RainbowPrime(17, 17, 3, 256, 52);
RainbowPrime prime32 = RainbowPrime(2, 11, 6, 256, 52);

// Mode4
DualMode mode4 = DualMode(A_SPEED, 0.25);
BlinkEPrime prime40 = BlinkEPrime(5, 69);
StrobePrime prime41 = StrobePrime(3, 23);

// Mode5
DualMode mode5 = DualMode(A_SPEED, 0.5);
FadePrime prime50 = FadePrime(6, 20, 1);
StrobePrime prime51 = StrobePrime(0, 100);

// Mode6
DualMode mode6 = DualMode(A_SPEED, 0.5);
StrobePrime prime60 = StrobePrime(0, 100);
LegoPrime prime61 = LegoPrime(8);

// Mode7
TriTilt mode7 = TriTilt(A_TILTY, 0.5);
EmberPrime prime70 = EmberPrime(16, 16, 1);
EmberPrime prime71 = EmberPrime(16, 16, 1);
EmberPrime prime72 = EmberPrime(16, 16, 1);

// Mode9
TriTilt mode8 = TriTilt(A_TILTX, 0.5);
TracerPrime prime80 = TracerPrime(3, 23);
TracerPrime prime81 = TracerPrime(3, 23);
TracerPrime prime82 = TracerPrime(3, 23);

// Mode9
DualMode mode9 = DualMode(A_FLIPZ, 0.5);
FadePrime prime90 = FadePrime(100, 50, 2);
FadePrime prime91 = FadePrime(100, 50, 2);

// Mode10
DualMode mode10 = DualMode(A_TILTY, 0.5);
StrobePrime prime100 = StrobePrime(15, 21);
TwoTimePrime prime101 = TwoTimePrime(15, 9, 3, 9);

// Mode11
DualMode mode11 = DualMode(A_TILTX, 0.5);
ChasePrime prime110 = ChasePrime(50, 10, 5);
MorphPrime prime111 = MorphPrime(50, 10, 4);


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
  &mode8,
  &mode9,
  &mode10,
  &mode11,
};

#define NUM_BUNDLES 3
int8_t bundles[NUM_BUNDLES][NUM_MODES] = {
  { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11}, // Everything
  { 3,  4,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1}, // Only speed
  { 7,  8,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1}, // Tilt primes
};

// SETUP MODES HERE
void setupModes() {
  // Mode0
  mode0.color = 0x08;

  // Mode1

  // Mode2
  mode2.palette[0] = 0x08 + 0x40;
  mode2.palette[1] = 0x1c + 0x40;
  mode2.palette[2] = 0x0c + 0x40;
  mode2.palette[3] = 0x10 + 0x40;
  mode2.palette[4] = 0x18 + 0x40;

  // Mode3
  mode3.prime[0] = &prime30;
  mode3.prime[1] = &prime31;
  mode3.prime[2] = &prime32;

  // Mode4
  prime40.num_colors = 7;
  prime40.palette[0] = 0x06 + 0x40;
  prime40.palette[1] = 0x3e + 0x40;
  prime40.palette[2] = 0x3d + 0x40;
  prime40.palette[3] = 0x3c + 0x40;
  prime40.palette[4] = 0x3b + 0x40;
  prime40.palette[5] = 0x3a + 0x40;
  prime40.palette[6] = 0x39 + 0x40;
  prime40.palette[7] = 0x00;

  prime41.num_colors = 8;
  prime41.palette[0] = 0x08 + 0x40;
  prime41.palette[1] = 0x0b + 0x40;
  prime41.palette[2] = 0x0e + 0x40;
  prime41.palette[3] = 0x11 + 0x40;
  prime41.palette[4] = 0x14 + 0x40;
  prime41.palette[5] = 0x17 + 0x40;
  prime41.palette[6] = 0x1a + 0x40;
  prime41.palette[7] = 0x1d + 0x40;

  mode4.prime[0] = &prime40;
  mode4.prime[1] = &prime41;

  // Mode5
  prime50.num_colors = 6;
  prime50.palette[0] = 0x08 + 0x40;
  prime50.palette[1] = 0x0c + 0x40;
  prime50.palette[2] = 0x10 + 0x40;
  prime50.palette[3] = 0x14 + 0x40;
  prime50.palette[4] = 0x18 + 0x40;
  prime50.palette[5] = 0x1c + 0x40;
  prime50.palette[6] = 0x00 + 0x40;
  prime50.palette[7] = 0x00 + 0x40;

  prime51.num_colors = 7;
  prime51.palette[0] = 0x3d + 0x40;
  prime51.palette[1] = 0x0a + 0x40;
  prime51.palette[2] = 0x0e + 0x00;
  prime51.palette[3] = 0x12 + 0x40;
  prime51.palette[4] = 0x16 + 0x00;
  prime51.palette[5] = 0x1a + 0x40;
  prime51.palette[6] = 0x1e + 0x40;
  prime51.palette[7] = 0x00 + 0x40;

  mode5.prime[0] = &prime50;
  mode5.prime[1] = &prime51;

  // Mode6
  prime60.num_colors = 1;
  prime60.palette[0] = 0x00 + 0x40;
  prime60.palette[1] = 0x00 + 0x40;
  prime60.palette[2] = 0x00 + 0x40;
  prime60.palette[3] = 0x00 + 0x40;
  prime60.palette[4] = 0x00 + 0x40;
  prime60.palette[5] = 0x00 + 0x40;
  prime60.palette[6] = 0x00 + 0x40;
  prime60.palette[7] = 0x00 + 0x40;

  prime61.num_colors = 8;
  prime61.palette[0] = 0x08 + 0x40;
  prime61.palette[1] = 0x0b + 0x40;
  prime61.palette[7] = 0x0e + 0x40;
  prime61.palette[2] = 0x11 + 0x40;
  prime61.palette[6] = 0x14 + 0x40;
  prime61.palette[3] = 0x17 + 0x40;
  prime61.palette[4] = 0x1a + 0x40;
  prime61.palette[5] = 0x1d + 0x40;

  mode6.prime[0] = &prime60;
  mode6.prime[1] = &prime61;

  // Mode7
  prime70.num_colors = 1;
  prime70.palette[0] = 0x08 + 0x40;
  prime70.palette[1] = 0x00 + 0x40;
  prime70.palette[2] = 0x00 + 0x40;
  prime70.palette[3] = 0x00 + 0x40;
  prime70.palette[4] = 0x00 + 0x40;
  prime70.palette[5] = 0x00 + 0x40;
  prime70.palette[6] = 0x00 + 0x40;
  prime70.palette[7] = 0x00 + 0x40;

  prime71.num_colors = 1;
  prime71.palette[0] = 0x10 + 0x40;
  prime71.palette[1] = 0x00 + 0x40;
  prime71.palette[2] = 0x00 + 0x40;
  prime71.palette[3] = 0x00 + 0x40;
  prime71.palette[4] = 0x00 + 0x40;
  prime71.palette[5] = 0x00 + 0x40;
  prime71.palette[6] = 0x00 + 0x40;
  prime71.palette[7] = 0x00 + 0x40;

  prime72.num_colors = 1;
  prime72.palette[0] = 0x18 + 0x40;
  prime72.palette[1] = 0x00 + 0x40;
  prime72.palette[2] = 0x00 + 0x40;
  prime72.palette[3] = 0x00 + 0x40;
  prime72.palette[4] = 0x00 + 0x40;
  prime72.palette[5] = 0x00 + 0x40;
  prime72.palette[6] = 0x00 + 0x40;
  prime72.palette[7] = 0x00 + 0x40;

  mode7.prime[0] = &prime70;
  mode7.prime[1] = &prime71;
  mode7.prime[2] = &prime72;

  // Mode8
  prime80.num_colors = 7;
  prime80.palette[0] = 0x39 + 0x40;
  prime80.palette[1] = 0x0a + 0x40;
  prime80.palette[2] = 0x0e + 0x00;
  prime80.palette[3] = 0x12 + 0x40;
  prime80.palette[4] = 0x16 + 0x00;
  prime80.palette[5] = 0x1a + 0x40;
  prime80.palette[6] = 0x1e + 0x40;
  prime80.palette[7] = 0x00 + 0x00;

  prime81.num_colors = 7;
  prime81.palette[0] = 0x3b + 0x40;
  prime81.palette[1] = 0x0a + 0x40;
  prime81.palette[2] = 0x0e + 0x00;
  prime81.palette[3] = 0x12 + 0x40;
  prime81.palette[4] = 0x16 + 0x00;
  prime81.palette[5] = 0x1a + 0x40;
  prime81.palette[6] = 0x1e + 0x40;
  prime81.palette[7] = 0x00 + 0x00;

  prime82.num_colors = 7;
  prime82.palette[0] = 0x3d + 0x40;
  prime82.palette[1] = 0x0a + 0x40;
  prime82.palette[2] = 0x0e + 0x00;
  prime82.palette[3] = 0x12 + 0x40;
  prime82.palette[4] = 0x16 + 0x00;
  prime82.palette[5] = 0x1a + 0x40;
  prime82.palette[6] = 0x1e + 0x40;
  prime82.palette[7] = 0x00 + 0x00;

  mode8.prime[0] = &prime80;
  mode8.prime[1] = &prime81;
  mode8.prime[2] = &prime82;

  // Mode9
  prime90.num_colors = 1;
  prime90.palette[0] = 0x16;
  prime90.palette[1] = 0x00;
  prime90.palette[2] = 0x00;
  prime90.palette[3] = 0x00;
  prime90.palette[4] = 0x00;
  prime90.palette[5] = 0x00;
  prime90.palette[6] = 0x00;
  prime90.palette[7] = 0x00;

  prime91.num_colors = 1;
  prime91.palette[0] = 0x1e;
  prime91.palette[1] = 0x00;
  prime91.palette[2] = 0x00;
  prime91.palette[3] = 0x00;
  prime91.palette[4] = 0x00;
  prime91.palette[5] = 0x00;
  prime91.palette[6] = 0x00;
  prime91.palette[7] = 0x00;

  mode9.prime[0] = &prime90;
  mode9.prime[1] = &prime91;

  // Mode10
  prime100.num_colors = 5;
  prime100.palette[0] = 0x08 + 0x40;
  prime100.palette[1] = 0x0b + 0x40;
  prime100.palette[7] = 0x0e + 0x40;
  prime100.palette[2] = 0x11 + 0x40;
  prime100.palette[6] = 0x14 + 0x40;
  prime100.palette[3] = 0x17 + 0x40;
  prime100.palette[4] = 0x1a + 0x40;
  prime100.palette[5] = 0x1d + 0x40;

  prime101.num_colors = 5;
  prime101.palette[0] = 0x08 + 0x40;
  prime101.palette[1] = 0x0b + 0x40;
  prime101.palette[7] = 0x0e + 0x40;
  prime101.palette[2] = 0x11 + 0x40;
  prime101.palette[6] = 0x14 + 0x40;
  prime101.palette[3] = 0x17 + 0x40;
  prime101.palette[4] = 0x1a + 0x40;
  prime101.palette[5] = 0x1d + 0x40;

  mode10.prime[0] = &prime100;
  mode10.prime[1] = &prime101;

  // Mode11
  prime110.num_colors = 8;
  prime110.palette[0] = 0x08 + 0x40;
  prime110.palette[1] = 0x0b + 0x40;
  prime110.palette[7] = 0x0e + 0x40;
  prime110.palette[2] = 0x11 + 0x40;
  prime110.palette[6] = 0x14 + 0x40;
  prime110.palette[3] = 0x17 + 0x40;
  prime110.palette[4] = 0x1a + 0x40;
  prime110.palette[5] = 0x1d + 0x40;

  prime111.num_colors = 8;
  prime111.palette[0] = 0x08 + 0x40;
  prime111.palette[1] = 0x0b + 0x40;
  prime111.palette[7] = 0x0e + 0x40;
  prime111.palette[2] = 0x11 + 0x40;
  prime111.palette[6] = 0x14 + 0x40;
  prime111.palette[3] = 0x17 + 0x40;
  prime111.palette[4] = 0x1a + 0x40;
  prime111.palette[5] = 0x1d + 0x40;

  mode11.prime[0] = &prime110;
  mode11.prime[1] = &prime111;


  // DO NOT EDIT THIS FOR LOOP
  for (uint8_t i = 0; i < NUM_MODES; i++) {
    mode_addrs[i] = MEMORY_START + (i * MEMORY_SIZE);
  }
  // END
}

// Change this to a value between 1 and 255 to upload new settings to the EEPROM
uint8_t current_version = 255;

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

uint8_t cur_bundle = 0;
uint8_t bundle_idx = 0;
Mode *mode = modes[bundles[cur_bundle][bundle_idx]];
elapsedMicros limiter = 0;
uint8_t accel_counter = 0;
float fxg, fyg, fzg = 0.0;
uint8_t button_state = 0;
uint16_t since_press = 0;
bool conjure = false;
bool conjure_toggle = false;

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

void setup() {
#ifndef DEBUG
  power_usart0_disable();
#endif
  power_spi_disable();

  randomSeed(analogRead(0));

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
  ADCSRA = 0; // Disable ADC
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  // Wait for everything to be ready
  delay(4000);
  limiter = 0;
}

void loop() {
  if (mode->use_button) {
    handleModePress(digitalRead(PIN_BUTTON) == LOW);
  } else {
    handlePress(digitalRead(PIN_BUTTON) == LOW);
  }

  // Every 10ms we sample the accelerometer and update the mode's acc values.
  // DualMode uses this to switch variants and future modes could do other
  // things with the values
  if (accel_counter >= 10) accel_counter = 0;
  if (accel_counter == 0) {
    accUpdate();
    mode->updateAcc(fxg, fyg, fzg);
  }
  accel_counter++;

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
  writeFrame(0, 0, 0);
  accStandby();
  digitalWrite(PIN_LDO, LOW);
  delay(4000);

  // Set sleep mode and power down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  noInterrupts();
  attachInterrupt(0, pushInterrupt, FALLING);
  EIFR = bit(INTF0);  // clear flag for interrupt 0

  // turn off brown-out enable in software
  // BODS must be set to one and BODSE must be set to zero within four clock cycles
  MCUCR = bit(BODS) | bit(BODSE);
  // The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit(BODS);

  // We are guaranteed that the sleep_cpu call will be done
  // as the processor executes the next instruction after
  // interrupts are turned on.
  interrupts ();  // one cycle
  sleep_cpu ();   // one cycle

  // Wait until button is releaed
  uint16_t held_count = 0;
  limiter = 0;
  while (digitalRead(PIN_BUTTON) == LOW) {
    if (limiter > 64000) {
      limiter = 0;
      held_count++;
    }
    if (held_count > 1500) {
      cur_bundle = (cur_bundle + 1) % NUM_BUNDLES;
      Serial.print(F("current bundle ")); Serial.println(cur_bundle);
      flash(0, 0, 128, 5);
      held_count = 500;
    }
  }

  // Wake up. Power on LDO before trying to access the accelerometer
  // Wait for the accelerometer to come back online before continuing
  Serial.println(F("Waking up now"));
  digitalWrite(PIN_LDO, HIGH);
  accInit();
  resetMode();
  conjure = conjure_toggle = false;
  delay(4000);
}

void pushInterrupt() {
  sleep_disable();
  detachInterrupt(0);
}

void resetMode() {
  bundle_idx = 0;
  mode = modes[bundles[cur_bundle][bundle_idx]];
  mode->reset();
  fxg = fyg = fzg = 0.0;
}

void incMode() {
  bundle_idx = (bundle_idx + 1) % NUM_MODES;
  if (bundles[cur_bundle][bundle_idx] == -1) bundle_idx = 0;
  mode = modes[bundles[cur_bundle][bundle_idx]];
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

  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
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

void accSend(uint8_t reg_address, uint8_t data) {
  Wire.beginTransmission(MMA7660_ADDRESS);
  Wire.write(reg_address);
  Wire.write(data);
  Wire.endTransmission();
}

float translateAccel(int8_t g, float fg, float alpha) {
  if (g >= 64) {
    // Out of bounds, don't alter fg
    return fg;
  } else if (g >= 32) {
    // Translate 32  - 63 to -32 - -1
    g = -64 + g;
  }

  return ((g / 20.0) * alpha) + (fg * (1.0 - alpha));
}

void accUpdate() {
  Wire.beginTransmission(MMA7660_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(MMA7660_ADDRESS, 3);

  if (Wire.available()) {
    fxg = translateAccel(Wire.read(), fxg, mode->alpha);
    fyg = translateAccel(Wire.read(), fyg, mode->alpha);
    fzg = translateAccel(Wire.read(), fzg, mode->alpha);
  }
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
          incMode();
          Serial.print(F("changing to mode ")); Serial.println(bundles[cur_bundle][bundle_idx]);
        }
        button_state = S_PLAY_OFF;
      } else if (since_press > 1000) {
        Serial.println(F("will sleep"));
        flash(128, 128, 128, 5); since_press = 500;
        button_state = S_PLAY_SLEEP_WAIT;
      }
      break;

    case S_PLAY_SLEEP_WAIT:
      if (!pressed) {
        enterSleep();
        button_state = S_PLAY_OFF;
      } else if (since_press > 1500) {
        Serial.println(F("will conjure"));
        flash(0, 0, 128, 5); since_press = 500;
        button_state = S_PLAY_CONJURE_WAIT;
      }
      break;

    case S_PLAY_CONJURE_WAIT:
      if (!pressed) {
        if (conjure) { Serial.println(F("conjure off")); }
        else {         Serial.println(F("conjure on")); }
        conjure = !conjure;
        button_state = S_PLAY_OFF;
      } else if (since_press > 1500) {
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
        mode->save(mode_addrs[bundles[cur_bundle][bundle_idx]]);
        mode->render_mode = 0;
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
        button_state = S_PSELECT_OFF;
      }
      break;

    default:
      button_state = S_PLAY_OFF;
      break;
  }
  since_press++;
}

void handleModePress(bool pressed) {
  int8_t rtn_code = mode->handlePress(pressed);
  switch (rtn_code) {
    case 1: // next mode
      incMode();
      Serial.print(F("changing to mode ")); Serial.println(bundles[cur_bundle][bundle_idx]);
      break;
    case 2: // save
      mode->save(mode_addrs[bundles[cur_bundle][bundle_idx]]);
      break;
    case 3: // sleep
      enterSleep();
      break;
    case 10: // flash white
      flash(128, 128, 128, 5);
      break;
    case 11: // flash red
      flash(128, 0, 0, 5);
      break;
    case 12: // flash yellow
      flash(128, 128, 0, 5);
      break;
    case 13: // flash green
      flash(0, 128, 0, 5);
      break;
    case 14: // flash cyan
      flash(0, 128, 128, 5);
      break;
    case 15: // flash blue
      flash(0, 0, 128, 5);
      break;
    case 16: // flash magenta
      flash(128, 0, 128, 5);
      break;
    default:
      break;
  }
}
