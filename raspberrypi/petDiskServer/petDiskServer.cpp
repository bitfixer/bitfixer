#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>
#include "rpiThreeWireSPI.h"
#include "petDiskCommand.h"
#include "URLFetcher.h"
#include "Directory.h"
#include "StringSplitter.h"
#include "URL.h"
#include "URLUploader.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <curl/curl.h>

/*
int main(int argc, char **argv)
{
    MemoryStruct mem;
    mem.memory = (char*)malloc(5);
    mem.memory[0] = 'A';
    mem.memory[1] = 'B';
    mem.memory[2] = 'C';
    mem.memory[3] = 'D';
    mem.memory[4] = 'E';
    mem.size = 5;

    URLUploader uploader("http://www.bitfixer.com/pd/");
    uploader.upload("zzzyyy.prg", mem);
    printf("done.\n");
}
*/

class PETFile
{
public:
    PETFile(const char* fname, const char* dirname)
    : _fname(NULL)
    , _mem(NULL)
    , _memPos(0)
    {
        // copy input filename
        _fname = new char[strlen(fname)+1];
        strcpy(_fname, fname);

        _dirname = new char[strlen(dirname)+1];
        strcpy(_dirname, dirname);

        _canonicalFname = new char[strlen(fname)+1];
        for (int i = 0; i < strlen(fname); i++)
        {
            //_canonicalFname[i] = tolower(_fname[i]);
            _canonicalFname[i] = _fname[i];
        }
        _canonicalFname[strlen(fname)] = 0;

        _fullFname = new char[strlen(_dirname) + 1 + strlen(_canonicalFname) + 1];
        //sprintf(_fullFname, "%s/%s", _dirname, _canonicalFname);
        sprintf(_fullFname, "%s%s", _dirname, _canonicalFname);
    }

    ~PETFile()
    {
        if (_fname)
        {
            delete[] _fname;
        }

        if (_canonicalFname)
        {
            delete[] _canonicalFname;
        }

        if (_dirname)
        {
            delete[] _dirname;
        }

        if (_fullFname)
        {
            delete[] _fullFname;
        }
    }

    const char* getFullFname()
    {
        return _fullFname;
    }

    void open()
    {
        // if url
        if (_mem)
        {
            delete _mem;
            _mem = NULL;
        }

        _mem = new MemoryStruct();
        _fetcher.fetchURL(_fullFname, *_mem);
        printf("dl %d bytes\n", _mem->size);
    }

    int read(unsigned char* buffer, int size)
    {
        int bytesLeft = _mem->size - _memPos;
        int bytesToRead = size;
        if (bytesToRead > bytesLeft)
        {
            bytesToRead = bytesLeft;
        }

        memcpy(buffer, &(_mem->memory[_memPos]), bytesToRead);
        _memPos += bytesToRead;

        return bytesToRead;
    }

    int getSize()
    {
        if (_mem)
        {
            return _mem->size;
        }

        return 0;
    }

private:
    char* _fname;
    char* _dirname;
    char* _canonicalFname;
    char* _fullFname;

    URLFetcher _fetcher;
    MemoryStruct* _mem;
    int _memPos;
};

void list_dir(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s type %d len %d\n",entry->d_name, entry->d_type, entry->d_reclen);
    }

    closedir(dir);
}

