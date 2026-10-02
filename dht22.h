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

#define DHT22_MAX_TRIES     100
#define DHT22_TIMEOUT       1000    // microseconds

volatile float dht22_temperature;
volatile float dht22_humidity;

void dht22_init(void);
void dht22_read_data(void);
void dht22_delay_us(uint16_t us);
uint8_t dht22_read_byte(void);

#endif // DHT22_H
