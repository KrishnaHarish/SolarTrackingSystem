/*
  ESP32 Servo Sweep Test

  Use this to verify:
  - GPIO pin is correct
  - Servo power is stable (use external 5V)
  - Common ground between external supply and ESP32

  Library:
  - ESP32Servo (Arduino Library Manager)
*/

#include <Arduino.h>
#include <ESP32Servo.h>

static const int PIN_SERVO = 18; // your servo signal pin

static const int SERVO_MIN_DEG = 10;
static const int SERVO_MAX_DEG = 170;
static const int STEP_DEG = 2;
static const uint32_t STEP_DELAY_MS = 20;

Servo servo;

int clampInt(int value, int lo, int hi) {
  if (value < lo) return lo;
  if (value > hi) return hi;
  return value;
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nESP32 Servo Sweep Test");

  servo.setPeriodHertz(50);
  servo.attach(PIN_SERVO, 500, 2400);

  int start = clampInt(90, SERVO_MIN_DEG, SERVO_MAX_DEG);
  servo.write(start);
  Serial.printf("Servo on GPIO%d, start=%ddeg\n", PIN_SERVO, start);
}

void loop() {
  // Sweep up
  for (int a = SERVO_MIN_DEG; a <= SERVO_MAX_DEG; a += STEP_DEG) {
    servo.write(a);
    delay(STEP_DELAY_MS);
  }

  delay(300);

  // Sweep down
  for (int a = SERVO_MAX_DEG; a >= SERVO_MIN_DEG; a -= STEP_DEG) {
    servo.write(a);
    delay(STEP_DELAY_MS);
  }

  delay(300);
}
