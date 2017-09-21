REMOTE := 192.168.1.22

all:
	g++ -o yuv_net_recv -std=c++11 main.cpp CommPort.cpp timer.cpp NetPort.cpp

transfer:
	scp yuv_net_recv/main.cpp pi@$(REMOTE):~/yuv_net_recv/main.cpp
	scp Makefile pi@$(REMOTE):~/yuv_net_recv/Makefile
	scp ../shared/CommPort.h pi@$(REMOTE):~/yuv_net_recv/CommPort.h
	scp ../shared/CommPort.cpp pi@$(REMOTE):~/yuv_net_recv/CommPort.cpp
	scp ../shared/timer.hpp pi@$(REMOTE):~/yuv_net_recv/timer.hpp
	scp ../shared/timer.cpp pi@$(REMOTE):~/yuv_net_recv/timer.cpp
	scp ../shared/NetPort.h pi@$(REMOTE):~/yuv_net_recv/NetPort.h
	scp ../shared/NetPort.cpp pi@$(REMOTE):~/yuv_net_recv/NetPort.cpp
	scp ../external/Net/Net.h pi@$(REMOTE):~/yuv_net_recv/Net.h