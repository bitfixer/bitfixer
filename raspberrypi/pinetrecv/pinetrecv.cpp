#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "NetPort.h"
#include "TCP.h"
#include "URLFetcher.h"

#include "YTVideo.h"

#define SEARCHCMD   11
#define SEARCHPAGE  12

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
    
    int vv = 0;
    while (!done)
    {
        int recv_size = client.recv(buffer, 1024);
        if (recv_size > 0)
        {
            cmdPacket* pkt = (cmdPacket*)buffer;
            printf("received command %d size %d\n", pkt->cmd, recv_size);
            
            int dataSize = recv_size - 3;
            pkt->data[dataSize] = 0;
            printf("got string %s\n", pkt->data);
            memcpy(temp, pkt->data, 256);
            //client.send(buffer, 1);
            
            for (int d = 0; d < dataSize; d++)
            {
                printf("data %d is %d 0x%X\n", d, temp[d], temp[d]);
            }
            
            //std::vector<YouTube::Video> videos = YouTube::search("cats", 1);
            std::vector<YouTube::Video> videos = YouTube::search(temp, 1);
            printf("got %d results.\n", videos.size());
            
            printf("video: %s\n", videos[0].title().c_str());
            
            
            memset(buffer, 0, 2048);
            //sprintf((char*)buffer, "%s: resp %d", temp, vv++);
            
            memcpy(buffer, videos[0].title().c_str(), videos[0].title().length());
            client.send(buffer, 256);
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
