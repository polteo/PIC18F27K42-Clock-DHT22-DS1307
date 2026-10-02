/*
 * File:   ds1307.c
 */

#include "ds1307.h"
#include "i2c.h"

#define DS1307_ADDR_W   0xD0
#define DS1307_ADDR_R   0xD1

volatile rtc_time_t rtc_time = {0, 0, 0, 1, 1, 1, 0};

static uint8_t bcd2bin(uint8_t v) { return (uint8_t)((v >> 4) * 10 + (v & 0x0F)); }
static uint8_t bin2bcd(uint8_t v) { return (uint8_t)(((v / 10) << 4) | (v % 10)); }

void ds1307_init(void) {
    uint8_t sec;
    i2c_start();
    i2c_write(DS1307_ADDR_W);
    i2c_write(0x00);
    i2c_start();
    i2c_write(DS1307_ADDR_R);
    sec = i2c_read(0);
    i2c_stop();

    if(sec & 0x80) {                // clock halt bit set -> start oscillator
        i2c_start();
        i2c_write(DS1307_ADDR_W);
        i2c_write(0x00);
        i2c_write(sec & 0x7F);
        i2c_stop();
    }
}

void ds1307_read_time(void) {
    uint8_t r[7];
    uint8_t i;
    i2c_start();
    i2c_write(DS1307_ADDR_W);
    i2c_write(0x00);
    i2c_start();
    i2c_write(DS1307_ADDR_R);
    for(i = 0; i < 7; i++) {
        r[i] = i2c_read(i < 6);
    }
    i2c_stop();

    rtc_time.seconds = bcd2bin(r[0] & 0x7F);
    rtc_time.minutes = bcd2bin(r[1] & 0x7F);
    rtc_time.hours   = bcd2bin(r[2] & 0x3F);
    rtc_time.weekday = bcd2bin(r[3] & 0x07);
    rtc_time.day     = bcd2bin(r[4] & 0x3F);
    rtc_time.month   = bcd2bin(r[5] & 0x1F);
    rtc_time.year    = bcd2bin(r[6]);
}

void ds1307_write_time(const rtc_time_t *t) {
    i2c_start();
    i2c_write(DS1307_ADDR_W);
    i2c_write(0x00);
    i2c_write(bin2bcd(t->seconds) & 0x7F);
    i2c_write(bin2bcd(t->minutes));
    i2c_write(bin2bcd(t->hours));
    i2c_write(bin2bcd(t->weekday));
    i2c_write(bin2bcd(t->day));
    i2c_write(bin2bcd(t->month));
    i2c_write(bin2bcd(t->year));
    i2c_stop();
}
