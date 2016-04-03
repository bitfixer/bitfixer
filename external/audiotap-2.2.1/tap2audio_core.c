/* Audiotap: a program for playing TAP files or converting them to WAV files,
 * and to convert C64 tapes to TAP files
 *
 * Copyright (c) Fabrizio Gennari, 2003
 *
 * The program is distributed under the GNU General Public License.
 * See file LICENSE.TXT for details.
 *
 * tap2audio_core.c : main tap->audio processing
 * 
 * This file belongs to the tap->audio part
 * This file is part of Audiotap core processing files
 */

#include "audiotap_loop.h"
#include "audiotap.h"
#include "audiotap_callback.h"
#include "tap2audio_core.h"
#include <stddef.h>
#include <string.h>

void tap2audio(const char *infile,
               const char *outfile,
               char output_to_audio,
               struct tapdec_params *params,
               uint32_t freq)
{
  uint8_t machine, videotype, halfwaves;
  struct audiotap *audiotap_in, *audiotap_out;

  if (audio2tap_open_from_file3(&audiotap_in,
                               infile,
                               NULL,
                               &machine,
                               &videotype,
                               &halfwaves) != AUDIOTAP_OK){
    error_message("File %s cannot be opened for reading", infile);
    return;
  }
  if (!output_to_audio){
    if (tap2audio_open_to_wavfile4(&audiotap_out, outfile, params, freq, machine, videotype) != AUDIOTAP_OK){
      error_message("File %s cannot be opened", infile);
      audiotap_out = NULL;
    }
  }
  
  if(audiotap_out != NULL){
    if (halfwaves){
      tap2audio_enable_halfwaves(audiotap_out, 1);
      audio2tap_enable_disable_halfwaves(audiotap_in, 1);
    }

    audiotap_loop(audiotap_in, audiotap_out, audiotap_out, NULL);
    tap2audio_close(audiotap_out);
  }
  audio2tap_close(audiotap_in);
}

