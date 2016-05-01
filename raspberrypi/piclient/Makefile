REMOTE := 10.0.0.5

all:
	g++ -o main -lwiringPi main.cpp

transfer:
	scp main.c pi@$(REMOTE):~/piclient/main.c
	scp main.cpp pi@$(REMOTE):~/piclient/main.cpp
	scp Makefile pi@$(REMOTE):~/piclient/Makefile

image:
	mkfs.fat -F 32 -C disk.img 4096