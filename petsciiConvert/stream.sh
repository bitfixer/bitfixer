#youtube-dl --id -f 36 $1
ffmpeg -i $1.3gp -y -vn $1.wav
#afplay $1.wav &
ffmpeg -i $1.3gp -vf scale=$3:$4 -y -r $2 -vcodec rawvideo -pix_fmt gray -f rawvideo pipe:1 2>/dev/null | ./build/Debug/petsciiConvert2 -f $2 -w $3 -h $4 -p gray -t -s 80 2>/dev/null | ../c64_video_timed_writer/build/Debug/c64_video_timed_writer -t $5 -p 88888 -a $6
#sleep 0.4; afplay $1.wav
