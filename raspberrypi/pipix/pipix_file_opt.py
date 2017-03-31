#!/usr/bin/python

from socket import *
import sys
import select
import serial
import time

#setup the UDP input
host = "0.0.0.0"
port = 9999
s = socket(AF_INET, SOCK_DGRAM)
s.bind((host, port))
addr = (host, port)
buf = 1024

ser = serial.Serial('/dev/ttyAMA0', 460800, timeout=10)

# check for file input
print "len " + str(len(sys.argv))
fname = ""
if len(sys.argv) >= 2:
    print sys.argv[1]
    fname = sys.argv[1]

numframes = 1000
file = open(fname, 'rb')

starttime = time.time()

#for t in range(0,numframes):
#t = 0
done = False
while done == False:
    #str, addr = s.recvfrom(buf)
    #print t, len(str)
    #t = t+1

    str = file.read(1000)
    if (len(str) == 1000):
        for i in range(0,10):
            ser.write(chr(0xff));

        ser.write(chr(0x00))
        ser.write(str)

        file.seek(24,1)

        #x = ser.read()
        #while x != ':':
        #    print x
        #    x = ser.read()

        time.sleep(0.1)
    else:
        done = True

endtime = time.time()
fps = numframes/(endtime-starttime)

print endtime-starttime
print fps


ser.close()








#for r in range(0,25):
#        str = file.read(40)
#        print str

#    file.seek(24, 1)
#    print

#    time.sleep(0.1)
