/*
 * File:   dht22.c
 * DHT22 driver, single-wire on DHT22_* pin (config.h).
 * Bus is released (input) when idle; external 4.7k pull-up required.
 */

#include "dht22.h"

volatile float dht22_temperature = 0.0f;
volatile float dht22_humidity = 0.0f;

static uint8_t dht22_timeout;

// Wait until the pin equals 'level'; returns elapsed us (0 on timeout)
static uint8_t wait_level(uint8_t level) {
    uint8_t t = 1;
    while((DHT22_PORT ? 1 : 0) != level) {
        __delay_us(1);
        if(++t >= DHT22_TIMEOUT) {
            dht22_timeout = 1;
            return 0;
        }
    }
    return t;
}

void dht22_init(void) {
    DHT22_LAT = 0;
    DHT22_TRIS = 1;
}

uint8_t dht22_read_byte(void) {
    uint8_t i, v = 0;
    for(i = 0; i < 8; i++) {
        wait_level(1);                  // end of 50us low
        __delay_us(40);                 // '0' ~26us high, '1' ~70us high
        v <<= 1;
        if(DHT22_PORT) v |= 1;
        wait_level(0);                  // end of bit
    }
    return v;
}

uint8_t dht22_read_data(void) {
    uint8_t d[5];
    uint8_t i, gie;
    int16_t raw;

    dht22_timeout = 0;

    // Start signal: pull low >= 1ms, then release
    DHT22_LAT = 0;
    DHT22_TRIS = 0;
    __delay_ms(2);
    DHT22_TRIS = 1;
    __delay_us(30);

    gie = INTCON0bits.GIE;
    INTCON0bits.GIE = 0;                 // timing critical section

    wait_level(0);                      // sensor response low
    wait_level(1);                      // response high
    wait_level(0);                      // start of first bit
    if(!dht22_timeout) {
        for(i = 0; i < 5; i++) {
            d[i] = dht22_read_byte();
        }
    }

    INTCON0bits.GIE = gie;

    if(dht22_timeout) return 0;
    if((uint8_t)(d[0] + d[1] + d[2] + d[3]) != d[4]) return 0;

    dht22_humidity = (float)(((uint16_t)d[0] << 8) | d[1]) / 10.0f;
    raw = (int16_t)(((uint16_t)(d[2] & 0x7F) << 8) | d[3]);
    dht22_temperature = (float)raw / 10.0f;
    if(d[2] & 0x80) dht22_temperature = -dht22_temperature;
    return 1;
}
