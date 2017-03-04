all: tapencoder.dll tapdecoder.dll

ifdef WITH_SINE
  libtapdecoder.so tapdecoder.dll:ADD_CFLAGS+=-DHAVE_SINE_WAVE
  libtapdecoder.so:ADD_LDFLAGS=-lm
endif

%.dll:ADD_LDFLAGS+=-Wl,--out-implib=$*.lib

clean:
	rm -f *.dll *.lib *~ *.so

lib%.so: %.c
	$(CC) $(CFLAGS) $(ADD_CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(ADD_LDFLAGS)

ifdef WITH_VERSION
  RESOURCE_OBJECT=%-resource.o
endif

%.dll: %-resource.o

WINDRES=windres

%-resource.o: %-resource.rc
	$(WINDRES) -o $@ $^


%.dll: %.c %.def $(RESOURCE_OBJECT)
	$(CC) $(CFLAGS) $(ADD_CFLAGS) -shared -static-libgcc -o $@ $+ $(LDFLAGS) $(ADD_LDFLAGS)
