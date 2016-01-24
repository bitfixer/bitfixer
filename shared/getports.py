#getports.py
# Michael Hill May 2015
# quick script for determining which usb-to-serial adapter is connected to
# an Atmel STK500
# Mac OS has an annoying habit of switching the ids for the serial adapters

import os
import time

ports = os.popen("ls /dev/tty.usb*").read().split()

foundport = 0
index = 0
for port in ports:
    if foundport == 0:
        cmd = "avrdude -P "+port+" -c stk500v2 -p atmega168 -n > avrtest.txt 2>&1 &"
        print cmd
        os.system(cmd)

        avrdone = 0
       
        #check output file
        while avrdone == 0:
            time.sleep(1)
            f = open('avrtest.txt', 'r')
            result = f.read()
            f.close()

            # look for success condition or timeout condition
            res = result.find('stk500v2_command(): command failed')
            if res > -1:
                # stk500 present, no chip
                foundport = 1
                avrdone = 1
                print "avr found at port "+port

            if avrdone == 0:
                res = result.find('AVR device initialized')
                if res > -1:
                    foundport = 1
                    avrdone = 1
                    print "avr found at port "+port

            if avrdone == 0:
                res = result.find('stk500v2_ReceiveMessage(): timeout')
                if res > -1:
                    print "timeout, need to kill"
                    os.system("ps ax | grep avrdude > proc.txt")
                    f = open('proc.txt', 'r')
                    line = f.readline()
                    f.close()
                    print line
                    parts = line.split(' ')
                    pid = parts[0]
                    cmd = "kill -9 "+str(pid)
                    print cmd
                    os.system(cmd)
                    os.system('rm proc.txt')
                    avrdone = 1

        os.system('rm avrtest.txt')

        if foundport == 1:
            print "found port at "+port
            os.system("echo "+port+" > master.port")
        else:
            os.system("echo "+port+" > spare.port")
    else:
        os.system("echo "+port+" > spare.port")

if foundport == 0:
    print "No AVR Detected."