REMOTE := 192.168.1.22
NAME := petsciiConvert

SOURCES := main.cpp Image.cpp timer.cpp dct.cpp

all:
	g++ -o $(NAME) -std=c++11 $(SOURCES)

transfer:
	scp main.cpp pi@$(REMOTE):~/$(NAME)/main.cpp
	scp Makefile pi@$(REMOTE):~/$(NAME)/Makefile
	scp ../shared/Image.hpp pi@$(REMOTE):~/$(NAME)/Image.hpp
	scp ../shared/Image.cpp pi@$(REMOTE):~/$(NAME)/Image.cpp
	scp ../shared/timer.hpp pi@$(REMOTE):~/$(NAME)/timer.hpp
	scp ../shared/timer.cpp pi@$(REMOTE):~/$(NAME)/timer.cpp
	scp petsciiGlyphs.h pi@$(REMOTE):~/$(NAME)/petsciiGlyphs.h
	scp dct.cpp pi@$(REMOTE):~/$(NAME)/dct.cpp
	scp dct.h pi@$(REMOTE):~/$(NAME)/dct.h

