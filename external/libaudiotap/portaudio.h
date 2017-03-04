/* Audiotap shared library: a higher-level interface to TAP shared library
 * 
 * Header file for portaudio library, modified by replacing prototypes with
 * pointers to functions, so functions can be loaded with dlsym/GetProcAddress.
 *
 * Original files (C) 1999-2002, Ross Bencina and Phil Burk
 * This file (C) Fabrizio Gennari, 2003-2008
 *
 * The program is distributed under the GNU Lesser General Public License.
 * See file LESSER-LICENSE.TXT for details.
 */

#if !defined PORTAUDIO_DECLARE_HERE
#define EXTERN extern
#elif __GNUC__ >= 4
#define EXTERN __attribute__ ((visibility ("hidden")))
#else
#define EXTERN
#endif

typedef int PaError;
typedef unsigned long PaSampleFormat;
#define paInt32          ((PaSampleFormat) 0x00000002) /**< @see PaSampleFormat */

typedef enum PaErrorCode
{
    paNoError = 0,

    paNotInitialized = -10000,
    paUnanticipatedHostError,
    paInvalidChannelCount,
    paInvalidSampleRate,
    paInvalidDevice,
    paInvalidFlag,
    paSampleFormatNotSupported,
    paBadIODeviceCombination,
    paInsufficientMemory,
    paBufferTooBig,
    paBufferTooSmall,
    paNullCallback,
    paBadStreamPtr,
    paTimedOut,
    paInternalError,
    paDeviceUnavailable,
    paIncompatibleHostApiSpecificStreamInfo,
    paStreamIsStopped,
    paStreamIsNotStopped,
    paInputOverflowed,
    paOutputUnderflowed,
    paHostApiNotFound,
    paInvalidHostApi,
    paCanNotReadFromACallbackStream,      /**< @todo review error code name */
    paCanNotWriteToACallbackStream,       /**< @todo review error code name */
    paCanNotReadFromAnOutputOnlyStream,   /**< @todo review error code name */
    paCanNotWriteToAnInputOnlyStream,     /**< @todo review error code name */
    paIncompatibleStreamHostApi,
    paBadBufferPtr
} PaErrorCode;

EXTERN PaError (*Pa_Initialize)( void );
EXTERN PaError (*Pa_Terminate)( void );

typedef void PaStream;
EXTERN PaError (*Pa_OpenDefaultStream)( PaStream** stream,
                             int numInputChannels,
                             int numOutputChannels,
                             PaSampleFormat sampleFormat,
                             double sampleRate,
                             unsigned long framesPerBuffer,
                             void *streamCallback,
                             void *userData );
EXTERN PaError (*Pa_CloseStream)( PaStream *stream );
EXTERN PaError (*Pa_StartStream)( PaStream *stream );
EXTERN PaError (*Pa_StopStream)( PaStream *stream );
EXTERN PaError (*Pa_ReadStream)( PaStream* stream,
                      void *buffer,
                      unsigned long frames );
EXTERN PaError (*Pa_WriteStream)( PaStream* stream,
                       const void *buffer,
                       unsigned long frames );
