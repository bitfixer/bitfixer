#screen -d -m /dev/tty.usbserial1 38400
screen -p0 -X exec \!\! lsz -X petDiskPiInterface.bin
#echo GO
#sleep 5
#screen -p0 -X quit
#echo DONE
