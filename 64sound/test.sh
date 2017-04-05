stty -f /dev/tty.usbserial cs8 1200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
echo "HELLO" > /dev/tty.usbserial