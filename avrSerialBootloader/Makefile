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

DEVICE     = atmega168
CLOCK      = 8000000
PROGRAMMER = -c stk500v2 -P $(MASTERPORT)
OBJECTS    = avrSerialBootloader.o calcrc.o ../shared/UART_routines.o
FUSES      = -U efuse:w:0x00:m hfuse:w:0xdf:m -U lfuse:w:0xe2:m

INCLUDEDIR = -I../shared
BUILDDIR = build

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
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) $(INCLUDEDIR)

# symbolic targets:
all:	avrSerialBootloader.bin

.c.o:
	$(COMPILE) -c $< -o $@
	echo $@
	mv $@ $(BUILDDIR)/$(notdir $@)

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -D -U flash:w:avrSerialBootloader.hex

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: erase flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID avrSerialBootloader.hex

clean:
	rm -f avrSerialBootloader.hex avrSerialBootloader.elf avrSerialBootloader.bin $(BUILDDIR)/*.o

erase:
	$(AVRDUDE) -e

# file targets:
avrSerialBootloader.elf: $(OBJECTS)
	mkdir -p $(BUILDDIR)
	cd $(BUILDDIR); $(COMPILE) -o avrSerialBootloader.elf $(notdir $(OBJECTS)) -Wl,-Ttext=0x3800
	mv $(BUILDDIR)/avrSerialBootloader.elf avrSerialBootloader.elf

avrSerialBootloader.hex: avrSerialBootloader.elf
	rm -f avrSerialBootloader.hex
	avr-objcopy -j .text -j .data -O ihex avrSerialBootloader.elf avrSerialBootloader.hex
	avr-size --format=avr --mcu=$(DEVICE) avrSerialBootloader.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

avrSerialBootloader.bin: avrSerialBootloader.hex
	rm -f avrSerialBootloader.bin
	avr-objcopy -I ihex -O binary avrSerialBootloader.hex avrSerialBootloader.bin

# Targets for code debugging and analysis:
disasm:	avrSerialBootloader.elf
	avr-objdump -d avrSerialBootloader.elf

cpp:
	$(COMPILE) -E avrSerialBootloader.c calcrc.c ../shared/UART_routines.c

ports:
	python ../shared/getports.py

debug:
	minicom -D $(SPAREPORT)
