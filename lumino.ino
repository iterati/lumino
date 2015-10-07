#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <OSM_NEOv1-1-4.h>
#include "elapsedMillis.h"
#include "modes.h"

#define DEBUG

#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3

#define NUM_MODES 8

// Declare modes here. Palettes go in setupModes()
// Mode0 - RGB Strobe
SingleMode mode0 = SingleMode();
StrobePrime prime00 = StrobePrime(5, 10);

// Mode1 - dim rainbow trail strobes when shaken
DualMode mode1 = DualMode(A_SHAKE, 200);
StrobePrime prime10 = StrobePrime(11, 0);
StrobePrime prime11 = StrobePrime(4, 7);

// Mode2 -
SingleMode mode2 = SingleMode();
StrobePrime prime20 = StrobePrime(5, 10);

// Mode3 -
DualMode mode3 = DualMode(A_SHAKE, 200);
StrobePrime prime30 = StrobePrime(11, 0);
StrobePrime prime31 = StrobePrime(4, 7);

// Mode4 -
SingleMode mode4 = SingleMode();
StrobePrime prime40 = StrobePrime(5, 10);

// Mode5 -
DualMode mode5 = DualMode(A_SHAKE, 200);
StrobePrime prime50 = StrobePrime(11, 0);
StrobePrime prime51 = StrobePrime(4, 7);

// Mode6 -
SingleMode mode6 = SingleMode();
StrobePrime prime60 = StrobePrime(5, 10);

// Mode7 -
DualMode mode7 = DualMode(A_SHAKE, 200);
StrobePrime prime70 = StrobePrime(11, 0);
StrobePrime prime71 = StrobePrime(4, 7);


uint8_t cur_mode = 0;
Mode *modes[NUM_MODES] = {&mode0, &mode1, &mode2, &mode3, &mode4, &mode5, &mode6, &mode7};
Mode *mode = modes[cur_mode];


