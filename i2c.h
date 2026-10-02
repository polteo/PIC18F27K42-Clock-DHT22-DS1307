/*
 * File:   i2c.h
 * Software (bit-banged) I2C master, pins defined in config.h
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "config.h"

void    i2c_init(void);
void    i2c_start(void);
void    i2c_stop(void);
uint8_t i2c_write(uint8_t data);        // returns 1 if ACKed
uint8_t i2c_read(uint8_t ack);          // ack=1 -> send ACK, 0 -> NACK

#endif // I2C_H
