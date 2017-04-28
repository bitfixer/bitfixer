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

class PETFile
{
public:
    PETFile(const char* fname, const char* dirname)
    : _fname(NULL)
    {
        // copy input filename
        _fname = new char[strlen(fname)+1];
        strcpy(_fname, fname);

        _dirname = new char[strlen(dirname)+1];
        strcpy(_dirname, dirname);

        _canonicalFname = new char[strlen(fname)+1];
        for (int i = 0; i < strlen(fname); i++)
        {
            _canonicalFname[i] = tolower(_fname[i]);
        }
        _canonicalFname[strlen(fname)] = 0;

        _fullFname = new char[strlen(_dirname) + 1 + strlen(_canonicalFname) + 1];
        sprintf(_fullFname, "%s/%s", _dirname, _canonicalFname);
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

private:
    char* _fname;
    char* _dirname;
    char* _canonicalFname;
    char* _fullFname;
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
    mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    //list_dir(dirname);

    unsigned char buffer[512];
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

    bool reading = false;
    bool writing = false;
    FILE* prgfp = NULL;
    // directory
    DIR* dir = NULL;

    int temp = 5;
    while(1)
    {
        // read command
        spi.transfer((unsigned char*)&cmd, sizeof(petDiskCommand));
        //printf("fname: %s\n", cmd.arg);
        if (cmd.command_id == PD_CMD_OPEN_FILE_FOR_READING)
        {
            printf("reading\n");
            unsigned char filename[256];
            unsigned char fullname[512];
            uint16_t size = 0;

            if (prgfp)
            {
                fclose(prgfp);
                prgfp = NULL;
            }

            printf("open for reading: %s\n", cmd.arg);
            bool found = find_file(cmd.arg, dirname, filename);
            if (found)
            {
                sprintf((char*)fullname, "%s/%s", dirname, filename);
                printf("found %s, full %s\n", filename, fullname);
                prgfp = fopen((const char*)fullname, "rb");
                fseek(prgfp, 0, SEEK_END);
                size = (uint16_t)ftell(prgfp);
                fseek(prgfp, 0, SEEK_SET);
            }


            // send the file length
            unsigned char sizeBytes[2];
            sizeBytes[0] = (size & 0xFF00) >> 8;
            sizeBytes[1] = size & 0x00FF;

            spi.transfer(sizeBytes, 2);

            reading = true;
            writing = false;
        }
        else if (cmd.command_id == PD_CMD_OPEN_FILE_FOR_WRITING)
        {
            PETFile petfile((const char*)cmd.arg, (const char*)dirname);
            printf("filename: %s\n", petfile.getFullFname());
            prgfp = fopen(petfile.getFullFname(), "wb");
            reading = false;
            writing = true;
        }
        else if (cmd.command_id == PD_CMD_READ_BLOCK)
        {
            int bytes_read = fread(buffer, 1, 512, prgfp);
            //printf("read %d bytes\n", bytes_read);
            spi.transfer(buffer, 512);
        }
        else if (cmd.command_id == PD_CMD_WRITE_BLOCK)
        {
            int bytes_read = spi.transfer(buffer, 512);
            printf("writing %d bytes\n", bytes_read);
            fwrite(buffer, 1, bytes_read, prgfp);
        }
        else if (cmd.command_id == PD_CMD_CLOSE_FILE)
        {
            printf("closing file\n");
            if (prgfp)
            {
                fclose(prgfp);
                prgfp = NULL;
            }
            reading = false;
            writing = false;
        }
        else if (cmd.command_id == PD_CMD_DIRECTORY)
        {
            printf("open directory.\n");
            if (dir)
            {
                closedir(dir);
                dir = NULL;
            }

            dir = opendir(dirname);
        }
        else if (cmd.command_id == PD_CMD_GET_NEXT_DIRECTORY_ENTRY)
        {
            struct dirent* entry = NULL;
            DirectoryEntry* dirent = (DirectoryEntry*)buffer;

            while ((entry = readdir(dir)) != NULL)
            {
                if (entry->d_type == DT_REG)
                {
                    // regular file
                    break;
                }
            }

            if (entry != NULL)
            {
                // get extension
                int maxlen = 17;
                int namelen = strlen(entry->d_name);
                int extstartindex = -1;
                if (entry->d_name[namelen-4] == '.')
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
                    dirent->name[i] = toupper(entry->d_name[i]);
                }

                printf("name %s name_length: %d\n", dirent->name, dirent->name_length);

                memset(dirent->ext, 0, 3);
                if (extstartindex > 0)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        dirent->ext[i] = toupper(entry->d_name[extstartindex + i]);
                    }
                }
                temp--;
            }
            else
            {
                dirent->valid = 0;
                closedir(dir);
                dir = NULL;
            }

            spi.transfer(buffer, 256);
        }
    }

    return 1;
}
