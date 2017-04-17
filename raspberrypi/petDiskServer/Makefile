REMOTE := 192.168.1.7

OBJECTS	= 	petDiskServer.o \
			timer.o \
			rpiSoftSpi.o

BUILDDIR = build

COMPILE = g++ -c -std=c++11

.cpp.o:
	$(COMPILE) $< -o $@

all: $(OBJECTS)
	g++ -o petDiskServer $(OBJECTS) -lwiringPi

transfer:
	scp petDiskServer.cpp pi@$(REMOTE):~/petDiskServer/petDiskServer.cpp
	scp ../../shared/timer.hpp pi@$(REMOTE):~/petDiskServer/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/petDiskServer/timer.cpp
	scp Makefile pi@$(REMOTE):~/petDiskServer/Makefile
	scp ../../shared/rpi/rpiSoftSpi.h pi@$(REMOTE):~/petDiskServer/rpiSoftSpi.h
	scp ../../shared/rpi/rpiSoftSpi.cpp pi@$(REMOTE):~/petDiskServer/rpiSoftSpi.cpp
