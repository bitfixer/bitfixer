#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "commands.h"
#include "image_utilities.h"

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


void set_port_input()
{
    for (int i = 0; i < 8; i++)
    {
        pinMode(i, INPUT);
        pullUpDnControl(i, PUD_OFF);
    }
}

void set_port_output()
{
    for (int i = 0; i < 8; i++)
    {
        pinMode(i, OUTPUT);
        pullUpDnControl(i, PUD_OFF);
    }
}

void init()
{
    wiringPiSetup();
    // set all pins input
    set_port_input();
    
    pinMode(8, INPUT);
    pullUpDnControl(8, PUD_DOWN);
    
    pinMode(9, OUTPUT);
    fastDigitalWrite(9, HIGH);
}

unsigned char piReadByte()
{
    unsigned char byte = 0;
    for (int i = 7; i >= 0; i--)
    {
        byte <<= 1;
        byte += fastDigitalRead(i);
    }
    return byte;
}

void piWriteByte(unsigned char byte)
{
    unsigned char bit = 0;
    for (int i = 0; i < 8; i++)
    {
        fastDigitalWrite(i, byte & 0x01);
        byte >>= 1;
    }
}

bool wait_for_signal(int timeout_usec)
{
    bool found = false;
    int usec = 0;
    while (!found && (timeout_usec == -1 || usec < timeout_usec))
    {
        int readval = fastDigitalRead(8);
        if (readval == 0)
        {
            found = true;
        }
        else
        {
            delayMicroseconds(1);
            usec++;
        }
    }
    
    //printf("wait for signal timed out.\n");
    return found;
}

bool wait_for_signal_notready(int timeout_usec)
{
    bool found = false;
    int usec = 0;
    while (!found && (timeout_usec == -1 || usec < timeout_usec))
    {
        int readval = fastDigitalRead(8);
        if (readval == 1)
        {
            found = true;
        }
        else
        {
            delayMicroseconds(1);
            usec++;
        }
    }
    
    //printf("wait for notready timed out.\n");
    return found;
}

void signal_ready()
{
    // lower the handshake line
    // c64 is looking for negative edge
    fastDigitalWrite(9, LOW);
}

void signal_notready()
{
    fastDigitalWrite(9, HIGH);
    //usleep(1);
    delayMicroseconds(1);
    fastDigitalWrite(9, LOW);
    //usleep(1);
    delayMicroseconds(1);
    fastDigitalWrite(9, HIGH);
}

unsigned char receive_byte_with_handshake()
{
    fastDigitalWrite(9, HIGH);
    wait_for_signal(-1);
    unsigned char byte = piReadByte();
    signal_ready();
    // c64 has received our signal
    wait_for_signal_notready(-1);
    signal_notready();
    return byte;
}

void send_byte_with_handshake(unsigned char byte)
{
    fastDigitalWrite(9, HIGH);
    while (!wait_for_signal(10000))
    {
        signal_notready();
    }
    piWriteByte(byte);
    signal_ready();
    while (!wait_for_signal_notready(10000))
    {
        signal_notready();
    }
    signal_notready();
}

void receive_command()
{
    unsigned char cmd;
    cmd = receive_byte_with_handshake();
    printf("cmd is %d\n", cmd);
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
            
            float err1 = sqrt(dr1*dr1 + dg1*dg1 + db1*db1);
            float err2 = sqrt(dr2*dr2 + dg2*dg2 + db2*db2);
            
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
    
    for (int c1 = 0; c1 < 15; c1++)
    {
        for (int c2 = c1+1; c2 < 16; c2++)
        {
            //printf("colors %d %d\n", c1, c2);
            // get total error for this color pair
            float totalerror = 0;
            for (int y = ystart; y < ystart+8; y++)
            {
                for (int x = xstart; x < xstart+8; x++)
                {
                    int index = y*width*ncolors + x*ncolors;
                    unsigned char r = rgb[index];
                    unsigned char g = rgb[index+1];
                    unsigned char b = rgb[index+2];
                    
                    float dr1 = (float)colors[c1].r-(float)r;
                    float dg1 = (float)colors[c1].g-(float)g;
                    float db1 = (float)colors[c1].b-(float)b;
                    
                    float dr2 = (float)colors[c2].r-(float)r;
                    float dg2 = (float)colors[c2].g-(float)g;
                    float db2 = (float)colors[c2].b-(float)b;
                    
                    float err1 = sqrt(dr1*dr1 + dg1*dg1 + db1*db1);
                    float err2 = sqrt(dr2*dr2 + dg2*dg2 + db2*db2);
                    
                    float minerr = (err1 < err2) ? err1 : err2;
                    totalerror += minerr;
                }
            }
            
            if (totalerror < minerror)
            {
                c1_min = c1;
                c2_min = c2;
                minerror = totalerror;
            }
        }
    }
    
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
            //unsigned char colorbyte = 0x1;
            colormap[colormap_index] = colorbyte;
            
            /*
            for (int yy = y; yy < y+8; y++)
            {
                for (int xx = x; xx < x+8; x++)
                {
                    int index = yy*width*ncolors + xx*ncolors;
                    int bitmap_index = h*width + w;
                    unsigned char r = rgb[index];
                    unsigned char g = rgb[index+1];
                    unsigned char b = rgb[index+2];
                    
                    *bl = r;
                    bl++;
                    *bl = g;
                    bl++;
                    *bl = b;
                }
            }
            */
        }
    }
    printf("got colormap\n");
}

