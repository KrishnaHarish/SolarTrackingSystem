# ESP32 Solar Tracker (1 Servo + 2 LDR)

## What you need
- ESP32 dev board (shows up as **COM3** on your PC)
- 1 servo motor (SG90/MG90S/etc)
- 2 LDR (photoresistors)
- 2 resistors (10k is a good start)
- External 5V supply for servo (phone charger / 5V adapter)

## Wiring (important)
### Servo
- Servo **VCC (red)** -> **5V external**
- Servo **GND (brown/black)** -> **external GND**
- Servo **SIGNAL (orange/yellow)** -> ESP32 **GPIO18** (default in code)
- Connect **external GND** to **ESP32 GND** (common ground). This is required.

### LDR voltage dividers (2x)
Each LDR needs a divider so ESP32 can read a voltage.

For LEFT sensor (same for RIGHT):
- **3.3V -> LDR -> (junction) -> 10k resistor -> GND**
- Connect the **junction** to an ESP32 analog pin:
  - LEFT junction -> **GPIO34**
  - RIGHT junction -> **GPIO35**

Notes:
- GPIO34/35 are input-only (perfect for analog).
- Prefer **ADC1 pins** (GPIO32–39) on ESP32.

## Upload steps (Arduino IDE)
1. Install **ESP32 boards** in Arduino IDE (Boards Manager).
2. Install library: **ESP32Servo** (Library Manager).
3. Open `solar_tracker_esp32.ino`.
4. Tools:
   - Board: **ESP32 Dev Module** (or your specific ESP32)
   - Port: **COM3**
5. Click **Upload**.

## Tuning / fixing direction
Open Serial Monitor at **115200**.

If the servo turns the wrong way (moves away from light):
- Swap the movement directions in code by flipping the angle updates:
  - Change `angle += SERVO_STEP_DEG;` to `angle -= SERVO_STEP_DEG;` (and vice versa)

If it jitters too much:
- Increase `DEAD_BAND` (e.g., 120–250)
- Increase `SMOOTHING_ALPHA` (e.g., 0.90–0.95)
- Reduce `SERVO_STEP_DEG` to 1 (already default)

## Common problems
- **Servo doesn’t move / ESP32 resets**: servo is drawing too much current from USB/ESP32. Use external 5V and common ground.
- **Analog readings stuck at 0/4095**: divider wiring wrong or wrong pin.
- **No COM3**: try different USB cable (data cable), install driver if needed.
