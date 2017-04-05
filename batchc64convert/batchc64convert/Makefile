REMOTE := 192.168.1.25

all:
	g++ -o batchc64convert -std=c++11 main.cpp Image.cpp Ditherer.cpp C64Image.cpp

transfer:
	scp main.cpp pi@$(REMOTE):~/batchc64convert/main.cpp
	scp Makefile pi@$(REMOTE):~/batchc64convert/Makefile
	scp ../../shared/Image.hpp pi@$(REMOTE):~/batchc64convert/Image.hpp
	scp ../../shared/Image.cpp pi@$(REMOTE):~/batchc64convert/Image.cpp
	scp ../../shared/Ditherer.hpp pi@$(REMOTE):~/batchc64convert/Ditherer.hpp
	scp ../../shared/Ditherer.cpp pi@$(REMOTE):~/batchc64convert/Ditherer.cpp
	scp ../../shared/C64Image.hpp pi@$(REMOTE):~/batchc64convert/C64Image.hpp
	scp ../../shared/C64Image.cpp pi@$(REMOTE):~/batchc64convert/C64Image.cpp
	scp ../../shared/c64_colors.h pi@$(REMOTE):~/batchc64convert/c64_colors.h