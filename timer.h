/*
 * File:   timer.h
 * Timer0 10 ms system tick
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "config.h"

#define TIMER0_RELOAD_H  0xD8   // 65536 - 10000 = 0xD8F0 (10 ms @ 1 MHz timer clock)
#define TIMER0_RELOAD_L  0xF0

void timer_init(void);

#endif // TIMER_H
