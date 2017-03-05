clean:
	rm -f *.prg
	rm -f *.o
	rm -f *.tap
	rm -f *.wav

%.prg: %.c
	cl65 -t c64 $< -o $@
	rm $*.o

%.tap: %.prg
	prg2wav -s -t $@ -m c64ntsc $<

%.wav: %.tap
	tap2wav -f 44100 -r $< > $@.long.wav
	ffmpeg -ss 7.5 -i $@.long.wav $@
	rm $@.long.wav

%.load : %.wav
	afplay $<
