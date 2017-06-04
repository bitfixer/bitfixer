REMOTE := 192.168.1.7

OBJECTS	= 	petDiskServer.o \
			rpiThreeWireSPI.o \
			URLFetcher.o \
			Directory.o \
			URLUploader.o

BUILDDIR = build

COMPILE = g++ -c -std=c++11

.cpp.o:
	$(COMPILE) $< -o $@

all: $(OBJECTS)
	g++ -o petDiskServer $(OBJECTS) -lwiringPi -lcurl

transfer:
	scp petDiskServer.cpp pi@$(REMOTE):~/petDiskServer/petDiskServer.cpp
	scp ../../shared/timer.hpp pi@$(REMOTE):~/petDiskServer/timer.hpp
	scp ../../shared/timer.cpp pi@$(REMOTE):~/petDiskServer/timer.cpp
	scp Makefile pi@$(REMOTE):~/petDiskServer/Makefile
	scp ../../shared/rpi/rpiThreeWireSPI.h pi@$(REMOTE):~/petDiskServer/rpiThreeWireSPI.h
	scp ../../shared/rpi/rpiThreeWireSPI.cpp pi@$(REMOTE):~/petDiskServer/rpiThreeWireSPI.cpp
	scp ../../shared/petDiskCommand.h pi@$(REMOTE):~/petDiskServer/petDiskCommand.h
	scp ../../shared/rpi/URLFetcher.h pi@$(REMOTE):~/petDiskServer/URLFetcher.h
	scp ../../shared/rpi/URLFetcher.cpp pi@$(REMOTE):~/petDiskServer/URLFetcher.cpp
	scp ../../shared/rpi/URLUploader.h pi@$(REMOTE):~/petDiskServer/URLUploader.h
	scp ../../shared/rpi/URLUploader.cpp pi@$(REMOTE):~/petDiskServer/URLUploader.cpp
	scp ../../shared/Directory.h pi@$(REMOTE):~/petDiskServer/Directory.h
	scp ../../shared/Directory.cpp pi@$(REMOTE):~/petDiskServer/Directory.cpp
	scp ../../shared/StringSplitter.h pi@$(REMOTE):~/petDiskServer/StringSplitter.h
	scp ../../shared/URL.h pi@$(REMOTE):~/petDiskServer/URL.h
	scp ../../external/picojson/picojson.h pi@$(REMOTE):~/petDiskServer/picojson.h
