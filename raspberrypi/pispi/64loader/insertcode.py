import sys

infname = sys.argv[1]
templatefname = sys.argv[2]
outfname = sys.argv[3]
key = sys.argv[4]

fp = open(templatefname, 'r')
outfp = open(outfname, 'w')
for line in fp:
	outfp.write(line)
	#outfp.write("*\n")
	if key in line:
		#outfp.write("***")
		infp = open(infname, 'r')
		for inline in infp:
			outfp.write(inline)
		infp.close()

outfp.close()
fp.close()
