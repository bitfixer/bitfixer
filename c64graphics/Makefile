graph:
	cl65 -t c64 graph.c -o graph.prg
	#cl65  -o graph.prg --start-addr $4000 -t c64 -C c64-asm.cfg graph.c
	~/Documents/VintageComputer/VICE/tools/c1541 -format disk,1 d64 diskimage.d64 -attach diskimage.d64 -write graph.prg graph

