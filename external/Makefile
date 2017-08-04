# makefile for external tools

libaudiotap.so:
	cd libaudiotap; make libaudiotap.so; cp libaudiotap.so /usr/local/lib
	cd libaudiotap; make clean

prg2wav:
	cd wav-prg; make cmdline/prg2wav AUDIOTAP_HDR=../libaudiotap AUDIOTAP_LIB=/usr/local/lib
	cd wav-prg; cp cmdline/prg2wav /usr/local/bin

tap2wav:
	cd c64tapedecode-0.1-src/src; make tap2wav
	cd c64tapedecode-0.1-src/src; cp tap2wav /usr/local/bin

clean:
	cd libaudiotap; make clean
	cd wav-prg; make clean
	cd c64tapedecode-0.1-src/src; make clean