void create_test_bitmap(unsigned char *dest, int width, int height)
{
    // first clear the bitmap
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            dest[h*width + w] = 0;
        }
    }
    
    for (int h = 0; h < 8; h++)
    {
        for (int w = 0; w < width; w++)
        {
            //printf("setting index %d to 1\n", h*width+w);
            dest[h*width + w] = 1;
        }
    }
    
    /*
    float xcenter = (float)width/(float)2.0;
    float ycenter = (float)height/(float)2.0;
    float radius = 60.0;
    float linewidth = 5.0;
    
    // draw a circle
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            float xpos = (float)w + 0.5;
            float ypos = (float)h + 0.5;
            
            float xdist = xpos-xcenter;
            float ydist = ypos-ycenter;
            
            float thisrad = sqrt(xdist*xdist + ydist*ydist);
            if (thisrad >= radius - (linewidth/2.0) &&
                thisrad <= radius + (linewidth/2.0))
            {
                dest[h*width + w] = 1;
            }
        }
    }
    */
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

void generate_test_rgb(unsigned char *rgb, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float grad = (float)y / (float)height;
            float r = grad * 255.0;
            
            float gradx = (float)x / (float)width;
            float g = gradx * 255.0;
            
            int index = y*width*3 + x*3;
            rgb[index] = (int)r;
            rgb[index+1] = (int)g;
            rgb[index+2] = 0;
        }
    }
}

// test - watch for input
int main(void)
{
    struct timeval startTime;
    struct timeval endTime;
    bool started = false;
    unsigned char rgb[320*200*3];
    unsigned char mod_rgb[320*200*3];
    unsigned char bitmap[320*200];
    unsigned char colormap[1000];
    unsigned char c64_bitmap[8000];
    color c64_colors[16];
    char temp[128];
    get_64_colors(c64_colors);
    bool done = false;
    int bytesReceived = 0;
    
    // pre-process
    Decoder decoder;
    decoder.init();
    
    /*
    for (int i = 0; i < 1; i++)
    {
        int gotFrames = 0;
        // get a frame from decoder
        while (gotFrames < 5)
        {
            bool gotFrame = decoder.getFrameRGB(rgb, 99);
            if (gotFrame)
            {
                gotFrames++;
            }
        }
        
        SaveFrameFromRgb(rgb, 320, 200, i);
    }
    
    return 1;
    */
    
    
    
    init();
    
    for (int i = 0; i < 10; i++)
    {
        int gotFrames = 0;
        // get a frame from decoder
        while (gotFrames < 5)
        {
            bool gotFrame = decoder.getFrameRGB(rgb, 99);
            if (gotFrame)
            {
                gotFrames++;
            }
        }
         
        //generate_test_rgb(rgb, 320, 200);
        printf("got frame.\n");
        //read_rgb_from_ppm(rgb, (const char *)"avh.ppm");
        colormap_from_rgb(colormap, rgb, 320, 200, c64_colors);
        bitmap_from_rgb(bitmap, rgb, colormap, mod_rgb, 320, 200, c64_colors);
        create_c64_bitmap(c64_bitmap, bitmap, 320, 200);
        
        sprintf(temp, "pframe_%04d.c64", i);
        FILE *fp = fopen(temp, "wb");
        fwrite(colormap, 1, 1000, fp);
        fwrite(c64_bitmap, 1, 8000, fp);
        fclose(fp);
        printf("ready.\n");
        
        /*
        int index = i % 10;
        sprintf(temp, "frame_%04d.c64", index);
        FILE *fp = fopen(temp, "rb");
        fread(colormap, 1, 1000, fp);
        fread(c64_bitmap, 1, 8000, fp);
        fclose(fp);
        */
         
        // send one bitmap frame
        receive_command();
        if (!started)
        {
            started = true;
            gettimeofday(&startTime, NULL);
        }
        
        set_port_output();
        for (int i = 0; i < 1000; i++)
        {
            //printf("sending byte %d\n", i);
            send_byte_with_handshake(colormap[i]);
        }
        
        //for (int j = 0; j < 8000; j++)
        for (int j = 0; j < 7936; j++)
        {
            send_byte_with_handshake(c64_bitmap[j]);
        }
        bytesReceived += 9000;
        set_port_input();
    }
    
    gettimeofday(&endTime, NULL);
    double start = (double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0);
    double end = (double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0);
    double elapsed = end-start;
    
    printf("start %lf end %lf elapsed %lf bytes %d BPS %f\n", start, end, elapsed, bytesReceived, (float)bytesReceived / elapsed);
     
    return 1;
}