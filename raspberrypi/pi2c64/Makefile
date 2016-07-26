REMOTE := 10.0.0.11

all:
	g++ -o pi2c64 -lwiringPi pi2c64.cpp

transfer:
	scp pi2c64.cpp pi@$(REMOTE):~/pi2c64/pi2c64.cpp
	scp Makefile pi@$(REMOTE):~/pi2c64/Makefile
