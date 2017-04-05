<?php

    $glyphdir = "glyphs";
    $outfname = "petsciiGlyphs.h";
    
    $fp = fopen($outfname, "w");
    fwrite($fp, "unsigned char glyphs[] = {\n");
    
    for ($i = 0; $i < 128; $i++)
    {
        $fname = "$glyphdir/$i.txt";
        $contents = file_get_contents($fname);
	
        $currchindex = 0;
        fwrite($fp, "//$i\n");
        for ($j = 0; $j < 8; $j++)
        {
            for ($k = 0; $k < 8; $k++)
            {
                $ch = $contents[$currchindex];
                fwrite($fp, "'$ch', ");
                $currchindex++;
            }
            fwrite($fp, "\n");
        }

    }
    
    fwrite($fp, "};\n");
    fclose($fp);               
    
?>
