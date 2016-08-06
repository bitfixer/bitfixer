clean:
	rm -f client_sd.o
	rm -f petpix.prg
	rm -f petpix8032.*
	rm -f petpixfast.*

petpix: clean
	cl65 -t pet client_sd.c -o petpix.prg

petpix_fast:
	cl65 -t pet client_sd_fast.c -o petpixf.prg

petpix_8032:
	cl65 -t pet client_8032.c -o petpix8032.prg

testpeek:
	cl65 -t pet testpeek.c -o testpeek.prg

petpix_8032.tap: petpix_8032
	./prg2wav -s -t petpix8032.tap -m c64ntsc petpix8032.prg

petpix_8032.wav: petpix_8032.tap
	./c64tape -f 44100 -r petpix8032.tap > petpix8032_long.wav
	ffmpeg -ss 7.5 -i petpix8032_long.wav petpix8032.wav

loadpet: clean petpix_8032.wav
	afplay petpix8032.wav



petpix_fast.tap: petpix_fast
	./prg2wav -s -t petpixfast.tap -m c64ntsc petpixf.prg

petpix_fast.wav: petpix_fast.tap
	./c64tape -f 44100 -r petpixfast.tap > petpixfast_long.wav
	ffmpeg -ss 7.5 -i petpixfast_long.wav petpixfast.wav

petpix_fast.load: clean petpix_fast.wav
	afplay petpixfast.wav