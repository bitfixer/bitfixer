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
OBJECTS    = PETdisk.o IEEE488.o ../bf-avr-sdlib/SD_routines.o \
				../bf-avr-sdlib/SPI_routines.o ../shared/UART_routines.o \
				SPIDataSource.o ../shared/avr/3WireSPI.o ../shared/avr/Utils.o
#FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x24:m
FUSES      = -U efuse:w:0x00:m -U hfuse:w:0xdf:m -U lfuse:w:0xe2:m

INCLUDEDIR = -I../bf-avr-sdlib -I../shared -I../shared/avr
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
all:	PETdisk.hex

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
	$(AVRDUDE) -U flash:w:PETdisk.hex

flashnoverify:	all
	$(AVRDUDE) -V -U flash:w:PETdisk.hex

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID PETdisk.hex

clean:
	rm -f PETdisk.hex PETdisk.bin PETdisk.elf $(BUILDDIR)/*.o

# file targets:
PETdisk.elf: $(OBJECTS)
	cd $(BUILDDIR); $(COMPILE) -o PETdisk.elf $(notdir $(OBJECTS))
	mv $(BUILDDIR)/PETdisk.elf PETdisk.elf

PETdisk.hex: PETdisk.elf
	rm -f PETdisk.hex
	avr-objcopy -j .text -j .data -O ihex PETdisk.elf PETdisk.hex
	avr-size --format=avr --mcu=$(DEVICE) PETdisk.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

PETdisk.bin: PETdisk.elf
	rm -f PETdisk.bin
	avr-objcopy -j .text -j .data -O binary PETdisk.elf PETdisk.bin
	avr-size --format=avr --mcu=$(DEVICE) PETdisk.elf

# Targets for code debugging and analysis:
disasm:	PETdisk.elf
	avr-objdump -d PETdisk.elf

#cpp:
#	$(COMPILE) -E PETdisk.c ../bf-avr-sdlib/FAT32.c IEEE488.c \
#	../bf-avr-sdlib/SD_routines.c ../bf-avr-sdlib/SPI_routines.c ../shared/UART_routines.c

ports:
	python ../shared/getports.py

debug:
	minicom -D $(SPAREPORT)

firmware: PETdisk.bin
	dd if=PETdisk.bin ibs=14336 count=1 of=FIRM30B.BIN conv=sync

screen:
	screen $(SPAREPORT) 38400

xmodem: firmware
	screen -p0 -X exec \!\! lsz -X PETdisk.bin
