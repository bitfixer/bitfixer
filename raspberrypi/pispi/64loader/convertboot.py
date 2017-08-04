import sys
import os

fname = sys.argv[1]

# get size of file
size = os.path.getsize(fname)

with open(fname, "rb") as f:
    print "\t.byte\t$%02x\t; size is %d" % (size, size)
    byte = f.read(1)
    while byte:
        print "\t.byte\t$%02x" % ord(byte)
        byte = f.read(1)
