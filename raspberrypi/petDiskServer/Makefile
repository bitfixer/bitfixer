REMOTE := 192.168.1.7

all:
	g++ -o pispi -lwiringPi -std=c++11 petDiskServer.cpp

transfer:
	scp petDiskServer.cpp pi@$(REMOTE):~/petDiskServer/petDiskServer.cpp
	scp Makefile pi@$(REMOTE):~/petDiskServer/Makefile
