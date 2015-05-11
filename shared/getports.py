#getports.py
# Michael Hill May 2015
# quick script for determining which usb-to-serial adapter is connected to
# an Atmel STK500
# Mac OS has an annoying habit of switching the ids for the serial adapters

import os

def timeout_command(command, timeout):
    """call shell-command and either return its output or kill it
        if it doesn't normally exit within timeout seconds and return None"""
    import subprocess, datetime, os, time, signal
    start = datetime.datetime.now()
    print start
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    while process.poll() is None:
        time.sleep(0.1)
        now = datetime.datetime.now()
        if (now - start).seconds > timeout:
            os.kill(process.pid, signal.SIGKILL)
            os.waitpid(-1, os.WNOHANG)
            return None
    return process.stdout.read()


ports = os.popen("ls /dev/tty.usb*").read().split()
os.popen("rm -r test1")

foundport = 0
for port in ports:
    if foundport == 0:
        output = timeout_command(["avrdude", "-P", port, "-c", "stk500v2", "-p", "atmega168", "-U", "efuse:r:test1:i"], 2)
        if os.path.isfile("test1"):
            print "stk500 on "+port
            os.popen("rm test1")
            os.popen("echo "+port+" > master.port")
            foundport = 1
        else:
            os.popen("echo "+port+" > spare.port")
    else:
        os.popen("echo "+port+" > spare.port")
