/*
 * File:   i2c.c
 * Software I2C master (~100 kHz at 16 MHz). Lines are open-drain:
 * driven low via TRIS=0/LAT=0, released (high via pull-up) via TRIS=1.
 */

#include "i2c.h"

#define I2C_DELAY()     __delay_us(5)

static void sda_high(void) { I2C_SDA_TRIS = 1; }
static void sda_low(void)  { I2C_SDA_LAT = 0; I2C_SDA_TRIS = 0; }

static void scl_high(void) {
    uint8_t t = 100;
    I2C_SCL_TRIS = 1;
    while(!I2C_SCL_PORT && --t) {   // clock stretching with timeout
        __delay_us(1);
    }
}
static void scl_low(void)  { I2C_SCL_LAT = 0; I2C_SCL_TRIS = 0; }

void i2c_init(void) {
    I2C_SDA_LAT = 0;
    I2C_SCL_LAT = 0;
    sda_high();
    I2C_SCL_TRIS = 1;
    I2C_DELAY();
}

void i2c_start(void) {
    sda_high();
    scl_high();
    I2C_DELAY();
    sda_low();
    I2C_DELAY();
    scl_low();
}

void i2c_stop(void) {
    sda_low();
    I2C_DELAY();
    scl_high();
    I2C_DELAY();
    sda_high();
    I2C_DELAY();
}

uint8_t i2c_write(uint8_t data) {
    uint8_t i, ack;
    for(i = 0; i < 8; i++) {
        if(data & 0x80) sda_high(); else sda_low();
        data <<= 1;
        I2C_DELAY();
        scl_high();
        I2C_DELAY();
        scl_low();
    }
    sda_high();
    I2C_DELAY();
    scl_high();
    I2C_DELAY();
    ack = !I2C_SDA_PORT;
    scl_low();
    return ack;
}

uint8_t i2c_read(uint8_t ack) {
    uint8_t i, data = 0;
    sda_high();
    for(i = 0; i < 8; i++) {
        data <<= 1;
        I2C_DELAY();
        scl_high();
        if(I2C_SDA_PORT) data |= 1;
        I2C_DELAY();
        scl_low();
    }
    if(ack) sda_low(); else sda_high();
    I2C_DELAY();
    scl_high();
    I2C_DELAY();
    scl_low();
    sda_high();
    return data;
}
