REMOTE := 192.168.1.22

all:
	g++ -o pinetrecv -std=c++11 -lcurl pinetrecv.cpp CommPort.cpp timer.cpp NetPort.cpp TCP.cpp URLFetcher.cpp YTVideo.cpp

transfer:
	scp pinetrecv.cpp pi@$(REMOTE):~/pinetrecv/pinetrecv.cpp
	scp Makefile pi@$(REMOTE):~/pinetrecv/Makefile
	scp ../../shared/CommPort.h pi@$(REMOTE):~/pinetrecv/CommPort.h
	scp ../../shared/CommPort.cpp pi@$(REMOTE):~/pinetrecv/CommPort.cpp
	scp ../../shared/timer.hpp pi@$(REMOTE):~/pinetrecv/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/pinetrecv/timer.cpp
	scp ../../shared/NetPort.h pi@$(REMOTE):~/pinetrecv/NetPort.h
	scp ../../shared/NetPort.cpp pi@$(REMOTE):~/pinetrecv/NetPort.cpp
	scp ../../external/Net/Net.h pi@$(REMOTE):~/pinetrecv/Net.h
	scp ../../shared/TCP.* pi@$(REMOTE):~/pinetrecv/
	scp ../../shared/rpi/URLFetcher.* pi@$(REMOTE):~/pinetrecv/
	scp ../../shared/youtube/YTVideo.* pi@$(REMOTE):~/pinetrecv/
