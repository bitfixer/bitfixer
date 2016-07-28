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

class Decoder
{
public:
    Decoder() {};
    int init();
    
    bool getFrameRGB(unsigned char *rgb, int frameIndex);
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
};

void SaveFrameFromRgb(unsigned char *rgb, int width, int height, int iFrame);