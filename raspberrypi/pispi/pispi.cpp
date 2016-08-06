#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "image_utilities.h"

class Timer
{
public:
    void start()
    {
        gettimeofday(&startTime, NULL);
    }
    void end()
    {
        gettimeofday(&endTime, NULL);
    }
    
    void report(const char *infostr)
    {
        double start = (double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0);
        double end = (double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0);
        double elapsed = end-start;
        printf("timer %s: %lf\n", infostr, elapsed);
    }
    
    float getCurrentElapsedTime()
    {
        double start = (double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0);
        double end = (double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0);
        float elapsed = end-start;
        return elapsed;
    }
    
private:
    struct timeval startTime;
    struct timeval endTime;
};

class FrameDataSource
{
public:
    virtual bool seekToTime(float pts, bool doConvert = false) = 0;
    virtual const unsigned char *getFrameChunk(int chunk) = 0;
    virtual void workForChunk(int chunk, float pts) {};
};

class C64FrameDataSource : public FrameDataSource
{
public:
    C64FrameDataSource(const char *fname)
    {
        fp = fopen(fname, "rb");
    }
    
    ~C64FrameDataSource()
    {
        if (fp)
        {
            fclose(fp);
            fp = NULL;
        }
    }
    
    bool seekToTime(float pts, bool doConvert = false)
    {
        if (!fp)
            return false;
        
        printf("seek to %f\n", pts);
        //float currpts = 0.0;
        bool newFrame = false;
        
        // TEST
        currpts = -1.0;
        while (currpts < pts && fp)
        {
            newFrame = true;
            size_t res = fread(&currpts, 1, sizeof(float), fp);
            printf("res %d\n", res);
            if (res != sizeof(float))
            {
                printf("closing file\n");
                // eof
                fclose(fp);
                fp = NULL;
                return false;
            }
            else
            {
                // TEST
                currpts = pts;
                
                printf("currpts %f pts %f\n", currpts, pts);
                if (currpts < pts)
                {
                    fseek(fp, 9216, SEEK_CUR);
                }
            }
        }
        
        if (newFrame)
        {
            fread(frame, 1, 9216, fp);
        }
        return true;
    }
    
    void workForChunk(int chunk, float pts)
    {
        if (chunk == 8)
        {
            seekToTime(pts);
        }
    }
    
    const unsigned char *getFrameChunk(int chunk)
    {
        return &frame[chunk*chunksize];
    }
    
private:
    FILE *fp = NULL;
    float currpts = -1.0;
    int chunksize = 1024;
    unsigned char frame[9216];
};

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} color;

void get_64_colors(color *c64_colors)
{
    // black
    c64_colors[0].r = 0;
    c64_colors[0].g = 0;
    c64_colors[0].b = 0;
    
    c64_colors[1].r = 255;
    c64_colors[1].g = 255;
    c64_colors[1].b = 255;
    
    c64_colors[2].r = 104;
    c64_colors[2].g = 55;
    c64_colors[2].b = 43;
    
    c64_colors[3].r = 112;
    c64_colors[3].g = 164;
    c64_colors[3].b = 178;
    
    c64_colors[4].r = 111;
    c64_colors[4].g = 61;
    c64_colors[4].b = 134;
    
    c64_colors[5].r = 88;
    c64_colors[5].g = 141;
    c64_colors[5].b = 67;
    
    c64_colors[6].r = 53;
    c64_colors[6].g = 40;
    c64_colors[6].b = 121;
    
    c64_colors[7].r = 184;
    c64_colors[7].g = 199;
    c64_colors[7].b = 111;
    
    c64_colors[8].r = 111;
    c64_colors[8].g = 79;
    c64_colors[8].b = 37;
    
    c64_colors[9].r = 67;
    c64_colors[9].g = 57;
    c64_colors[9].b = 0;
    
    c64_colors[10].r = 154;
    c64_colors[10].g = 103;
    c64_colors[10].b = 89;
    
    c64_colors[11].r = 68;
    c64_colors[11].g = 68;
    c64_colors[11].b = 68;
    
    c64_colors[12].r = 108;
    c64_colors[12].g = 108;
    c64_colors[12].b = 108;
    
    c64_colors[13].r = 154;
    c64_colors[13].g = 210;
    c64_colors[13].b = 132;
    
    c64_colors[14].r = 108;
    c64_colors[14].g = 94;
    c64_colors[14].b = 181;
    
    c64_colors[15].r = 149;
    c64_colors[15].g = 149;
    c64_colors[15].b = 149;
}

