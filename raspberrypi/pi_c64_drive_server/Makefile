REMOTE := 192.168.1.19

all:
	g++ -o pi64drive -lwiringPi -std=c++11 pi64drive.cpp timer.cpp rpiSpiData.cpp

transfer:
	scp pi64drive.cpp pi@$(REMOTE):~/pi64drive/pi64drive.cpp
	scp Makefile pi@$(REMOTE):~/pi64drive/Makefile
	scp ../../shared/timer.hpp pi@$(REMOTE):~/pi64drive/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/pi64drive/timer.cpp
	scp ../../shared/c64drive.h pi@$(REMOTE):~/pi64drive/c64drive.h
	scp ../../shared/rpi/rpiSpiData.h pi@$(REMOTE):~/pi64drive/rpiSpiData.h
	scp ../../shared/rpi/rpiSpiData.cpp pi@$(REMOTE):~/pi64drive/rpiSpiData.cpp
