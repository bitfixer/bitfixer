REMOTE := 192.168.1.19

all:
	g++ -o spi_multi -lwiringPi -std=c++11 spi_multi.cpp timer.cpp rpiSpiData.cpp

transfer:
	scp spi_multi.cpp pi@$(REMOTE):~/spi_multi/spi_multi.cpp
	scp Makefile pi@$(REMOTE):~/spi_multi/Makefile
	scp ../../shared/timer.hpp pi@$(REMOTE):~/spi_multi/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/spi_multi/timer.cpp
	scp ../../shared/c64drive.h pi@$(REMOTE):~/spi_multi/c64drive.h
	scp ../../shared/rpi/rpiSpiData.h pi@$(REMOTE):~/spi_multi/rpiSpiData.h
	scp ../../shared/rpi/rpiSpiData.cpp pi@$(REMOTE):~/spi_multi/rpiSpiData.cpp
