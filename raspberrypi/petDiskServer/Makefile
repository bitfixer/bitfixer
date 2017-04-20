REMOTE := 192.168.1.7

OBJECTS	= 	petDiskServer.o \
			timer.o \
			rpiThreeWireSPI.o

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
	scp ../../shared/rpi/rpiThreeWireSPI.h pi@$(REMOTE):~/petDiskServer/rpiThreeWireSPI.h
	scp ../../shared/rpi/rpiThreeWireSPI.cpp pi@$(REMOTE):~/petDiskServer/rpiThreeWireSPI.cpp
