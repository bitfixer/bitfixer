/* Audiotap shared library: a higher-level interface to TAP shared library
 * 
 * Header file for audiofile library, modified by replacing prototypes with
 * pointers to functions, so functions can be loaded with dlsym/GetProcAddress.
 *
 * Original file (C) 1998-2000, Michael Pruett <michael@68k.org>
 * This file (C) Fabrizio Gennari, 2003
 *
 * The program is distributed under the GNU Lesser General Public License.
 * See file LESSER-LICENSE.TXT for details.
 */

#if !defined AUDIOFILE_DECLARE_HERE
#define EXTERN extern
#elif __GNUC__ >= 4
#define EXTERN __attribute__ ((visibility ("hidden")))
#else
#define EXTERN
#endif

#include <stdint.h>
#include <sys/types.h>

typedef struct _AFfilesetup *AFfilesetup;
typedef struct _AFfilehandle *AFfilehandle;

#if defined(__FreeBSD__) || \
	defined(__DragonFly__) || \
	defined(__NetBSD__) || \
	defined(__OpenBSD__) || \
	defined(__APPLE__) || \
	defined(__sgi) || \
	(defined(__linux__) && defined(__LP64__))
// BSD and IRIX systems define off_t as a 64-bit signed integer.
// Linux defines off_t as a 64-bit signed integer in LP64 mode.
typedef off_t AFframecount;
typedef off_t AFfileoffset;
#else
// For all other systems, use int64_t.
typedef int64_t AFframecount;
typedef int64_t AFfileoffset;
#endif

#define AF_NULL_FILESETUP       ((struct _AFfilesetup *) 0)
#define AF_NULL_FILEHANDLE      ((struct _AFfilehandle *) 0)

enum
{
        AF_DEFAULT_TRACK = 1001
};

enum
{
        AF_FILE_UNKNOWN = -1,
        AF_FILE_RAWDATA = 0,
        AF_FILE_AIFFC = 1,
        AF_FILE_AIFF = 2,
        AF_FILE_NEXTSND = 3,
        AF_FILE_WAVE = 4,
        AF_FILE_BICSF = 5,
        AF_FILE_IRCAM = AF_FILE_BICSF
};

enum
{
        AF_SAMPFMT_TWOSCOMP = 401, /* linear two's complement */
        AF_SAMPFMT_UNSIGNED = 402, /* unsigned integer */
        AF_SAMPFMT_FLOAT = 403, /* 32-bit IEEE floating-point */
        AF_SAMPFMT_DOUBLE = 404 /* 64-bit IEEE double-precision floating-point */
};

enum
{
        AF_BAD_NOT_IMPLEMENTED = 0,     /* not implemented yet */
        AF_BAD_FILEHANDLE = 1,  /* tried to use invalid filehandle */
        AF_BAD_OPEN = 3,        /* unix open failed */
        AF_BAD_CLOSE = 4,       /* unix close failed */
        AF_BAD_READ = 5,        /* unix read failed */
        AF_BAD_WRITE = 6,       /* unix write failed */
        AF_BAD_LSEEK = 7,       /* unix lseek failed */
        AF_BAD_NO_FILEHANDLE = 8,       /* failed to allocate a filehandle struct */
        AF_BAD_ACCMODE = 10,    /* unrecognized audio file access mode */
        AF_BAD_NOWRITEACC = 11, /* file not open for writing */
        AF_BAD_NOREADACC = 12,  /* file not open for reading */
        AF_BAD_FILEFMT = 13,    /* unrecognized audio file format */
        AF_BAD_RATE = 14,       /* invalid sample rate */
        AF_BAD_CHANNELS = 15,   /* invalid number of channels*/
        AF_BAD_SAMPCNT = 16,    /* invalid sample count */
        AF_BAD_WIDTH = 17,      /* invalid sample width */
        AF_BAD_SEEKMODE = 18,   /* invalid seek mode */
        AF_BAD_NO_LOOPDATA = 19,        /* failed to allocate loop struct */
        AF_BAD_MALLOC = 20,     /* malloc failed somewhere */
        AF_BAD_LOOPID = 21,
        AF_BAD_SAMPFMT = 22,    /* bad sample format */
        AF_BAD_FILESETUP = 23,  /* bad file setup structure*/
        AF_BAD_TRACKID = 24,    /* no track corresponding to id */
        AF_BAD_NUMTRACKS = 25,  /* wrong number of tracks for file format */
        AF_BAD_NO_FILESETUP = 26,       /* failed to allocate a filesetup struct*/
        AF_BAD_LOOPMODE = 27,   /* unrecognized loop mode value */
        AF_BAD_INSTID = 28,     /* invalid instrument id */
        AF_BAD_NUMLOOPS = 29,   /* bad number of loops */
        AF_BAD_NUMMARKS = 30,   /* bad number of markers */
        AF_BAD_MARKID = 31,     /* bad marker id */
        AF_BAD_MARKPOS = 32,    /* invalid marker position value */
        AF_BAD_NUMINSTS = 33,   /* invalid number of instruments */
        AF_BAD_NOAESDATA = 34,
        AF_BAD_MISCID = 35,
        AF_BAD_NUMMISC = 36,
        AF_BAD_MISCSIZE = 37,
        AF_BAD_MISCTYPE = 38,
        AF_BAD_MISCSEEK = 39,
        AF_BAD_STRLEN = 40,     /* invalid string length */
        AF_BAD_RATECONV = 45,
        AF_BAD_SYNCFILE = 46,
        AF_BAD_CODEC_CONFIG = 47,       /* improperly configured codec */
        AF_BAD_CODEC_STATE = 48,        /* invalid codec state: can't recover */
        AF_BAD_CODEC_LICENSE = 49,      /* no license available for codec */
        AF_BAD_CODEC_TYPE = 50, /* unsupported codec type */
        AF_BAD_COMPRESSION = AF_BAD_CODEC_CONFIG,       /* for back compat */
        AF_BAD_COMPTYPE = AF_BAD_CODEC_TYPE,    /* for back compat */

