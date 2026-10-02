/*
 * File:   ds1307.h
 * DS1307 RTC driver (I2C address 0x68)
 */

#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>
#include "config.h"

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;      // 24h
    uint8_t weekday;
    uint8_t day;
    uint8_t month;
    uint8_t year;       // 00-99
} rtc_time_t;

extern volatile rtc_time_t rtc_time;

void ds1307_init(void);
void ds1307_read_time(void);
void ds1307_write_time(const rtc_time_t *t);

#endif // DS1307_H
