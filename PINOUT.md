# Pinout and connection guide (PIC18F27K42, 28-pin SPDIP/SOIC)

All pins are defined in **one place: `config.h`**. Change a pin there and nowhere else.
Target MCU is unchanged (PIC18F27K42); a smaller device such as the PIC16F18323 would require porting
the register names (oscillator, Timer0, interrupt registers) and is not part of this mapping.

| PIC pin | Port | Function | Connects to | Macros (`config.h`) |
|---|---|---|---|---|
| 2  | RA0 | DHT22 DATA | DHT22 pin 2 (+4.7k pull-up to 3.3/5V) | `DHT22_*` |
| 21 | RB0 | 74HC595 SER (data) | SER (pin 14) of digit 0 '595 | `SR_DATA_*` |
| 22 | RB1 | 74HC595 SRCLK (shift clock) | pin 11 of all '595 | `SR_CLK_*` |
| 23 | RB2 | 74HC595 RCLK (latch) | pin 12 of all '595 | `SR_LATCH_*` |
| 24 | RB3 | Buzzer (active, via transistor) | buzzer + | `BUZZER_*` |
| 25 | RB4 | Button: alarm on/off | to GND (internal pull-up) | `BTN_ALARM_*` |
| 26 | RB5 | Button: stop buzzer | to GND (internal pull-up) | `BTN_STOP_*` |
| 18 | RC3 | DS1307 SDA (software I2C) | DS1307 SDA (pin 5) + 4.7k pull-up | `I2C_SDA_*` |
| 15 | RC4 | DS1307 SCL (software I2C) | DS1307 SCL (pin 6) + 4.7k pull-up | `I2C_SCL_*` |
| 1  | MCLR | Reset | 10k to VDD / programmer | – |
| 20 / 8,19 | VDD / VSS | Power | 5 V (or 3.3 V) / GND, 100 nF decoupling | – |

> RC3 is SDA and RC4 is SCL (matching the original `TRISC = 0x18` comment). I2C is bit-banged, so the
> hardware MSSP is not used. Verify pin numbers against the datasheet for your package.

## 74HC595 / 7-segment display (one '595 per digit, daisy-chained)

PIC `RB0` → '595 #0 `SER`; each '595 `QH'` (pin 9) → next '595 `SER`. `SRCLK` and `RCLK` are shared by all four.
Tie `OE` (pin 13) to GND and `SRCLR` (pin 10) to VCC.

| '595 output | Segment |
|---|---|
| Q0 | a |
| Q1 | b |
| Q2 | c |
| Q3 | d |
| Q4 | e |
| Q5 | f |
| Q6 | g |
| Q7 | dp |

'595 #0 = leftmost digit (hours tens) … '595 #3 = rightmost digit. Use a current-limiting resistor
(~330 Ω) per segment. Set `DISPLAY_COMMON_CATHODE` in `config.h` to `0` for common-anode displays.
The colon / decimal point is the `dp` segment of digit 1 (HH.MM, DD.MM) or digit 2 (temperature XX.X).
Because every digit has its own latch, the display is statically driven (no multiplexing).

## Buttons
RB4 toggles the alarm (default alarm time 07:00, set via `alarm_set_time()`); RB5 silences the buzzer.
