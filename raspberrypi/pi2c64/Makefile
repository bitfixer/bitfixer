REMOTE := 10.135.3.103

all:
	g++ -o pi2c64 -lwiringPi pi2c64.cpp

transfer:
	scp pi2c64.cpp pi@$(REMOTE):~/pi2c64/pi2c64.cpp
	scp commands.h pi@$(REMOTE):~/pi2c64/commands.h
	scp Makefile pi@$(REMOTE):~/pi2c64/Makefile
