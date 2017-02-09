all: clean prog audio wav

play: all
	afplay out.wav

dgemu: all
	../dgemu/build/Debug/dgemu

wav:
	ffmpeg -f f32le -ar 44100 -ac 1 -i out.raw out.wav

audio:
	../dgtape/build/Debug/dgtape dgprog.bin

prog:
	zasm -uwy --reqcolon dgprog.asm dgprog_1.bin
	python dglength.py dgprog_1.bin dgprog.bin
	rm dgprog_1.bin

clean:
	rm -f *.wav
	rm -f *.bin
	rm -f *.lst
	rm -f *.raw
	rm -f -r s
