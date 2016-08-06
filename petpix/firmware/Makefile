# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected. We recommend that you leave it undefined and
#                add settings like this to your ~/.avrduderc file:
#                   default_programmer = "stk500v2"
#                   default_serial = "avrdoper"
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

MASTERPORT = $(shell cat master.port)
SPAREPORT = $(shell cat spare.port)

DEVICE     = atmega644
CLOCK      = 8000000
PROGRAMMER = -c stk500v2 -P $(MASTERPORT)
OBJECTS    = PETpix2.o UART_routines.o
#FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x24:m
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0xe2:m

PORTS = $(shell ls /dev/tty.usb*)

# ATMega8 fuse bits used above (fuse bits for other devices are different!):
# Example for 8 MHz internal oscillator
# Fuse high byte:
# 0xd9 = 1 1 0 1   1 0 0 1 <-- BOOTRST (boot reset vector at 0x0000)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   +---------- EESAVE (set to 0 to preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (clock option, depends on oscillator type)
#        | | +---------------- SPIEN (if set to 1, serial programming is disabled)
#        | +------------------ WDTON (if set to 0, watchdog is always on)
#        +-------------------- RSTDISBL (if set to 0, RESET pin is disabled)
# Fuse low byte:
# 0x24 = 0 0 1 0   0 1 0 0
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (8M internal RC)
#        | |  +--------------- SUT 1..0 (slowly rising power)
#        | +------------------ BODEN (if 0, brown-out detector is enabled)
#        +-------------------- BODLEVEL (if 0: 4V, if 1: 2.7V)
#
# For computing fuse byte values for other devices and options see
# the fuse bit calculator at http://www.engbedded.com/fusecalc/


# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -std=gnu99

# symbolic targets:
all:	PETpix2.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:PETpix2.hex

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID PETpix2.hex

clean:
	rm -f PETpix2.hex PETpix2.elf $(OBJECTS)

# file targets:
PETpix2.elf: $(OBJECTS)
	$(COMPILE) -o PETpix2.elf $(OBJECTS)

PETpix2.hex: PETpix2.elf
	rm -f PETpix2.heximage
	avr-objcopy -j .text -j .data -O ihex PETpix2.elf PETpix2.hex
	avr-size --format=avr --mcu=$(DEVICE) PETpix2.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

PETpix2.bin: PETpix2.elf
	rm -f PETpix2.bin
	avr-objcopy -j .text -j .data -O binary PETpix2.elf PETpix2.bin
	avr-size --format=avr --mcu=$(DEVICE) PETpix2.elf

# Targets for code debugging and analysis:
disasm:	PETpix2.elf
	avr-objdump -d PETpix2.elf

cpp:
	$(COMPILE) -E PETpix2.c UART_routines.c

ports: 
	python ../../shared/getports.py

debug:
	minicom -D $(SPAREPORT)
