REMOTE := 192.168.1.19

all:
	g++ -o pi64drive -std=c++11 -lwiringPi -lpthread pi64drive.cpp timer.cpp cbmdos.cpp d64fs.cpp localfs.cpp C64Dos.cpp

transfer:
	scp *.cpp pi@$(REMOTE):~/pi64drive/
	scp *.h pi@$(REMOTE):~/pi64drive/
	scp Makefile pi@$(REMOTE):~/pi64drive/Makefile
	scp ../../shared/timer.hpp pi@$(REMOTE):~/pi64drive/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/pi64drive/timer.cpp
	scp ../../shared/c64drive.h pi@$(REMOTE):~/pi64drive/c64drive.h
	scp ../../shared/rpi/rpiSpiData.h pi@$(REMOTE):~/pi64drive/rpiSpiData.h
	scp ../../shared/rpi/rpiSpiData.cpp pi@$(REMOTE):~/pi64drive/rpiSpiData.cpp
	scp ../../shared/rpi/Fifo.h pi@$(REMOTE):~/pi64drive/Fifo.h
