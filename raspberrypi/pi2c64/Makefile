REMOTE := 10.135.3.103

all:
	g++ -o pi2c64 -lwiringPi -lavformat -lavcodec -lavutil -lswscale pi2c64.cpp image_utilities.cpp

transfer:
	scp pi2c64.cpp pi@$(REMOTE):~/pi2c64/pi2c64.cpp
	scp commands.h pi@$(REMOTE):~/pi2c64/commands.h
	scp image_utilities.cpp pi@$(REMOTE):~/pi2c64/image_utilities.cpp
	scp image_utilities.h pi@$(REMOTE):~/pi2c64/image_utilities.h
	scp Makefile pi@$(REMOTE):~/pi2c64/Makefile
