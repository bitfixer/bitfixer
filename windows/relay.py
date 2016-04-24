import serial
import socket
import sys
import msvcrt

ser = serial.Serial('COM5', 38400, timeout=1)
print(ser.name)
#ser.write(b'ABCD')


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('0.0.0.0', 29999)
print >> sys.stderr, 'starting on %s port %s' % server_address
sock.bind(server_address)

quit = False
while quit == False:
    print >> sys.stderr, '\nwaiting'
    data, address = sock.recvfrom(4096)
    print >> sys.stderr, '\nreceived %s bytes from %s' % (len(data), address)
    ser.write(data)


#ser.close()