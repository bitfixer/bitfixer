clean:
	rm -f client_sd.o
	rm -f petpix.prg

petpix: clean
	cl65 -t pet client_sd.c -o petpix.prg

petpix_fast:
	cl65 -t pet client_sd_fast.c -o petpixf.prg

testpeek:
	cl65 -t pet testpeek.c -o testpeek.prg