void read_rgb_from_ppm(unsigned char *dest, const char *fname)
{
    FILE *fp = fopen(fname, "rb");
    char temp[128];
    // parse ppm header
    bool done = false;
    int headerlen = 0;
    char *t = temp;
    while (!done)
    {
        size_t res = fread(t, 1, 1, fp);
        headerlen++;
        if (*t == 0x0A)
        {
            // look for "255"
            *t = 0x00;
            printf("got: %s\n", temp);
            
            if (strcmp(temp, "255") == 0)
            {
                done = true;
            }
            
            t = temp;
        }
        else
        {
            t++;
        }
    }
    
    // now read all the pixels
    int width = 320;
    int height = 200;
    int colors = 3;
    
    int bytes = width*height*colors;
    fread(dest, 1, bytes, fp);
    fclose(fp);
}

void bitmap_from_rgb(unsigned char *bitmap, unsigned char *rgb, unsigned char *colormap,
                     unsigned char *mod_rgb, int width, int height, color *colors)
{
    int ncolors = 3;
    int rows = height/8;
    int columns = width/8;
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            int row = h/8;
            int col = w/8;
            int colormap_index = row*columns + col;
            unsigned char colorbyte = colormap[colormap_index];
            
            int c1 = (colorbyte & 0xF0) >> 4;
            int c2 = (colorbyte & 0x0F);
            
            int index = h*width*ncolors + w*ncolors;
            int bitmap_index = h*width + w;
            unsigned char r = rgb[index];
            unsigned char g = rgb[index+1];
            unsigned char b = rgb[index+2];
            
            float dr1 = (float)colors[c1].r-(float)r;
            float dg1 = (float)colors[c1].g-(float)g;
            float db1 = (float)colors[c1].b-(float)b;
            
            float dr2 = (float)colors[c2].r-(float)r;
            float dg2 = (float)colors[c2].g-(float)g;
            float db2 = (float)colors[c2].b-(float)b;
            
            float err1 = dr1*dr1 + dg1*dg1 + db1*db1;
            float err2 = dr2*dr2 + dg2*dg2 + db2*db2;
            
            if (err1 < err2)
            {
                bitmap[bitmap_index] = 1;
                
                mod_rgb[index] = colors[c1].r;
                mod_rgb[index+1] = colors[c1].g;
                mod_rgb[index+2] = colors[c1].b;
            }
            else
            {
                bitmap[bitmap_index] = 0;
                mod_rgb[index] = colors[c2].r;
                mod_rgb[index+1] = colors[c2].g;
                mod_rgb[index+2] = colors[c2].b;
            }
        }
    }
}

