#!/usr/bin/python

from socket import *
import sys
import select
import serial
import time
import binascii

class FrameDataSource:
    def read(self, size):
        print "FrameDataSource recv " + str(size)
        return 0

class NetworkFrameDataSource(FrameDataSource):
    def __init__(self):
        host = "0.0.0.0"
        port = 9999
        self.s = socket(AF_INET, SOCK_DGRAM)
        self.s.bind((host, port))
        addr = (host, port)

    def read(self):
        size = 1000
        print "NetworkFrameDataSource recv " + str(size)
        res, addr = self.s.recvfrom(size)
        print "got " + str(len(res)) + " bytes from " + str(addr)
        # network frames should always play in realtime
        return res, -1.0

class FileFrameDataSource(FrameDataSource):
    def __init__(self, fname):
        self.file = open(fname, 'rb')

    def read(self):
        size = 1024
        print "FileFrameDataSource recv " + str(size)
        framedata = self.file.read(size)
        if len(framedata) != size:
            return "", -1.0

        framestr = framedata[:1000]
        #timestr = framedata[1000:1024].rstrip()
        timedata = framedata[1000:1024]
        #print "timedata: " + timedata + "*"
        #timestr = binascii.b2a_uu(timedata).rstrip()
        timestr = "%s" % timedata
        #print "timestr: " + timestr + "*"
        frametime = float(timestr)
        return framestr, frametime

ser = serial.Serial('/dev/ttyAMA0', 460800, timeout=10)
datasource = FrameDataSource()

# check for file input
if len(sys.argv) >= 2:
    print sys.argv[1]
    fname = sys.argv[1]
    datasource = FileFrameDataSource(fname)
else:
    datasource = NetworkFrameDataSource()

starttime = time.time()

numframes = 0
done = False
while done == False:
    framestr, frametime = datasource.read()
    if (len(framestr) == 1000):
        print "frametime " + str(frametime)
        currPlaybackTime = time.time() - starttime
        playFrame = True
        if (currPlaybackTime < frametime):
            # wait until correct time to play frame
            waitTime = frametime - currPlaybackTime
            time.sleep(waitTime)
        elif (currPlaybackTime > frametime and frametime >= 0.0):
            playFrame = False
            print "skip frame at " + str(frametime)

        if playFrame == True:
            currPlaybackTime = time.time() - starttime
            print "frame " + str(frametime) + " real " + str(currPlaybackTime)

            for i in range(0,10):
                ser.write(chr(0xff));

            ser.write(chr(0x00))
            ser.write(framestr[:1000])

            numframes += 1
    else:
        done = True

endtime = time.time()
fps = numframes/(endtime-starttime)

print endtime-starttime
print fps

ser.close()
