REMOTE := 10.0.0.13

all:
	g++ -o pispi -lwiringPi pispi.cpp

transfer:
	scp pispi.cpp pi@$(REMOTE):~/pispi/pispi.cpp
	scp Makefile pi@$(REMOTE):~/pispi/Makefile
