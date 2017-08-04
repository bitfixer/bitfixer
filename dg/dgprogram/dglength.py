import os
import sys

#print sys.argv[1]
size = os.path.getsize(sys.argv[1])
print "size of " + sys.argv[1] + " is " + str(size)

size = (size + 0x0100) - 1;
size_lo = size & 0x00FF
size_hi = (size & 0xFF00) >> 8

print "size_lo " + str(size_lo) + " " + format(size_lo, '#04X')
print "size_hi " + str(size_hi) + " " + format(size_hi, '#04X')

fh = open(sys.argv[1], "rb")
ba = bytearray(fh.read())
fh.close()

#replace address bytes
ba[26] = size_lo
ba[27] = size_hi

of = open(sys.argv[2], "wb")
of.write(ba)
of.close()
