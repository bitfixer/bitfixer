REMOTE := 192.168.1.29

SOURCES := imconv_batch.cpp Image.cpp Ditherer.cpp timer.cpp C64Image.cpp

all:
	g++ -o imconv -lpthread -std=c++11 $(SOURCES)

transfer:
	scp imconv/imconv_batch.cpp pi@$(REMOTE):~/imconv/imconv_batch.cpp
	scp Makefile pi@$(REMOTE):~/imconv/Makefile
	scp ../shared/Image.hpp pi@$(REMOTE):~/imconv/Image.hpp
	scp ../shared/Image.cpp pi@$(REMOTE):~/imconv/Image.cpp
	scp ../shared/Ditherer.hpp pi@$(REMOTE):~/imconv/Ditherer.hpp
	scp ../shared/Ditherer.cpp pi@$(REMOTE):~/imconv/Ditherer.cpp
	scp ../shared/timer.hpp pi@$(REMOTE):~/imconv/timer.hpp
	scp ../shared/timer.cpp pi@$(REMOTE):~/imconv/timer.cpp
	scp ../shared/C64Image.hpp pi@$(REMOTE):~/imconv/C64Image.hpp
	scp ../shared/C64Image.cpp pi@$(REMOTE):~/imconv/C64Image.cpp
	scp ../shared/StringTools.h pi@$(REMOTE):~/imconv/StringTools.h
	scp ../shared/c64_colors.h pi@$(REMOTE):~/imconv/c64_colors.h