unsigned char color_byte_for_block(unsigned char *rgb, int xstart, int ystart, int width, int height, color *colors)
{
    //return 0x10;
    
    //printf("block at %d %d\n", xstart, ystart);
    
    int c1_min = 0;
    int c2_min = 0;
    int ncolors = 3;
    float minerror = 999999.0;
    
#ifdef TESTCOLOR
    c1_min = 0;
    c2_min = 1;
#else
    
     float rtotal;
     float gtotal;
     float btotal;
    float mincolorerror = 999999.0;
    
     // get avg color
     for (int y = ystart; y < ystart+8; y++)
     {
         for (int x = xstart; x < xstart+8; x++)
         {
             int index = y*width*ncolors + x*ncolors;
             unsigned char r = rgb[index];
             unsigned char g = rgb[index+1];
             unsigned char b = rgb[index+2];
             
             rtotal += (float)r;
             gtotal += (float)g;
             btotal += (float)b;
         }
     }
     
     rtotal /= 64.0;
     gtotal /= 64.0;
     btotal /= 64.0;
    
    c1_min = 0;
    for (int c = 0; c < 15; c++)
    {
        float dr1 = (float)colors[c].r-(float)rtotal;
        float dg1 = (float)colors[c].g-(float)gtotal;
        float db1 = (float)colors[c].b-(float)btotal;
        float err1 = dr1*dr1 + dg1*dg1 + db1*db1;
        
        if (err1 < mincolorerror)
        {
            mincolorerror = err1;
            c1_min = c;
        }
    }
    
    //for (int c1 = 0; c1 < 15; c1++)
    //{
        //for (int c2 = c1+1; c2 < 16; c2++)
    for (int c2 = 0; c2 < 16; c2++)
    {
        if (c2 != c1_min)
        {
            //printf("colors %d %d\n", c1, c2);
            // get total error for this color pair
            float totalerror = 0;
            int startpixel = 2;
            int increment = 4;
            //int startpixel = 0;
            //int increment = 1;
            
            for (int y = ystart+startpixel; y < ystart+8; y+=increment)
            {
                for (int x = xstart+startpixel; x < xstart+8; x+=increment)
                {
                    int index = y*width*ncolors + x*ncolors;
                    unsigned char r = rgb[index];
                    unsigned char g = rgb[index+1];
                    unsigned char b = rgb[index+2];
                    
                    float dr1 = (float)colors[c1_min].r-(float)r;
                    float dg1 = (float)colors[c1_min].g-(float)g;
                    float db1 = (float)colors[c1_min].b-(float)b;
                    
                    float dr2 = (float)colors[c2].r-(float)r;
                    float dg2 = (float)colors[c2].g-(float)g;
                    float db2 = (float)colors[c2].b-(float)b;
                    
                    float err1 = dr1*dr1 + dg1*dg1 + db1*db1;
                    float err2 = dr2*dr2 + dg2*dg2 + db2*db2;
                    
                    float minerr = (err1 < err2) ? err1 : err2;
                    totalerror += minerr;
                }
            }
            
            if (totalerror < minerror)
            {
                //c1_min = c1;
                c2_min = c2;
                minerror = totalerror;
            }
        }
    }
    //}
#endif
    
    //printf("colors %d %d\n", c1_min, c2_min);
    
    // create color byte
    unsigned char colorbyte = (c1_min << 4) + c2_min;
    return colorbyte;
}

void colormap_from_rgb(unsigned char *colormap, unsigned char *rgb, int width, int height, color *colors)
{
    int rows = height/8;
    int columns = width/8;
    
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < columns; c++)
        {
            int x = c*8;
            int y = r*8;
            int colormap_index = r*columns + c;
            
            unsigned char colorbyte = color_byte_for_block(rgb, x, y, width, height, colors);
            colormap[colormap_index] = colorbyte;
        }
    }
    //printf("got colormap\n");
}

// input - pixel-addressed bitmap
// output - c64 bitmap bytes
void create_c64_bitmap(unsigned char *dest, unsigned char *src, int width, int height)
{
    int c64bytes = (width/8) * (height/8) * 8;
    // clear
    memset(dest, 0, c64bytes);
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            //printf("src %d = %d\n", h*width+w, src[h*width+w]);
            if (src[h*width + w] == 1)
            {
                int row = h/8;
                int c = w/8;
                int line = h & 7;
                int bit = 7 - (w & 7);
                int byte = row*320 + c*8 + line;
                
                //printf("h %d w %d: row %d col %d line %d bit %d byte %d\n",
                //       h,w,row,c,line,bit,byte);
                
                unsigned char b = dest[byte];
                unsigned char mask = 1 << bit;
                b = b | mask;
                dest[byte] = b;
            }
        }
    }
}

class MP4FrameDataSource : public FrameDataSource
{
public:
    MP4FrameDataSource(const char *fname, float fr) :
    framerate(fr)
    {
        decoder.init(fname);
        get_64_colors(c64_colors);
    }
    
    bool seekToTime(float pts, bool doConvert = false)
    {
        if (done)
            return false;
        
        // get a frame
        float framepts = 0.0;
        bool gotFrame = (currPts < pts) ? false : true;
        while (!gotFrame && !done)
        {
            //bool gf = decoder.getFrameRGB(rgb, true, done, framepts);
            bool gf = decoder.decodeFrame(done, framepts);
            if (gf)
            {
                if (framepts >= pts)
                {
                    if (doConvert)
                    {
                        timer.start();
                        decoder.frameToRGB(rgb);
                        colormap_from_rgb(colormap, rgb, 320, 200, c64_colors);
                        bitmap_from_rgb(bitmap, rgb, colormap, mod_rgb, 320, 200, c64_colors);
                        create_c64_bitmap(c64_bitmap, bitmap, 320, 200);
                        timer.end();
                        timer.report("decode");
                    }
                    
                    gotFrame = true;
                    currPts = framepts;
                }
            }
        }
        
        return gotFrame;
    }
    
