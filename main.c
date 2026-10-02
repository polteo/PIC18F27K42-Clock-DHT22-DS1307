/*
 * File:   main.c
 * Author: polteo
 * 
 * Created on October 2, 2026
 * 
 * PIC18F27K42 - Digital Clock with DS1307, DHT22, 4x7-segment display
 * Display Cycle: Time(5s) -> Date(3s) -> Year(3s) -> Temp(3s) -> Humidity(3s)
 * Alarm functionality included
 */

#include <xc.h>
#include "config.h"
#include "i2c.h"
#include "ds1307.h"
#include "dht22.h"
#include "display.h"
#include "timer.h"
#include "alarm.h"

// Global variables
volatile uint8_t display_state = DISPLAY_TIME;
volatile uint16_t display_timer = 0;
volatile uint8_t refresh_flag = 0;
volatile uint8_t alarm_triggered = 0;

// Function prototypes
void init_system(void);
void display_manager(void);
void alarm_check(void);

void main(void) {
    init_system();
    
    // Main loop
    while(1) {
        // Read sensors periodically (every 2 seconds)
        if(refresh_flag) {
            refresh_flag = 0;
            ds1307_read_time();
            dht22_read_data();
            alarm_check();
        }
        
        // Update display
        display_manager();
    }
}

void init_system(void) {
    // Clock configuration
    OSCCON = 0x70;  // 16MHz internal oscillator
    
    // Port configuration
    TRISA = 0xFF;   // RA all inputs (DHT22 on RA0)
    TRISB = 0x30;   // RB4, RB5 inputs (buttons), others outputs
    TRISC = 0x18;   // RC3, RC4 I2C (SDA, SCL)
    
    // Disable analog inputs
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    
    // Initialize modules
    i2c_init();
    ds1307_init();
    dht22_init();
    display_init();
    timer_init();
    alarm_init();
    
    // Enable global interrupts
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}

void display_manager(void) {
    static uint16_t cycle_timer = 0;
    uint8_t buffer[4];
    
    // State machine for display cycling
    switch(display_state) {
        case DISPLAY_TIME:
            // Show time: HH:MM (5 seconds)
            buffer[0] = (rtc_time.hours / 10) % 10;
            buffer[1] = rtc_time.hours % 10;
            buffer[2] = (rtc_time.minutes / 10) % 10;
            buffer[3] = rtc_time.minutes % 10;
            display_write_digits(buffer, SHOW_COLON);
            
            cycle_timer++;
            if(cycle_timer >= 500) {  // 5 seconds (10ms per tick)
                cycle_timer = 0;
                display_state = DISPLAY_DATE;
            }
            break;
            
        case DISPLAY_DATE:
            // Show date: DD.MM (3 seconds)
            buffer[0] = (rtc_time.day / 10) % 10;
            buffer[1] = rtc_time.day % 10;
            buffer[2] = (rtc_time.month / 10) % 10;
            buffer[3] = rtc_time.month % 10;
            display_write_digits(buffer, SHOW_DOT);
            
            cycle_timer++;
            if(cycle_timer >= 300) {  // 3 seconds
                cycle_timer = 0;
                display_state = DISPLAY_YEAR;
            }
            break;
            
        case DISPLAY_YEAR:
            // Show year: 20YY (3 seconds)
            buffer[0] = 2;
            buffer[1] = 0;
            buffer[2] = (rtc_time.year / 10) % 10;
            buffer[3] = rtc_time.year % 10;
            display_write_digits(buffer, NO_DOT);
            
            cycle_timer++;
            if(cycle_timer >= 300) {  // 3 seconds
                cycle_timer = 0;
                display_state = DISPLAY_TEMP;
            }
            break;
            
        case DISPLAY_TEMP:
            // Show temperature (3 seconds)
            // Format: XX.X°C or XX.X displayed as XXXX
            uint16_t temp_display = (uint16_t)(dht22_temperature * 10);
            buffer[0] = (temp_display / 1000) % 10;
            buffer[1] = (temp_display / 100) % 10;
            buffer[2] = (temp_display / 10) % 10;
            buffer[3] = temp_display % 10;
            display_write_digits(buffer, SHOW_DOT);
            
            cycle_timer++;
            if(cycle_timer >= 300) {  // 3 seconds
                cycle_timer = 0;
                display_state = DISPLAY_HUMIDITY;
            }
            break;
            
        case DISPLAY_HUMIDITY:
            // Show humidity (3 seconds)
            // Format: XX% displayed as 00XX
            uint16_t humidity_display = (uint16_t)dht22_humidity;
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = (humidity_display / 10) % 10;
            buffer[3] = humidity_display % 10;
            display_write_digits(buffer, NO_DOT);
            
            cycle_timer++;
            if(cycle_timer >= 300) {  // 3 seconds
                cycle_timer = 0;
                display_state = DISPLAY_TIME;  // Loop back
            }
            break;
            
        default:
            display_state = DISPLAY_TIME;
            break;
    }
    
    // Refresh display (multiplexing at ~1kHz)
    display_refresh();
}

void alarm_check(void) {
    if(alarm_is_enabled()) {
        if(alarm_triggered == 0 && 
           rtc_time.hours == alarm_hour && 
           rtc_time.minutes == alarm_minute && 
           rtc_time.seconds == 0) {
            alarm_triggered = 1;
            alarm_on();  // Start buzzer/LED
        }
    }
}

// Interrupt handler for timer/display refresh
void __interrupt(high_priority) high_isr(void) {
    if(PIR0bits.TMR0IF) {
        PIR0bits.TMR0IF = 0;
        TMR0H = 0xFC;  // Reload for 10ms interrupt
        TMR0L = 0x18;
        
        refresh_flag = 1;
        display_timer++;
        
        if(display_timer >= 100) {  // Every 1 second
            display_timer = 0;
        }
    }
}
