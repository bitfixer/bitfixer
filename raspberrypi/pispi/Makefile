REMOTE := 10.0.0.13

all:
	g++ -o pispi -lwiringPi -lavformat -lavcodec -lavutil -lswscale -std=c++11 pispi.cpp image_utilities.cpp

transfer:
	scp pispi.cpp pi@$(REMOTE):~/pispi/pispi.cpp
	scp Makefile pi@$(REMOTE):~/pispi/Makefile
	scp image_utilities.cpp pi@$(REMOTE):~/pispi/image_utilities.cpp
	scp image_utilities.h pi@$(REMOTE):~/pispi/image_utilities.h
