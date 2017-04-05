echo $1
#echo $1.c64
rm -r ppm
rm -r out
mkdir ppm
mkdir out
ffmpeg -i $1 -vf scale=320:200 -an -r 4 ppm/out%04d.ppm
find ppm/* | xargs -n 1 ../imconv/build/Debug/imconv out c64
cat out/* > $1.c64
#echo scp $1.c64 pi@192.168.1.9:~/pinetrecv/$1.c64
