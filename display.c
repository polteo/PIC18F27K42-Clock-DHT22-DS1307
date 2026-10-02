/*
 * File:   display.c
 */

#include "display.h"

#define SEG_DP  0x80

static const uint8_t seg_table[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static uint8_t frame[DISPLAY_DIGITS];
static uint8_t shown[DISPLAY_DIGITS];
static uint8_t dirty = 1;

static void shift_byte(uint8_t data) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        SR_DATA_LAT = (data & 0x80) ? 1 : 0;
        SR_CLK_LAT = 1;
        SR_CLK_LAT = 0;
        data <<= 1;
    }
}

void display_init(void) {
    uint8_t i;
    SR_DATA_LAT = 0;
    SR_CLK_LAT = 0;
    SR_LATCH_LAT = 0;
    SR_DATA_TRIS = 0;
    SR_CLK_TRIS = 0;
    SR_LATCH_TRIS = 0;
    for(i = 0; i < DISPLAY_DIGITS; i++) {
        shown[i] = 0xFF;                // force first update
        frame[i] = 0;
    }
    dirty = 1;
    display_refresh();
}

void display_write_digits(const uint8_t *digits, uint8_t dot_mode) {
    uint8_t i;
    for(i = 0; i < DISPLAY_DIGITS; i++) {
        uint8_t v = (digits[i] < 10) ? seg_table[digits[i]] : 0;
        if((dot_mode == SHOW_COLON || dot_mode == SHOW_DOT) && i == 1) v |= SEG_DP;
        if(dot_mode == SHOW_DOT_TENTHS && i == 2) v |= SEG_DP;
#if !DISPLAY_COMMON_CATHODE
        v = (uint8_t)~v;
#endif
        frame[i] = v;
    }
}

void display_refresh(void) {
    int8_t i;
    uint8_t changed = 0;
    for(i = 0; i < DISPLAY_DIGITS; i++) {
        if(frame[i] != shown[i]) changed = 1;
    }
    if(!changed && !dirty) return;

    // Rightmost digit is last in the chain, so it is shifted first
    for(i = DISPLAY_DIGITS - 1; i >= 0; i--) {
        shift_byte(frame[i]);
        shown[i] = frame[i];
    }
    SR_LATCH_LAT = 1;
    SR_LATCH_LAT = 0;
    dirty = 0;
}
