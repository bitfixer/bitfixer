/* Audiotap shared library: a higher-level interface to TAP shared library
 *
 * libaudiotap_external_symbols.c: load optional libraries, and relative symbols.
 * This allows Audiotap tp work even when the needed libraries are not there,
 * at least as a TAP reader/writer.
 *
 * Copyright (c) Fabrizio Gennari, 2012
 *
 * The program is distributed under the GNU Lesser General Public License.
 * See file LESSER-LICENSE.TXT for details.
 */

#define AUDIOFILE_DECLARE_HERE
#include "audiofile.h"
#define PORTAUDIO_DECLARE_HERE
#include "portaudio.h"
#define TAPENCODER_DECLARE_HERE
#include "tapencoder.h"
#define TAPDECODER_DECLARE_HERE
#include "tapdecoder.h"
#include "audiotap.h"

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#if __GNUC__ >= 4
 __attribute__ ((visibility ("hidden")))
#endif
struct audiotap_init_status status = {
  LIBRARY_UNINIT,
  LIBRARY_UNINIT,
  LIBRARY_UNINIT,
  LIBRARY_UNINIT
};

static enum library_status audiofile_init(){
#if defined(WIN32)
  HINSTANCE handle;
#else
  void *handle;
#endif

#if defined(WIN32)
#define LOAD_FROM_LIBRARY(x,y) (void*)GetProcAddress(x,y)
#else
#define LOAD_FROM_LIBRARY(x,y) dlsym(x,y)
#endif

#define LOAD(x) \
  x=LOAD_FROM_LIBRARY(handle, #x); \
  if (!x) { \
    return LIBRARY_SYMBOLS_MISSING; \
  }

static const char* audiofile_library_name =
#if (defined _MSC_VER)
#ifdef _DEBUG
"audiofiled.dll"
#else //_MSC_VER and not DEBUG
"audiofile.dll"
#endif //_MSC_VER,DEBUG
#elif (defined _WIN32 || defined __CYGWIN__)
#ifdef AUDIOFILE_COMPILED_WITH_CYGWIN_SHELL
"cygaudiofile-1.dll"
#else //not AUDIOFILE_COMPILED_WITH_CYGWIN_SHELL
"libaudiofile-1.dll"
#endif //_WIN32 or __CYGWIN__,AUDIOFILE_COMPILED_WITH_CYGWIN_SHELL
#elif defined __APPLE__
"libaudiofile.1.dylib"
#else //not _MSC_VER, not _WIN32, not __CYGWIN__
"libaudiofile.so.0"
#endif//_MSC_VER,_WIN32 or __CYGWIN__
;

#if defined(WIN32)
  handle=LoadLibraryA(audiofile_library_name);
#else
  handle=dlopen(audiofile_library_name, RTLD_LAZY);
#endif
  if (!handle) {
    return LIBRARY_MISSING;
  }

  LOAD(afOpenFile)
  LOAD(afOpenFD)
  LOAD(afCloseFile)
  LOAD(afReadFrames)
  LOAD(afWriteFrames)
  LOAD(afSeekFrame)
  LOAD(afSetVirtualChannels)
  LOAD(afGetSampleFormat)
  LOAD(afSetVirtualSampleFormat)
  LOAD(afGetVirtualFrameSize)
  LOAD(afGetFrameCount)
  LOAD(afTellFrame)
  LOAD(afInitFileFormat)
  LOAD(afInitSampleFormat)
  LOAD(afInitChannels)
  LOAD(afInitRate)
  LOAD(afGetRate)
  LOAD(afNewFileSetup)
  LOAD(afFreeFileSetup)

  return LIBRARY_OK;
}

static enum library_status portaudio_init(){
  void *handle;

  static const char* portaudio_library_name =
#if (defined _MSC_VER)
    "portaudio_x86.dll"
#elif (defined _WIN32 || defined __CYGWIN__) //not _MSC_VER
#ifdef PORTAUDIO_COMPILED_WITH_CYGWIN_SHELL
    "cygportaudio-2.dll"
#else //not PORTAUDIO_COMPILED_WITH_CYGWIN_SHELL
    "libportaudio-2.dll"
#endif //_WIN32 or __CYGWIN__,AUDIOFILE_COMPILED_WITH_CYGWIN_SHELL
#elif defined __APPLE__
    "libportaudio.2.dylib"
#else //not _MSC_VER, not _WIN32, not __CYGWIN__
    "libportaudio.so.2"
#endif//_MSC_VER,_WIN32 or __CYGWIN__
    ;


#if defined(WIN32)
  handle=LoadLibraryA(portaudio_library_name);
#else
  handle=dlopen(portaudio_library_name, RTLD_LAZY);
#endif
  if (!handle)
    return LIBRARY_MISSING;
  LOAD(Pa_Initialize)
  LOAD(Pa_Terminate)
  LOAD(Pa_OpenDefaultStream)
  LOAD(Pa_CloseStream)
  LOAD(Pa_StartStream)
  LOAD(Pa_StopStream)
  LOAD(Pa_ReadStream)
  LOAD(Pa_WriteStream)

  if (Pa_Initialize() != paNoError)
  {
    return LIBRARY_INIT_FAILED;
  }

  return LIBRARY_OK;
}

static enum library_status libtapencoder_init(){
#if defined(WIN32)
  HMODULE
#else
  void *
#endif
  handle;

  static const char* tapencoder_library_name =
#if (defined _WIN32 || defined __CYGWIN__) 
    "tapencoder.dll"
#else
    "libtapencoder.so"
#endif//__WIN32 or __CYGWIN__
    ;


#if defined(WIN32)
  handle=LoadLibraryA(tapencoder_library_name);
#else
  handle=dlopen(tapencoder_library_name, RTLD_LAZY);
#endif
  if (!handle)
    return LIBRARY_MISSING;

  LOAD(tapenc_init2)
  LOAD(tapenc_exit)
  LOAD(tapenc_get_pulse)
  LOAD(tapenc_flush)
  LOAD(tapenc_get_max)
  LOAD(tapenc_invert)
  LOAD(tapenc_toggle_trigger_on_both_edges)
  LOAD(tapenc_set_silence_threshold)

  return LIBRARY_OK;
}

static enum library_status libtapdecoder_init(){
#if defined(WIN32)
  HMODULE
#else
  void *
#endif
  handle;

  static const char* tapdecoder_library_name =
#if (defined _WIN32 || defined __CYGWIN__) 
    "tapdecoder.dll"
#else
    "libtapdecoder.so"
#endif//__WIN32 or __CYGWIN__
    ;

  handle=
#if defined(WIN32)
  LoadLibraryA(tapdecoder_library_name);
#else
  dlopen(tapdecoder_library_name, RTLD_LAZY);

#endif
  if (!handle)
    return LIBRARY_MISSING;

  LOAD(tapdec_init2)
  LOAD(tapdec_exit)
  LOAD(tapdec_set_pulse)
  LOAD(tapdec_get_buffer)
  LOAD(tapdec_enable_halfwaves)
  LOAD(tapdec_enable_halfwaves)

  return LIBRARY_OK;
}

struct audiotap_init_status audiotap_initialize2(void){
  status.audiofile_init_status = audiofile_init();
  status.portaudio_init_status = portaudio_init();
  status.tapencoder_init_status = libtapencoder_init();
  status.tapdecoder_init_status = libtapdecoder_init();

  return status;
}

void audiotap_terminate_lib(void)
{
  if (status.portaudio_init_status == LIBRARY_OK)
    Pa_Terminate();
}

