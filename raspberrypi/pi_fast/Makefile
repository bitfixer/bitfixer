REMOTE := 10.0.0.13

all:
	g++ -o pifast -lwiringPi pifast.cpp

transfer:
	scp pifast.cpp pi@$(REMOTE):~/pifast/pifast.cpp
	scp Makefile pi@$(REMOTE):~/pifast/Makefile
