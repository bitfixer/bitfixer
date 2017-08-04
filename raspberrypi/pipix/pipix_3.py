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
buf = 2048

ser = serial.Serial('/dev/ttyAMA0', 460800, timeout=10)

starttime = time.time()
t = 0
while 1:
    str, addr = s.recvfrom(buf)
    print t, len(str)
    t = t+1

#    for i in range(0,10):
#        ser.write(chr(0xff));

#    ser.write(chr(0x00))
    ser.write(str)
    #time.sleep(0.1)

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
