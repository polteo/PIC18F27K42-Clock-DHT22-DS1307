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
volatile uint8_t tick_flag = 0;     // set every 10 ms by Timer0
volatile uint8_t alarm_triggered = 0;

// Function prototypes
void init_system(void);
void display_manager(void);
void alarm_check(void);
void buttons_check(void);

void main(void) {
    uint8_t sensor_ticks = 200;     // read sensors immediately, then every 2 s

    init_system();
    
    // Main loop
    while(1) {
        if(tick_flag) {
            tick_flag = 0;
            
            if(++sensor_ticks >= 200) {
                sensor_ticks = 0;
                dht22_read_data();
            }
            if(sensor_ticks % 100 == 0) {   // RTC once per second
                ds1307_read_time();
                alarm_check();
            }
            buttons_check();
            display_manager();
        }
    }
}

void init_system(void) {
    // Clock configuration: HFINTOSC 16 MHz (pin map is in config.h)
    OSCCON1 = 0x60;     // HFINTOSC, no divider
    OSCFRQ  = 0x05;     // 16 MHz
    
    // Disable analog inputs
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    
    // Buttons: inputs with weak pull-ups (active low)
    BTN_ALARM_TRIS = 1;
    BTN_STOP_TRIS = 1;
    BTN_ALARM_WPU = 1;
    BTN_STOP_WPU = 1;
    
    // Initialize modules (each configures its own pins from config.h)
    i2c_init();
    ds1307_init();
    dht22_init();
    display_init();
    timer_init();
    alarm_init();
    
    // Enable global interrupts
    INTCON0bits.GIE = 1;
}

void buttons_check(void) {
    static uint8_t alarm_prev = 1, stop_prev = 1;
    uint8_t a = BTN_ALARM_PORT, s = BTN_STOP_PORT;

    if(!a && alarm_prev) {
        alarm_toggle();
        alarm_triggered = 0;
    }
    if(!s && stop_prev) {
        alarm_off();
    }
    alarm_prev = a;
    stop_prev = s;
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
            {
            uint16_t temp_display = (uint16_t)(dht22_temperature * 10);
            buffer[0] = (temp_display / 1000) % 10;
            buffer[1] = (temp_display / 100) % 10;
            buffer[2] = (temp_display / 10) % 10;
            buffer[3] = temp_display % 10;
            display_write_digits(buffer, SHOW_DOT_TENTHS);
            }
            
            cycle_timer++;
            if(cycle_timer >= 300) {  // 3 seconds
                cycle_timer = 0;
                display_state = DISPLAY_HUMIDITY;
            }
            break;
            
        case DISPLAY_HUMIDITY:
            // Show humidity (3 seconds)
            // Format: XX% displayed as 00XX
            {
            uint16_t humidity_display = (uint16_t)dht22_humidity;
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = (humidity_display / 10) % 10;
            buffer[3] = humidity_display % 10;
            display_write_digits(buffer, NO_DOT);
            }
            
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
    
    // Shift the frame out to the 74HC595 chain (static drive, no multiplexing)
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
        } else if(rtc_time.minutes != alarm_minute) {
            alarm_triggered = 0;  // re-arm for the next day
        }
    }
}

// Interrupt handler for timer/display refresh
void __interrupt() high_isr(void) {
    if(PIR0bits.TMR0IF) {
        PIR0bits.TMR0IF = 0;
        TMR0H = TIMER0_RELOAD_H;  // Reload for 10ms interrupt
        TMR0L = TIMER0_RELOAD_L;
        
        tick_flag = 1;
        display_timer++;
        
        if(display_timer >= 100) {  // Every 1 second
            display_timer = 0;
        }
    }
}
