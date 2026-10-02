/*
 * File:   dht22.h
 * Author: polteo
 * 
 * DHT22 Temperature & Humidity Sensor Driver Header
 */

#ifndef DHT22_H
#define DHT22_H

#include <xc.h>
#include <stdint.h>
#include "config.h"

#define DHT22_TIMEOUT       100     // microseconds per edge wait

extern volatile float dht22_temperature;
extern volatile float dht22_humidity;

void dht22_init(void);
uint8_t dht22_read_data(void);      // returns 1 on success, 0 on error
uint8_t dht22_read_byte(void);

#endif // DHT22_H
