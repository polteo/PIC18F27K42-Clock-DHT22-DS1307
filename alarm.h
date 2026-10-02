/*
 * File:   alarm.h
 * Author: polteo
 * 
 * Alarm and Buzzer Control Header (buzzer pin is defined in config.h)
 */

#ifndef ALARM_H
#define ALARM_H

#include <xc.h>
#include <stdint.h>
#include "config.h"

extern volatile uint8_t alarm_hour;
extern volatile uint8_t alarm_minute;
extern volatile uint8_t alarm_enabled;

void alarm_init(void);
void alarm_on(void);
void alarm_off(void);
void alarm_toggle(void);
uint8_t alarm_is_enabled(void);
void alarm_set_time(uint8_t hour, uint8_t minute);

#endif // ALARM_H
