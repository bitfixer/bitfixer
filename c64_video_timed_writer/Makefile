REMOTE := 192.168.1.29

all:
	g++ -o c64_video_timed_writer -std=c++11 main.cpp timer.cpp NetPort.cpp

transfer:
	scp c64_video_timed_writer/main.cpp pi@$(REMOTE):~/c64_video_timed_writer/main.cpp
	scp Makefile pi@$(REMOTE):~/c64_video_timed_writer/Makefile
	scp ../shared/timer.cpp pi@$(REMOTE):~/c64_video_timed_writer/timer.cpp
	scp ../shared/timer.hpp pi@$(REMOTE):~/c64_video_timed_writer/timer.hpp
	scp ../shared/NetPort.h pi@$(REMOTE):~/c64_video_timed_writer/NetPort.h
	scp ../shared/NetPort.cpp pi@$(REMOTE):~/c64_video_timed_writer/NetPort.cpp
	scp ../shared/CommPort.h pi@$(REMOTE):~/c64_video_timed_writer/CommPort.h
	scp ../external/Net/Net.h pi@$(REMOTE):~/c64_video_timed_writer/Net.h