apt-get update
apt-get -y install git
apt-get -y install wiringpi
apt-get -y install ffmpeg
apt-get -y install gcc-avr
apt-get -y install avr-libc
apt-get -y install avrdude
apt-get -y install ruby
apt-get -y install php
gem install rmate

#install http-server
apt-get -y install npm
npm install http-server -g
wget https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-stable-linux-arm.zip
unzip ngrok-stable-linux-arm.zip
mv ngrok /usr/local/bin
rm ngrok-stable-linux-arm.zip

cp /boot/config.txt config.txt; echo "core_freq=250" >> config.txt
cp config.txt /boot/config.txt
rm config.txt
#cp /boot/cmdline.txt cmdline.txt; echo "spidev.bufsiz=65535 " >> cmdline.txt; tr -d '\n' < cmdline.txt > cmdline_new.txt
#cp cmdline_new.txt /boot/cmdline.txt
#rm cmdline.txt; rm cmdline_new.txt

#git clone https://github.com/bitfixer/petpix_v2.git
#cd petpix_v2
#make all
