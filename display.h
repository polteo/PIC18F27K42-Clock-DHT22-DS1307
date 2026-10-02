/*
 * File:   display.h
 * 4-digit 7-segment display driven by 4 daisy-chained 74HC595
 * (one '595 per digit). Segment bits: Q0=a Q1=b Q2=c Q3=d Q4=e Q5=f Q6=g Q7=dp
 * Chain order: PIC SER -> '595 of digit 0 (leftmost) -> ... -> digit 3 (rightmost).
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "config.h"

#define DISPLAY_DIGITS  4

// Display cycle states
#define DISPLAY_TIME        0
#define DISPLAY_DATE        1
#define DISPLAY_YEAR        2
#define DISPLAY_TEMP        3
#define DISPLAY_HUMIDITY    4

// Decimal point / colon modes
#define NO_DOT          0   // no dots
#define SHOW_COLON      1   // dp of digit 1 (HH.MM)
#define SHOW_DOT        2   // dp of digit 1 (DD.MM)
#define SHOW_DOT_TENTHS 3   // dp of digit 2 (XX.X)

void display_init(void);
void display_write_digits(const uint8_t *digits, uint8_t dot_mode);
void display_refresh(void);     // shifts out the buffer if it changed

#endif // DISPLAY_H
