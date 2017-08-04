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
OBJECTS    = petDiskPiInterface.o ../shared/UART_routines.o ../shared/avr/softSpi.o
FUSES      = -U efuse:w:0x00:m -U hfuse:w:0xdf:m -U lfuse:w:0xe2:m

INCLUDEDIR = -I../shared -I../shared/avr
BUILDDIR = build

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
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) $(INCLUDEDIR)

# symbolic targets:
all:	petDiskPiInterface.hex

.c.o:
	$(COMPILE) -c $< -o $@
	mv $@ $(BUILDDIR)/$(notdir $@)

.cpp.o:
	$(COMPILE) -c $< -o $@
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
	$(AVRDUDE) -U flash:w:petDiskPiInterface.hex

flashnoverify:	all
	$(AVRDUDE) -V -U flash:w:petDiskPiInterface.hex

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID petDiskPiInterface.hex

clean:
	rm -f petDiskPiInterface.hex petDiskPiInterface.bin petDiskPiInterface.elf $(BUILDDIR)/*.o

# file targets:
petDiskPiInterface.elf: $(OBJECTS)
	cd $(BUILDDIR); $(COMPILE) -o petDiskPiInterface.elf $(notdir $(OBJECTS))
	mv $(BUILDDIR)/petDiskPiInterface.elf petDiskPiInterface.elf

petDiskPiInterface.hex: petDiskPiInterface.elf
	rm -f petDiskPiInterface.hex
	avr-objcopy -j .text -j .data -O ihex petDiskPiInterface.elf petDiskPiInterface.hex
	avr-size --format=avr --mcu=$(DEVICE) petDiskPiInterface.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

petDiskPiInterface.bin: petDiskPiInterface.elf
	rm -f petDiskPiInterface.bin
	avr-objcopy -j .text -j .data -O binary petDiskPiInterface.elf petDiskPiInterface.bin
	avr-size --format=avr --mcu=$(DEVICE) petDiskPiInterface.elf

# Targets for code debugging and analysis:
disasm:	petDiskPiInterface.elf
	avr-objdump -d petDiskPiInterface.elf

cpp:
	$(COMPILE) -E petDiskPiInterface.cpp ../shared/UART_routines.c

ports:
	python ../shared/getports.py

debug:
	minicom -D $(SPAREPORT)

firmware: petDiskPiInterface.bin
	dd if=petDiskPiInterface.bin ibs=14336 count=1 of=FIRM30B.BIN conv=sync