    void workForChunk(int chunk, float pts)
    {
        if (chunk == 1)
        {
            bool gotFrame = seekToTime(pts);
        }
        if (chunk == 2)
        {
            decoder.frameToRGB(rgb);
        }
        else if (chunk == 3)
        {
            colormap_from_rgb(colormap, rgb, 320, 200, c64_colors);
        }
        else if (chunk == 4)
        {
            bitmap_from_rgb(bitmap, rgb, colormap, mod_rgb, 320, 200, c64_colors);
        }
        else if (chunk == 8)
        {
            create_c64_bitmap(c64_bitmap, bitmap, 320, 200);
        }
    }
    
    const unsigned char *getFrameChunk(int chunk)
    {
        if (chunk == 0)
        {
            return colormap;
        }
        else
        {
            return &c64_bitmap[1024 * (chunk-1)];
        }
    }
    
private:
    float framerate;
    Decoder decoder;
    bool done = false;
    bool hasFrame = false;
    float currPts = -1.0;
    
    unsigned char rgb[320*200*3];
    unsigned char mod_rgb[320*200*3];
    unsigned char bitmap[320*200];
    unsigned char colormap[1024];
    unsigned char c64_bitmap[8192];
    color c64_colors[16];
    
    Timer timer;
};

void mp4toc64(const char *mp4fname, const char *c64fname, float framerate)
{
    MP4FrameDataSource source(mp4fname, framerate);
    FILE *fp = fopen(c64fname, "wb");
    
    float currTime = 0.0;
    bool done = false;
    while (!done)
    {
        bool gotFrame = source.seekToTime(currTime, true);
        if (gotFrame)
        {
            printf("writing frame at %f\n", currTime);
            fwrite(&currTime, 1, sizeof(float), fp);
            for (int i = 0; i < 9; i++)
            {
                const unsigned char *chunk = source.getFrameChunk(i);
                fwrite(chunk, 1, 1024, fp);
            }
            
            currTime += (1.0 / framerate);
        }
        else
        {
            done = true;
        }
    }
    
    fclose(fp);
}

// test - watch for input
int main(int argc, char **argv)
{
    unsigned char buffer[2048];
    unsigned char bitmap[8192];
    char temp[256];
    Timer playbackTimer;
    
    if (argc < 2)
    {
        printf("usage: pispi <input.c64>\n");
        return 1;
    }
    
    const char *fname = argv[1];
    
    int frameSkip = 5;
    memset(buffer, 0, 1024);
    
    // convert mp4
    mp4toc64(fname, "out3.c64", 6.0);
    C64FrameDataSource source("out3.c64");
    //MP4FrameDataSource source(fname, 6.0);
    
    wiringPiSetup();
    pinMode(0, INPUT);
    pullUpDnControl(0, PUD_UP);
    
    pinMode(1, OUTPUT);
    pullUpDnControl(1, PUD_OFF);
    fastDigitalWrite(1, LOW); // reset MCU
    
    int spi = wiringPiSPISetup(0, 2000000);
    
    delayMicroseconds(100000);
    fastDigitalWrite(1, HIGH);
    
    printf("checking for commands..\n");
    
    int frames = 0;
    unsigned char *imgptr = NULL;
    bool done = false;
    bool started = false;
    while (!done)
    {
        // loop - check for updates
        unsigned char cmd;
        
        while (fastDigitalRead(0) == HIGH)
        {
            delayMicroseconds(10);
        }
        
        // get byte
        int r = read(spi, &cmd, 1);
        
        // start a frame
        if (cmd == 0)
        {
            if (!started)
            {
                started = true;
                playbackTimer.start();
            }
            
            playbackTimer.end();
            
            const unsigned char *frameChunk = source.getFrameChunk(cmd);
            int s = write(spi, frameChunk, 1024);
            
            float currTime = playbackTimer.getCurrentElapsedTime();
            float currFps = (float)frames / currTime;
            
        }
        else if (cmd < 9)
        {
            const unsigned char *frameChunk = source.getFrameChunk(cmd);
            int s = write(spi, frameChunk, 1024);
        }
        
        // wait for deassert
        while (fastDigitalRead(0) == LOW)
        {
            delayMicroseconds(10);
        }
        
        float curr_playback_time = playbackTimer.getCurrentElapsedTime();
        source.workForChunk(cmd, curr_playback_time);
    }
    
    return 1;
}