        AF_BAD_INSTPTYPE = 51,  /* invalid instrument parameter type */
        AF_BAD_INSTPID = 52,    /* invalid instrument parameter id */
        AF_BAD_BYTEORDER = 53,
        AF_BAD_FILEFMT_PARAM = 54,      /* unrecognized file format parameter */
        AF_BAD_COMP_PARAM = 55, /* unrecognized compression parameter */
        AF_BAD_DATAOFFSET = 56, /* bad data offset */
        AF_BAD_FRAMECNT = 57,   /* bad frame count */
        AF_BAD_QUERYTYPE = 58,  /* bad query type */
        AF_BAD_QUERY = 59,      /* bad argument to afQuery() */
        AF_WARNING_CODEC_RATE = 60,     /* using 8k instead of codec rate 8012 */
        AF_WARNING_RATECVT = 61,        /* warning about rate conversion used */
        AF_BAD_HEADER = 62,     /* failed to parse header */
        AF_BAD_FRAME = 63,      /* bad frame number */
        AF_BAD_LOOPCOUNT = 64,  /* bad loop count */
        AF_BAD_DMEDIA_CALL = 65,        /* error in dmedia subsystem call */

        /* AIFF/AIFF-C specific errors when parsing file header */
        AF_BAD_AIFF_HEADER = 108,       /* failed to parse chunk header */
        AF_BAD_AIFF_FORM = 109, /* failed to parse FORM chunk */
        AF_BAD_AIFF_SSND = 110, /* failed to parse SSND chunk */
        AF_BAD_AIFF_CHUNKID = 111,      /* unrecognized AIFF/AIFF-C chunk id */
        AF_BAD_AIFF_COMM = 112, /* failed to parse COMM chunk */
        AF_BAD_AIFF_INST = 113, /* failed to parse INST chunk */
        AF_BAD_AIFF_MARK = 114, /* failed to parse MARK chunk */
        AF_BAD_AIFF_SKIP = 115, /* failed to skip unsupported chunk */
        AF_BAD_AIFF_LOOPMODE = 116      /* unrecognized loop mode (forw, etc)*/
};

/* basic operations on file handles and file setups */
EXTERN AFfilesetup (*afNewFileSetup) (void);
EXTERN void (*afFreeFileSetup) (AFfilesetup);

EXTERN AFfilehandle (*afOpenFile) (const char *filename, const char *mode,
  AFfilesetup setup);
EXTERN AFfilehandle (*afOpenFD) (int fd, const char *mode, AFfilesetup setup);
EXTERN int (*afCloseFile) (AFfilehandle file);
EXTERN void (*afInitFileFormat) (AFfilesetup, int format);
EXTERN int (*afReadFrames) (AFfilehandle, int track, void *buffer, int frameCount);
EXTERN int (*afWriteFrames) (AFfilehandle, int track, const void *buffer, int frameCount);
EXTERN AFframecount (*afSeekFrame) (AFfilehandle, int track, AFframecount frameoffset);
EXTERN AFfileoffset (*afTellFrame) (AFfilehandle, int track);
EXTERN float (*afGetFrameSize) (AFfilehandle file, int track, int expand3to4);
EXTERN float (*afGetVirtualFrameSize) (AFfilehandle, int track, int expand3to4);
EXTERN void (*afInitChannels) (AFfilesetup, int track, int nchannels);
EXTERN int (*afGetChannels) (AFfilehandle, int track);
EXTERN int (*afSetVirtualChannels) (AFfilehandle, int track, int channelCount);
EXTERN void (*afInitSampleFormat) (AFfilesetup, int track, int sampleFormat,
        int sampleWidth);
EXTERN void (*afGetSampleFormat) (AFfilehandle file, int track, int *sampfmt,
        int *sampwidth);
EXTERN int (*afSetVirtualSampleFormat) (AFfilehandle, int track,
        int sampleFormat, int sampleWidth);
EXTERN void (*afInitRate) (AFfilesetup, int track, double rate);
EXTERN double (*afGetRate) (AFfilehandle, int track);
EXTERN AFframecount (*afGetFrameCount) (AFfilehandle file, int track);
