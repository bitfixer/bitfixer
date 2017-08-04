clean:
	rm -f *.o
	rm -f *.ml
	rm -f *.bytes
	rm -f *.boot.s
	rm -f *.prg
	rm -f *.tap
	rm -f *.wav

test:
	echo "test"

%.boot.o: %.boot.s
	ca65 $<

%.o: %.s
	echo $< $@
	ca65 $<

%.ml: %.o
	echo $< $@
	ld65 -o $@ -C linker2.cfg $<

%.bytes: %.ml
	echo $< $@
	python convertboot.py $< > $@

%.boot.s: %.bytes
	echo $< $@
	python insertcode.py $< bootstrap.s $@ __program

%.boot.prg: %.boot.o
	ld65 -o $@ -C linker.cfg $<

%.tap: %.prg
	./prg2wav -s -t $@ -m c64ntsc $<

%.long.wav: %.tap
	./c64tape -f 44100 -r $< > $@

%.wav: %.long.wav
	ffmpeg -ss 7.5 -i $< $@

%.load: %.wav
	afplay $<

bootstrap:
	ca65 test.asm
	ld65 -C linker.cfg test.o

ldtest:
	cc65 ldtest.c
	ca65 ldtest.s
	ld65 -C linker.cfg ldtest.o

loader.prg: clean
	cl65 -t c64 loader.c asmloaders.c -o loader.prg

loader.tap: loader.prg
	prg2wav -s -t loader.tap -m c64ntsc loader.prg

loader.wav: loader.tap
	tap2wav -f 44100 -r loader.tap > loader_long.wav
	ffmpeg -ss 7.5 -i loader_long.wav loader.wav

load64: clean loader.wav
	afplay loader.wav

booter.prg: booter.o
	ld65 -o booter.prg -C linker.cfg booter.o

booter.tap: booter.prg
	./prg2wav -s -t booter.tap -m c64ntsc booter.prg

booter.wav: clean booter.tap
	./c64tape -f 44100 -r booter.tap > booter_long.wav
	ffmpeg -ss 7.5 -i booter_long.wav booter.wav
