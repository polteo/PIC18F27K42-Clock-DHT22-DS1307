# PIC18F27K42-Clock-DHT22-DS1307
Digital Clock with 4x7-segment displays, RTC (DS1307), DHT22 temperature/humidity sensor, and alarm functionality using PIC18F27K42 in XC8

## Hardware
All pin assignments are centralized in `config.h` and documented in [PINOUT.md](PINOUT.md)
(DHT22 on RA0, DS1307 I2C on RC3/RC4, 74HC595 chain on RB0-RB2, buzzer on RB3, buttons on RB4/RB5).
