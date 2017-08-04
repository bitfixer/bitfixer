REMOTE := 192.168.1.30

all:
	g++ -o omxplayer_time_reader -std=c++11 main.cpp timer.cpp NetPort.cpp

transfer:
	scp omxplayer_time_reader/main.cpp pi@$(REMOTE):~/omxplayer_time_reader/main.cpp
	scp Makefile pi@$(REMOTE):~/omxplayer_time_reader/Makefile
	scp ../shared/timer.cpp pi@$(REMOTE):~/omxplayer_time_reader/timer.cpp
	scp ../shared/timer.hpp pi@$(REMOTE):~/omxplayer_time_reader/timer.hpp
	scp ../shared/NetPort.h pi@$(REMOTE):~/omxplayer_time_reader/NetPort.h
	scp ../shared/NetPort.cpp pi@$(REMOTE):~/omxplayer_time_reader/NetPort.cpp
	scp ../external/Net/Net.h pi@$(REMOTE):~/omxplayer_time_reader/Net.h
	scp ../shared/CommPort.h pi@$(REMOTE):~/omxplayer_time_reader/CommPort.h