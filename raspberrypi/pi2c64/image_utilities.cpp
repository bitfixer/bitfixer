extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
}

#include <stdio.h>
#include <string>

const int final_height = 200;
const int final_width = 320;
const int num_channels = 3;

bool initialize_ffmpeg() {
	av_register_all();
	return true;
}

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
	printf("Saving frame: %d\n", iFrame);
	FILE *pFile;
	char szFilename[32];
	int y = 0;

	// Open file
	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile = fopen(szFilename, "wb");

	if (pFile == NULL) {
		return;
	}

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (y=0; y < height; y++) {
		fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
	}

	fclose(pFile);
}

void SaveFrameFromRgb(unsigned char *rgb, int width, int height, int iFrame) {
    printf("Saving frame: %d\n", iFrame);
    FILE *pFile;
    char szFilename[32];
    int y = 0;
    
    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile = fopen(szFilename, "wb");
    
    if (pFile == NULL) {
        return;
    }
    
    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
    
    int ncolors = 3;
    // Write pixel data
    for (y=0; y < height; y++) {
        //fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
        for (int x = 0; x < width; x++)
        {
            int index = y*width*ncolors + x*ncolors;
            fwrite(&rgb[index], 1, ncolors, pFile);
        }
    }
    
    fclose(pFile);
}


int AVFrameTo3dArr(AVFrame *pFrame, char ** &dstArr, int width, int height, int iFrame) {
	// Assumes dst array is (width*3) x (height)
	char r, g, b = 0;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int p = x*num_channels + y*pFrame->linesize[0];
			r = pFrame->data[0][p];
			g = pFrame->data[0][p+1];
			b = pFrame->data[0][p+2];

			dstArr[x+0][y] = r;
			dstArr[x+1][y] = g;
			dstArr[x+2][y] = b;
		}
	}

	return 0;
}

int open_video(std::string inputFile, AVCodecContext * &pCodecCtx, AVCodec * &pCodec, AVFormatContext * &pFormatCtx, int *videoStream){

	/**************************************************
	 * Opening file
	**************************************************/

	// Open File
	int return_code = avformat_open_input(&pFormatCtx, inputFile.c_str(), NULL, NULL);
	if (return_code != 0){
		char temp_str[1024];
		av_strerror(return_code, temp_str, sizeof(temp_str));
		printf("Error code when opening file: %s\n", temp_str);
		return -1;	
	}

	// Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		return -1;
	}

	// Dump information about file onto standard error
	//dump_format(pFormatCtx, 0, argv[1], 0);

	int i;

	// Find the first video stream
	*videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) {
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			*videoStream=i;
			break;
		}
	}
	if(*videoStream==-1) {
	      return -1; // Didn't find a video stream
	}
	
	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[*videoStream]->codec;

	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		printf("Unsupported codec found!\n");
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		return -1;
	}
}

int reading_loop( AVCodecContext *pCodecCtx, AVCodec *pCodec, AVFormatContext *pFormatCtx, int *videoStream){
	/**************************************************
	 * Storing Data
	**************************************************/
	AVFrame *pFrame = NULL;
	AVFrame *pFrameRGB = NULL;

	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL) {
		return -1;
	}

	uint8_t *buffer;
	int numBytes = avpicture_get_size(PIX_FMT_RGB24, final_width, final_height);
	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, final_width, final_height);

	int frameFinished;
	AVPacket packet;

	int i = 0;

	static struct SwsContext *img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, final_width, final_height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL); 

	// WHILE LOOP
	while(av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == *videoStream) {
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
		}

		if (frameFinished) {
			// Convert the image from native to RGB

			// deprecated
			//img_convert((AVPicture *) pFrameRGB, PIX_FMT_RGB24, (AVPicture *) pFrame, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
			if ( sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize) < 0) {
				return -1;
			}

			if (++i<=2) {
				SaveFrame(pFrameRGB, final_width, final_height, i);
			}
		}

		av_free_packet(&packet);

		if (i > 2) {
			break;
		}
	}

	// Clean Up - TODO: seperate
	av_free(buffer);
	av_free(pFrameRGB);
	av_free(pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

}

/*
int main() {
	// Initialize
	printf("Initializing ffmpeg\n");
	initialize_ffmpeg();

	// Open Video
	AVCodecContext *pCodecCtx = NULL;
	AVCodec *pCodec = NULL;
	AVFormatContext *pFormatCtx = NULL;
	int streamNum = -1;
	int *videoStream = &streamNum;
	open_video("dirkMonoClip.mp4", pCodecCtx, pCodec, pFormatCtx, videoStream);

	// Loop
	reading_loop(pCodecCtx, pCodec, pFormatCtx, videoStream);
	printf("Finished Main!\n");
}
*/