bool find_file(unsigned char* fname, const char* path, unsigned char* buffer)
{
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            // check filename
            //printf("checking %s against %s\n", fname, entry->d_name);
            if (strlen((char*)fname) == strlen(entry->d_name))
            {
                int len = strlen((char*)fname);
                bool match = true;
                for (int i = 0; i < len; i++)
                {
                    if (toupper(fname[i]) != toupper(entry->d_name[i]))
                    {
                        // not a match
                        match = false;
                        break;
                    }
                }

                if (match)
                {
                    // found the file
                    //printf("MATCH %s against %s\n", fname, entry->d_name);
                    break;
                }
            }
        }
    }

    if (entry != NULL)
    {
        strcpy((char*)buffer, (const char*)entry->d_name);
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: petDiskServer <dir>\n");
        exit(0);
    }

    const char* dirname = argv[1];
    printf("directory %s\n", dirname);
    PETFile* petFile = NULL;
    Directory directory;
    unsigned char buffer[512];

    directory.fetch(dirname);
    mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    int value = 0;
    rpiThreeWireSPI spi(1, // clock
                        0, // chip select
                        3, // miso
                        2 // mosi
                        );

    wiringPiSetup();
    spi.init();

    memset(buffer, 0, 512);
    petDiskCommand cmd;

    //bool reading = false;
    //bool writing = false;
    // directory
    DIR* dir = NULL;
    int directoryIndex = 0;
    while(1)
    {
        // read command
        spi.transfer((unsigned char*)&cmd, sizeof(petDiskCommand));

        if (cmd.command_id == PD_CMD_OPEN_FILE_FOR_READING)
        {
            uint16_t size = 0;
            printf("open for reading: %s\n", cmd.arg);

            // find the file in the directory
            int dirindex = directory.find((const char*)cmd.arg);

            if (dirindex >= 0)
            {
                string fn = directory.getEntry(dirindex);

                if (petFile)
                {
                    delete petFile;
                    petFile = NULL;
                }

                petFile = new PETFile(fn.c_str(), dirname);
                char* n = (char*)petFile->getFullFname();
                petFile->open();
                size = petFile->getSize();
            }

            // send the file length
            unsigned char sizeBytes[2];
            sizeBytes[0] = (size & 0xFF00) >> 8;
            sizeBytes[1] = size & 0x00FF;

            spi.transfer(sizeBytes, 2);
        }
        else if (cmd.command_id == PD_CMD_OPEN_FILE_FOR_WRITING)
        {
            //PETFile petfile((const char*)cmd.arg, (const char*)dirname);
            //printf("filename: %s\n", petfile.getFullFname());
            //prgfp = fopen(petfile.getFullFname(), "wb");
        }
        else if (cmd.command_id == PD_CMD_READ_BLOCK)
        {
            memset(buffer, 0, 512);
            int bytes_read = petFile->read(buffer, 512);
            spi.transfer(buffer, 512);
        }
        else if (cmd.command_id == PD_CMD_WRITE_BLOCK)
        {
            int bytes_read = spi.transfer(buffer, 512);
            printf("writing %d bytes\n", bytes_read);
            //fwrite(buffer, 1, bytes_read, prgfp);
        }
        else if (cmd.command_id == PD_CMD_CLOSE_FILE)
        {
            printf("closing file\n");
            /*
            if (prgfp)
            {
                fclose(prgfp);
                prgfp = NULL;
            }
            */
        }
        else if (cmd.command_id == PD_CMD_DIRECTORY)
        {
            directoryIndex = 0;
        }
        else if (cmd.command_id == PD_CMD_GET_NEXT_DIRECTORY_ENTRY)
        {
            DirectoryEntry* dirent = (DirectoryEntry*)buffer;

            if (directoryIndex < directory.numEntries())
            {
                string name = directory.getName(directoryIndex++);
                // get extension
                int maxlen = 17;
                int namelen = name.length();
                int extstartindex = -1;
                if (name.at(namelen-4) == '.')
                {
                    extstartindex = namelen - 3;
                    namelen = namelen - 4;
                }

                if (namelen >= maxlen)
                {
                    namelen = maxlen;
                }

                dirent->valid = 1;
                dirent->name_length = (unsigned char)namelen;

                memset(dirent->name, 0, maxlen);
                for (int i = 0; i < namelen; i++)
                {
                    dirent->name[i] = toupper(name.at(i));
                }

                printf("name %s name_length: %d\n", dirent->name, dirent->name_length);

                memset(dirent->ext, ' ', 3);
                if (extstartindex > 0)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        dirent->ext[i] = toupper(name.at(extstartindex + i));
                    }
                }
            }
            else
            {
                dirent->valid = 0;
            }

            spi.transfer(buffer, 256);
        }
        else if (cmd.command_id == PD_CMD_CHANGE_DIRECTORY)
        {
            printf("change directory: %s\n", cmd.arg);
        }
    }

    return 1;
}
