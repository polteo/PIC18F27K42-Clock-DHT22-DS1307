/*
 * File:   config.h
 * Central hardware configuration and pin map for PIC18F27K42.
 * ALL pin assignments live in this file - see also PINOUT.md.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ  16000000UL      // 16 MHz HFINTOSC

/* ---------------- DHT22 (single wire, external 4.7k pull-up) ---------------- */
#define DHT22_TRIS      TRISAbits.TRISA0
#define DHT22_LAT       LATAbits.LATA0
#define DHT22_PORT      PORTAbits.RA0

/* ---------------- DS1307 (software I2C, external 4.7k pull-ups) --------------- */
#define I2C_SDA_TRIS    TRISCbits.TRISC3
#define I2C_SDA_LAT     LATCbits.LATC3
#define I2C_SDA_PORT    PORTCbits.RC3
#define I2C_SCL_TRIS    TRISCbits.TRISC4
#define I2C_SCL_LAT     LATCbits.LATC4
#define I2C_SCL_PORT    PORTCbits.RC4

/* ---------------- 74HC595 chain (one '595 per 7-segment digit) --------------- */
#define SR_DATA_TRIS    TRISBbits.TRISB0    // SER   (pin 14 of first '595)
#define SR_DATA_LAT     LATBbits.LATB0
#define SR_CLK_TRIS     TRISBbits.TRISB1    // SRCLK (pin 11, all '595)
#define SR_CLK_LAT      LATBbits.LATB1
#define SR_LATCH_TRIS   TRISBbits.TRISB2    // RCLK  (pin 12, all '595)
#define SR_LATCH_LAT    LATBbits.LATB2

/* ---------------- Buzzer ---------------- */
#define BUZZER_TRIS     TRISBbits.TRISB3
#define BUZZER_LAT      LATBbits.LATB3

/* ---------------- Buttons (active low, internal weak pull-up) ---------------- */
#define BTN_ALARM_TRIS  TRISBbits.TRISB4    // toggle alarm on/off
#define BTN_ALARM_WPU   WPUBbits.WPUB4
#define BTN_ALARM_PORT  PORTBbits.RB4
#define BTN_STOP_TRIS   TRISBbits.TRISB5    // silence buzzer
#define BTN_STOP_WPU    WPUBbits.WPUB5
#define BTN_STOP_PORT   PORTBbits.RB5

/* Display polarity: 1 = common-cathode digits, 0 = common-anode */
#define DISPLAY_COMMON_CATHODE  1

#endif // CONFIG_H
