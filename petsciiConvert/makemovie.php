<?php

    $fname = $argv[1];
    $fmt = $argv[2];
    print $fname;

    system("mkdir $fname");
    $framerate = 15;

    // convert to images
    $cmd = "ffmpeg -i $fname.$fmt -r $framerate -f image2 -s 320x200 $fname/%d.png";
    print $cmd."\n";
    system($cmd);

    $num_images = iterator_count(new DirectoryIterator("$fname/")) - 2;
    print "$num_images total images.";

    $cmd = "build/Debug/petsciiConvert2 $fname $num_images $framerate";
    print "%cmd\n";
    system($cmd);

    $fp = fopen("$fname.txt","wb");
    for ($i = 1; $i <= $num_images; $i++)
    {
        print "writing image $i\n";
        $frame = file_get_contents("$fname/$i.txt");
        fwrite($fp, $frame);
    }
    fclose($fp);

    $cmd = "rm -r $fname";
    system($cmd);

    # transfer to rpi
    #$cmd = "scp $fname.txt pi@192.168.1.27:$fname.txt";
    #system($cmd);
?>
