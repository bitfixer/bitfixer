clean:
	rm -f loader.prg
	rm -f loader.o
	rm -f loader.tap
	rm -f loader_long.wav
	rm -f loader.wav

loader.prg: clean
	cl65 -t c64 loader.c -o loader.prg
	
loader.tap: loader.prg
	./prg2wav -s -t loader.tap -m c64ntsc loader.prg

loader.wav: loader.tap
	./c64tape -f 44100 -r loader.tap > loader_long.wav
	ffmpeg -ss 7.5 -i loader_long.wav loader.wav

load64: clean loader.wav
	afplay loader.wav