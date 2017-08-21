REMOTE := 192.168.1.19

all:
	g++ -o pitinyspitest -lwiringPi -std=c++11 pitinyspitest.cpp rpiSpiData.cpp

transfer:
	scp pitinyspitest.cpp pi@$(REMOTE):~/pitinyspitest/pitinyspitest.cpp
	scp Makefile pi@$(REMOTE):~/pitinyspitest/Makefile
	scp test.txt pi@$(REMOTE):~/pitinyspitest/test.txt
	scp ../../shared/rpi/rpiSpiData.h pi@$(REMOTE):~/pitinyspitest/rpiSpiData.h
	scp ../../shared/rpi/rpiSpiData.cpp pi@$(REMOTE):~/pitinyspitest/rpiSpiData.cpp
