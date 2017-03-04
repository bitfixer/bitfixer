all:audiotap.dll

ifdef WITH_VERSION
  RESOURCE_OBJECT=lib%-resource.o
endif

%.dll: lib%.o lib%_external_symbols.o windows_wait_event.o %.def $(RESOURCE_OBJECT)
	$(CC) -shared -static-libgcc -Wl,--out-implib=libaudiotap.a -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.dll *.lib *~ *.so

libaudiotap.so: libaudiotap.o libaudiotap_external_symbols.o pthread_wait_event.o
	$(CC) -shared -o $@ $^ -ldl $(LDFLAGS)

ifdef DEBUG
 CFLAGS+=-g
endif

ifdef OPTIMISE
 CFLAGS+=-O3
endif

ifdef LINUX64BIT
 CFLAGS+=-fPIC
endif

ifdef USE_RPATH
 LDFLAGS=-Wl,-rpath=$(USE_RPATH)
endif

WINDRES=windres

%-resource.o: %-resource.rc
	$(WINDRES) -o $@ $^