// Gamma table ensures a smoother fade. Based on gamma=2
static const uint8_t gamma_table[256] = {
  0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,
  2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   3,
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

elapsedMicros limiter = 0;      // Tracks timing
uint8_t accel_counter = 10;     // Grab accel data every 10 ms

uint8_t led_r, led_g, led_b;    // led channel value buffer
int acc_x, acc_y, acc_z;        // accel values

uint8_t button_state;           // What state is the button in
uint16_t since_press;           // How long since the button was pressed

void setupModes() {
  // Set num_colors, mode primes, and prime palettes here
  // Mode0
  prime00.num_colors = 3;
  prime00.palette[0] = 0x08;
  prime00.palette[1] = 0x10;
  prime00.palette[2] = 0x18;
  prime00.reset();
  mode0.prime = &prime00;
  mode0.reset();

  // Mode1
  prime10.num_colors = 8;
  prime10.palette[0] = 0x44;
  prime10.palette[1] = 0xc8;
  prime10.palette[2] = 0xcb;
  prime10.palette[3] = 0xcf;
  prime10.palette[4] = 0xd6;
  prime10.palette[5] = 0xd6;
  prime10.palette[6] = 0xd9;
  prime10.palette[7] = 0xdd;
  prime10.reset();
  mode1.prime[0] = &prime10;

  prime11.num_colors = 8;
  prime11.palette[0] = 0x44;
  prime11.palette[1] = 0x48;
  prime11.palette[2] = 0x4b;
  prime11.palette[3] = 0x4f;
  prime11.palette[4] = 0x56;
  prime11.palette[5] = 0x56;
  prime11.palette[6] = 0x59;
  prime11.palette[7] = 0x5d;
  prime11.reset();
  mode1.prime[1] = &prime11;
  mode1.reset();

  // Mode2
  prime20.num_colors = 3;
  prime20.palette[0] = 0x08;
  prime20.palette[1] = 0x10;
  prime20.palette[2] = 0x18;
  prime20.reset();
  mode2.prime = &prime20;
  mode2.reset();

  // Mode3
  prime30.num_colors = 8;
  prime30.palette[0] = 0x44;
  prime30.palette[1] = 0xc8;
  prime30.palette[2] = 0xcb;
  prime30.palette[3] = 0xcf;
  prime30.palette[4] = 0xd6;
  prime30.palette[5] = 0xd6;
  prime30.palette[6] = 0xd9;
  prime30.palette[7] = 0xdd;
  prime30.reset();
  mode3.prime[0] = &prime30;

  prime31.num_colors = 8;
  prime31.palette[0] = 0x44;
  prime31.palette[1] = 0x48;
  prime31.palette[2] = 0x4b;
  prime31.palette[3] = 0x4f;
  prime31.palette[4] = 0x56;
  prime31.palette[5] = 0x56;
  prime31.palette[6] = 0x59;
  prime31.palette[7] = 0x5d;
  prime31.reset();
  mode3.prime[1] = &prime31;
  mode3.reset();

  // Mode4
  prime40.num_colors = 3;
  prime40.palette[0] = 0x08;
  prime40.palette[1] = 0x10;
  prime40.palette[2] = 0x18;
  prime40.reset();
  mode4.prime = &prime40;
  mode4.reset();

  // Mode5
  prime50.num_colors = 8;
  prime50.palette[0] = 0x44;
  prime50.palette[1] = 0xc8;
  prime50.palette[2] = 0xcb;
  prime50.palette[3] = 0xcf;
  prime50.palette[4] = 0xd6;
  prime50.palette[5] = 0xd6;
  prime50.palette[6] = 0xd9;
  prime50.palette[7] = 0xdd;
  prime50.reset();
  mode5.prime[0] = &prime50;

  prime51.num_colors = 8;
  prime51.palette[0] = 0x44;
  prime51.palette[1] = 0x48;
  prime51.palette[2] = 0x4b;
  prime51.palette[3] = 0x4f;
  prime51.palette[4] = 0x56;
  prime51.palette[5] = 0x56;
  prime51.palette[6] = 0x59;
  prime51.palette[7] = 0x5d;
  prime51.reset();
  mode5.prime[1] = &prime51;
  mode5.reset();

  // Mode6
  prime60.num_colors = 3;
  prime60.palette[0] = 0x08;
  prime60.palette[1] = 0x10;
  prime60.palette[2] = 0x18;
  prime60.reset();
  mode6.prime = &prime60;
  mode6.reset();

  // Mode7
  prime70.num_colors = 8;
  prime70.palette[0] = 0x44;
  prime70.palette[1] = 0xc8;
  prime70.palette[2] = 0xcb;
  prime70.palette[3] = 0xcf;
  prime70.palette[4] = 0xd6;
  prime70.palette[5] = 0xd6;
  prime70.palette[6] = 0xd9;
  prime70.palette[7] = 0xdd;
  prime70.reset();
  mode7.prime[0] = &prime70;

  prime71.num_colors = 8;
  prime71.palette[0] = 0x44;
  prime71.palette[1] = 0x48;
  prime71.palette[2] = 0x4b;
  prime71.palette[3] = 0x4f;
  prime71.palette[4] = 0x56;
  prime71.palette[5] = 0x56;
  prime71.palette[6] = 0x59;
  prime71.palette[7] = 0x5d;
  prime71.reset();
  mode7.prime[1] = &prime71;
  mode7.reset();
}

void setup() {
#ifdef DEBUG
  Serial.begin(57600);
#endif

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

  // Init accelerometer, limiter, and mode;
  MMA7660.init();
}

void loop() {
  // Handle the button to see if we should switch modes or sleep.
  // In the future config could be entered through here as well.
  handlePress(digitalRead(PIN_BUTTON) == LOW);

  // Every 10ms we sample the accelerometer and update the mode's acc values.
  // DualMode uses this to switch variants and future modes could do other
  // things with the values
  if (accel_counter == 10) {
    MMA7660.getValues(&acc_x, &acc_y, &acc_z);
    mode->updateAcc(acc_x, acc_y, acc_z);
  }
  accel_counter--;
  if (accel_counter == 0) {
    accel_counter = 10;
  }

  // Get the color to be rendered
  mode->render(&led_r, &led_g, &led_b);

  // micros returns 64 * micros due to timer0 frequency
  // Once 1ms passes, continue and reset limiter
  while (limiter < 64000) {}
  limiter = 0;

  // Write out the gamma-corrected color to the LED
  analogWrite(PIN_R, gamma_table[led_r]);
  analogWrite(PIN_G, gamma_table[led_g]);
  analogWrite(PIN_B, gamma_table[led_b]);
}

void enterSleep() {
#ifdef DEBUG
  Serial.println(F("Going to sleep"));
#endif

  // Attach interupt to awaken chip from sleep
  attachInterrupt(0, pushInterrupt, LOW);

  // Turn off LEDs
  analogWrite(PIN_R, 0);
  analogWrite(PIN_G, 0);
  analogWrite(PIN_B, 0);

  // Put accelerometer to sleep
  MMA7660.MMA7660_I2C_SEND(0x07, 0x00);

  // Disable volatage regulator
  digitalWrite(PIN_LDO, LOW);

  // Delay a little so everything catches up
  delay(500);

  // Set sleep mode and power down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();

  // Wait until button is releaed
  while (digitalRead(PIN_BUTTON) == LOW) {}

#ifdef DEBUG
  Serial.println(F("Waking up now"));
#endif

  // Wake up. Power on LDO before trying to access the accelerometer
  digitalWrite(PIN_LDO, HIGH);
  MMA7660.MMA7660_I2C_SEND(0x07, 0x01);
  sleep_disable();
  changeMode(0);
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
      }
      break;

    // When on, if released before 1000ms, switch modes and go to "off"
    // If held for 1000ms, go to "wait" state before sleeping
    case 1:
      if (!pressed) {
        changeMode((cur_mode + 1) % NUM_MODES);
        button_state = 0;
      } else if (since_press > 1000) {
        button_state = 2;
      }
      break;

    // Once button is released, sleep and go to "off" state
    case 2:
      if (!pressed) {
        enterSleep();
        button_state = 0;
      }
      break;

    default:
      break;
  }
  since_press++;
}

void changeMode(uint8_t idx) {
  // Set current mode idx, point to mode, and then reset it
  cur_mode = idx;
  mode = modes[cur_mode];
  mode->reset();
}
