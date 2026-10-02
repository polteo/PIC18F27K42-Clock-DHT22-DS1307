/*
 * File:   alarm.c
 */

#include "alarm.h"

volatile uint8_t alarm_hour = 7;
volatile uint8_t alarm_minute = 0;
volatile uint8_t alarm_enabled = 0;

void alarm_init(void) {
    BUZZER_LAT = 0;
    BUZZER_TRIS = 0;
}

void alarm_on(void)  { BUZZER_LAT = 1; }
void alarm_off(void) { BUZZER_LAT = 0; }

void alarm_toggle(void) {
    alarm_enabled = !alarm_enabled;
    if(!alarm_enabled) alarm_off();
}

uint8_t alarm_is_enabled(void) { return alarm_enabled; }

void alarm_set_time(uint8_t hour, uint8_t minute) {
    alarm_hour = hour % 24;
    alarm_minute = minute % 60;
}
