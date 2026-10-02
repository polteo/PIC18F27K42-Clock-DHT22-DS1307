/*
 * File:   Makefile
 * Build file for PIC18F27K42 project
 */

# Compiler
CC = xc8
DEVICE = 18F27K42

# Compiler flags
CFLAGS = --chip=$(DEVICE) -Wall -O2

# Source files
SOURCES = main.c i2c.c ds1307.c dht22.c display.c timer.c alarm.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output
OUTPUT = clock_firmware.hex

# Build target
all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

clean:
	rm -f *.o *.hex *.sym *.lst

.PHONY: all clean
