// Get a list of videos
// Download videos
// return frame
// subsample tiny frame
// 	4 bit color (0-15)
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
}

class Projection
{
public:
    void init(float distance, float fx, float fy, int width, int height, int srcwidth, int srcheight);
    
    int getProjX(int x, int y)
    {
        return projx[x][y];
    }
    int getProjY(int x, int y)
    {
        return projy[x][y];
    }
    
private:
    float d;
    float fovx;
    float fovy;
    float rectw;
    float recth;
    float xcenter;
    float ycenter;
    float srccenterx;
    float srccentery;
    
    int **projx;
    int **projy;
    
};

class Decoder
{
public:
    Decoder() {};
    int init();
    
    bool getFrameRGB(unsigned char *rgb, bool useFrame);
    void projectFrame(AVFrame *frame, unsigned char *rgb, int width, int height, int srcwidth, int srcheight);
    
private:
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVFormatContext *pFormatCtx = NULL;
    int streamNum = -1;
    int *videoStream = &streamNum;
    
    AVFrame *pFrame = NULL;
    AVFrame *pFrameRGB = NULL;
    uint8_t *buffer;
    
    int frameFinished;
    AVPacket packet;
    struct SwsContext *img_convert_ctx;
    
    Projection proj;
};

void SaveFrameFromRgb(unsigned char *rgb, int width, int height, int iFrame);