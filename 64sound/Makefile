clean:
	rm -f term.prg
	rm -f term.tap
	rm -f term_long.wav
	rm -f term.wav

term.prg:
	~/Documents/VintageComputer/VICE/tools/petcat -w2 -o term.prg -- 64sound.bas

term.prg.2:
	~/Documents/VintageComputer/VICE/tools/c1541 -attach disk.d64 -read term term.prg

term.tap: term.prg
	./prg2wav -s -t term.tap -m c64ntsc term.prg

term.wav: clean term.tap
	./c64tape -f 44100 -r term.tap > term_long.wav
	ffmpeg -ss 7.5 -i term_long.wav term.wav

64load: term.wav
	afplay term.wav