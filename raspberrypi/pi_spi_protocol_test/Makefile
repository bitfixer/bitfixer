REMOTE := 192.168.1.19

all:
	g++ -o pispiprotocoltest -lwiringPi -std=c++11 pispiprotocoltest.cpp timer.cpp

transfer:
	scp pispiprotocoltest.cpp pi@$(REMOTE):~/pispiprotocoltest/pispiprotocoltest.cpp
	scp Makefile pi@$(REMOTE):~/pispiprotocoltest/Makefile
	scp test.txt pi@$(REMOTE):~/pispiprotocoltest/test.txt
	scp ../../shared/timer.hpp pi@$(REMOTE):~/pispiprotocoltest/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/pispiprotocoltest/timer.cpp
