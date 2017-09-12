#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "NetPort.h"
#include "TCP.h"
#include "URLFetcher.h"
#include "YTVideo.h"
#include "petscii.h"


#define SEARCHCMD   11
#define PLAYCMD     12

class FrameDataSource
{
public:
    virtual bool seekToTime(float pts, bool doConvert = false) = 0;
    virtual const unsigned char *getFrameChunk(int chunk) = 0;
    virtual void workForChunk(int chunk, float pts) {};
};

class NetworkFrameDataSource : public FrameDataSource
{
public:
    NetworkFrameDataSource()
    : port(127,0,0,1,99999,111111)
    {
        memset(frame, 0, 9216);
    }

    ~NetworkFrameDataSource()
    {
    }

    bool seekToTime(float pts, bool doConvert = false)
    {
        memset(frame, 0, frameSize);
        // get UDP frame
        int ret = port.recv_sync(frame, frameSize);
    }

    void workForChunk(int chunk, float pts)
    {
        if (chunk == 8)
        {
            seekToTime(pts);
        }
    }

    const unsigned char* getFrameChunk(int chunk)
    {
        return &frame[chunk*chunksize];
    }
private:
    NetPort port;
    int chunksize = 1024;
    int frameSize = 9216;
    unsigned char temp[9220];
    unsigned char frame[9216];
};

class PETNetworkFrameDataSource : public FrameDataSource
{
public:
    PETNetworkFrameDataSource()
    : port(127,0,0,1,88888,111111)
    {
        memset(frame, 0, frameSize);
    }

    ~PETNetworkFrameDataSource()
    {
    }

    bool seekToTime(float pts, bool doConvert = false)
    {
        memset(frame, 0, frameSize);
        printf("Waiting for UDP frame..\n");
        // get UDP frame
        int ret = port.recv_sync(frame, frameSize);
        printf("PET frame\n");
    }

    void workForChunk(int chunk, float pts)
    {
    }

    const unsigned char* getFrameChunk(int chunk)
    {
        seekToTime(0);
        return &frame[chunk*chunksize];
    }
private:
    NetPort port;
    int chunksize = 2000;
    int frameSize = 2000;
    unsigned char frame[4000];
};

typedef struct
{
    unsigned char device_id;
    unsigned char cmd;
    unsigned char len;
    unsigned char data[256];
} cmdPacket;

typedef struct
{
    unsigned char index;
    unsigned char numresults;
    unsigned char data[254];
} vidResult;

// test - watch for input
int main(int argc, char **argv)
{
    unsigned char buffer[2048];
    unsigned char bitmap[8192];
    char temp[256];
    int frameSkip = 5;
    memset(buffer, 0, 1024);

    FrameDataSource* source = new NetworkFrameDataSource();
    PETNetworkFrameDataSource petDataSource;

    TCPClient client("127.0.0.1", 44444);
    client.connect();

    // send device id
    unsigned char dev_id = 19;
    int s = client.send(&dev_id, 1);
    printf("checking for commands2..\n");

    int frames = 0;
    bool done = false;
    std::vector<YouTube::Video> videos;
    int videoIndex = 0;

    int vv = 0;
    while (!done)
    {
        int recv_size = client.recv(buffer, 1024);
        if (recv_size > 0)
        {
            cmdPacket* pkt = (cmdPacket*)buffer;
            printf("received command %d len %d recv_size %d\n", pkt->cmd, pkt->len, recv_size);

            if (pkt->cmd == SEARCHCMD)
            {
                if (pkt->len > 0)
                {
                    pkt->data[pkt->len] = 0;
                    printf("got string %s\n", pkt->data);
                    memcpy(temp, pkt->data, 256);

                    videos = YouTube::search(temp, 1);
                    printf("got %d results.\n", videos.size());
                    //printf("video: %s\n", videos[0].title().c_str());
                    videoIndex = 0;
                }

                memset(buffer, 0, 2048);
                std::string petsciiTitle = Petscii::asciiToPetscii(videos[videoIndex].title());
                //memcpy(buffer, petsciiTitle.c_str(), petsciiTitle.length());
                vidResult* result = (vidResult*)buffer;
                result->index = videoIndex;
                result->numresults = videos.size();
                memcpy(result->data, petsciiTitle.c_str(), petsciiTitle.length());
                printf("sending: %d %d %s\n", result->index, result->numresults, result->data);

                client.send(buffer, 256);
                videoIndex++;
            }
            else if (pkt->cmd == PLAYCMD)
            {
                if (pkt->len > 0)
                {
                    printf("got selection: %d\n", pkt->data[0]);
                    int selection = pkt->data[0];
                    printf("yt id: %s\n", videos[selection].id().c_str());
                }

                client.send(buffer, 256);
            }
            else if (pkt->cmd < 9)
            {
                printf("chunk command: %d\n", pkt->cmd);
                if (pkt->cmd == 0)
                {
                    const unsigned char *frameChunk = source->getFrameChunk(pkt->cmd);
                    client.send((unsigned char*)frameChunk, 1024);
                }
                else if (pkt->cmd < 9)
                {
                    const unsigned char *frameChunk = source->getFrameChunk(pkt->cmd);
                    client.send((unsigned char*)frameChunk, 1024);
                }

                source->workForChunk(pkt->cmd, 0.0);
            }
        }
    }

    /*
    while (!done)
    {
        int recv_size = client.recv(buffer, 1024);
        if (recv_size > 0)
        {
            unsigned char cmd = buffer[1];
            printf("received command %d\n", cmd);
            // start a frame
            if (cmd == 0)
            {
                const unsigned char *frameChunk = source->getFrameChunk(cmd);
                client.send((unsigned char*)frameChunk, 1024);
            }
            else if (cmd < 9)
            {
                const unsigned char *frameChunk = source->getFrameChunk(cmd);
                client.send((unsigned char*)frameChunk, 1024);
            }

            source->workForChunk(cmd, 0.0);
        }
    }
    */

    delete source;
    return 1;
}
