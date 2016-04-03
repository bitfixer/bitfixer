clean:
	rm -f client.o
	rm -f client.prg
	rm -f client.tap
	rm -f client_long.wav
	rm -f client.wav

client.prg:
	cl65 -t c64 client.c -o client.prg

client.tap: client.prg
	./prg2wav -s -t client.tap -m c64ntsc client.prg

client.wav: clean client.tap
	./c64tape -f 44100 -r client.tap > client_long.wav
	ffmpeg -ss 7.5 -i client_long.wav client.wav

install: client.wav
	afplay client.wav