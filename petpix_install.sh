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

#install janus
apt-get -y install libmicrohttpd-dev libjansson-dev \
libssl-dev libsrtp-dev libsofia-sip-ua-dev libglib2.0-dev \
libopus-dev libogg-dev libcurl4-openssl-dev liblua5.3-dev \
libconfig-dev pkg-config gengetopt libtool automake libnice-dev
apt-get -y remove libsrtp0-dev
apt-get -y install libsrtp2-dev
git clone https://github.com/meetecho/janus-gateway.git
cd janus-gateway
sh autogen.sh
./configure --prefix=/opt/janus
make
make install
make configs
cd ..

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
cp /boot/cmdline.txt cmdline.txt; echo "spidev.bufsiz=65535 " >> cmdline.txt; tr -d '\n' < cmdline.txt > cmdline_new.txt
cp cmdline_new.txt /boot/cmdline.txt
rm cmdline.txt; rm cmdline_new.txt

git clone https://github.com/bitfixer/petpix_v2.git
cd petpix_v2
make all
