/*
 * File:   timer.c
 * Timer0: 16-bit, clock Fosc/4 (4 MHz), prescaler 1:4 -> 1 MHz, 10 ms period.
 */

#include "timer.h"

void timer_init(void) {
    T0CON0 = 0x10;              // 16-bit mode, 1:1 postscaler, disabled
    T0CON1 = 0x42;              // Fosc/4, synchronized, prescaler 1:4
    TMR0H = TIMER0_RELOAD_H;
    TMR0L = TIMER0_RELOAD_L;
    PIR0bits.TMR0IF = 0;
    PIE0bits.TMR0IE = 1;
    T0CON0bits.T0EN = 1;
}
