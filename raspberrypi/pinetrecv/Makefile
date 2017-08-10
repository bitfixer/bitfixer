REMOTE := 192.168.1.22

all:
	g++ -o pinetrecv -lwiringPi -std=c++11 pinetrecv.cpp CommPort.cpp timer.cpp NetPort.cpp rpiSpiData.cpp

transfer:
	scp pinetrecv.cpp pi@$(REMOTE):~/pinetrecv/pinetrecv.cpp
	scp Makefile pi@$(REMOTE):~/pinetrecv/Makefile
	scp spock.txt pi@$(REMOTE):~/pinetrecv/
	scp ../../shared/CommPort.h pi@$(REMOTE):~/pinetrecv/CommPort.h
	scp ../../shared/CommPort.cpp pi@$(REMOTE):~/pinetrecv/CommPort.cpp
	scp ../../shared/timer.hpp pi@$(REMOTE):~/pinetrecv/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/pinetrecv/timer.cpp
	scp ../../shared/NetPort.h pi@$(REMOTE):~/pinetrecv/NetPort.h
	scp ../../shared/NetPort.cpp pi@$(REMOTE):~/pinetrecv/NetPort.cpp
	scp ../../external/Net/Net.h pi@$(REMOTE):~/pinetrecv/Net.h
	scp ../../shared/rpi/rpiSpiData.h pi@$(REMOTE):~/pinetrecv/rpiSpiData.h
	scp ../../shared/rpi/rpiSpiData.cpp pi@$(REMOTE):~/pinetrecv/rpiSpiData.cpp
