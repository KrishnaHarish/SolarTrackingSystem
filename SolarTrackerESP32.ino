/*
  ESP32 Solar Tracker (1 Servo + 2 LDRs)

  Hardware:
  - ESP32 Dev Module
  - 1x Servo (SG90/MG90S/etc)
  - 2x LDR (photoresistors) + 2x fixed resistors (10k recommended)

  Notes:
  - Power the servo from an external 5V supply (do NOT use ESP32 3.3V).
  - Connect external 5V GND to ESP32 GND (common ground).
  - Use ADC1 pins for analogRead on ESP32 (GPIO32-39 are safe).

  Library:
  - ESP32Servo (install via Arduino Library Manager)
*/

#include <Arduino.h>
#include <ESP32Servo.h>

// -------- Pins (change to match your wiring) --------
static const int PIN_LDR_LEFT  = 34; // ADC1
static const int PIN_LDR_RIGHT = 35; // ADC1
static const int PIN_SERVO     = 18; // PWM-capable GPIO

// -------- Servo limits & behavior --------
static const int SERVO_MIN_DEG = 10;   // protect mechanics
static const int SERVO_MAX_DEG = 170;  // protect mechanics
static const int SERVO_START_DEG = 90;

static const int SERVO_STEP_DEG = 1;   // how much to move each adjustment
static const int DEAD_BAND = 80;       // ignore small differences (tune)

// Smoothing: 0..1 (higher = smoother/less jitter)
static const float SMOOTHING_ALPHA = 0.85f;

// Loop timing
static const uint32_t LOOP_DELAY_MS = 25;

// Serial debug
static const bool DEBUG_SERIAL = true;
static const uint32_t DEBUG_EVERY_MS = 250;

Servo servo;

// Exponential moving averages for stability
float leftAvg = 0.0f;
float rightAvg = 0.0f;

int clampInt(int value, int lo, int hi) {
  if (value < lo) return lo;
  if (value > hi) return hi;
  return value;
}

float ema(float prev, float sample, float alpha) {
  return (alpha * prev) + ((1.0f - alpha) * sample);
}

void setup() {
  if (DEBUG_SERIAL) {
    Serial.begin(115200);
    delay(300);
    Serial.println("\nESP32 Solar Tracker starting...");
  }

  // Analog setup: ESP32 uses 12-bit ADC by default (0..4095)
  // You can adjust attenuation if needed.
  analogReadResolution(12);

  // Servo setup
  // Typical servo pulse is ~500-2400us. Adjust if your servo range is different.
  servo.setPeriodHertz(50);
  servo.attach(PIN_SERVO, 500, 2400);

  int start = clampInt(SERVO_START_DEG, SERVO_MIN_DEG, SERVO_MAX_DEG);
  servo.write(start);

  // Initialize averages with first readings to avoid a big jump
  int l0 = analogRead(PIN_LDR_LEFT);
  int r0 = analogRead(PIN_LDR_RIGHT);
  leftAvg = (float)l0;
  rightAvg = (float)r0;

  if (DEBUG_SERIAL) {
    Serial.printf("Initial LDR left=%d right=%d\n", l0, r0);
    Serial.printf("Servo pin=%d start=%ddeg\n", PIN_SERVO, start);
  }
}

// Global or static tracking
static int setpoint = SERVO_START_DEG;

void loop() {
  const int leftRaw = analogRead(PIN_LDR_LEFT);
  const int rightRaw = analogRead(PIN_LDR_RIGHT);

  // Smooth readings
  leftAvg = ema(leftAvg, (float)leftRaw, SMOOTHING_ALPHA);
  rightAvg = ema(rightAvg, (float)rightRaw, SMOOTHING_ALPHA);

  // Difference: positive means LEFT is brighter
  const int diff = (int)(leftAvg - rightAvg);

  // Move servo to reduce difference
  if (diff > DEAD_BAND) {
    // left side brighter => rotate toward left (increase or decrease depends on mounting)
    // Assuming increasing angle moves left
    setpoint += SERVO_STEP_DEG;
  } else if (diff < -DEAD_BAND) {
    // right brighter => rotate toward right
    setpoint -= SERVO_STEP_DEG;
  }

  setpoint = clampInt(setpoint, SERVO_MIN_DEG, SERVO_MAX_DEG);
  servo.write(setpoint);

  // Debug output
  static uint32_t lastDbg = 0;
  const uint32_t now = millis();
  if (DEBUG_SERIAL && (now - lastDbg) >= DEBUG_EVERY_MS) {
    lastDbg = now;
    Serial.printf("L=%d(%.0f) R=%d(%.0f) diff=%d angle=%d\n",
                  leftRaw, leftAvg, rightRaw, rightAvg, diff, setpoint);
  }

  delay(LOOP_DELAY_MS);
}
