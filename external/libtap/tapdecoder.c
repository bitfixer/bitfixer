/* TAP shared library: a library for converting audio data (as a stream of
 * PCM signed 32-bit samples, mono) to raw TAP data and back
 *
 * TAP specification was written by Per Håkan Sundell and is available at
 * http://www.computerbrains.com/tapformat.html
 *
 * The algorithm to decode TAP data into square, sine or triangular waves
 * is by Fabrizio Gennari.
 *
 * Copyright (c) Fabrizio Gennari, 2003-2012
 *
 * The program is distributed under the GNU Lesser General Public License.
 * See file LESSER-LICENSE.TXT for details.
 */

#include <stdlib.h>
#include <sys/types.h>
/* When using GNU libc, math.h requires linking
   with the additional library libm. Therefore,
   HAVE_SINE_WAVE is disabled by default,
   but it can be activated by setting a
   Makefile variable */
#if (!defined __GNU_LIBRARY__ && (!defined _MSC_VER || _MSC_VER > 1200))
#define HAVE_SINE_WAVE
#endif

#ifdef HAVE_SINE_WAVE
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif /* _MSC_VER */
#include <math.h>
#endif /*HAVE_SINE_WAVE*/
#include "tapdecoder.h"


struct tap_dec_t{
  uint8_t halfwaves;
  uint32_t first_consumed, second_consumed, first_semiwave, second_semiwave, volume;
  unsigned char negative;
  int32_t (*get_val)(uint32_t to_be_consumed, uint32_t this_pulse_len, uint32_t volume);
};

static int32_t tap_get_triangle_val(uint32_t to_be_consumed, uint32_t this_pulse_len, uint32_t volume){
  int64_t in;

  to_be_consumed = (to_be_consumed < this_pulse_len / 2) ?
    to_be_consumed + 1 :
    this_pulse_len - 1 - to_be_consumed;
  in = ((int64_t)to_be_consumed)*volume*2;
  return (int32_t)(in / this_pulse_len);
}

#ifdef HAVE_SINE_WAVE
static int32_t tap_get_sinewave_val(uint32_t to_be_consumed, uint32_t this_pulse_len, uint32_t volume){
  double angle=(M_PI*(to_be_consumed+1))/this_pulse_len;
  return (int32_t)(volume*sin(angle));
}
#endif

static int32_t tap_get_squarewave_val(uint32_t this_pulse_len, uint32_t to_be_consumed, uint32_t volume){
  return volume;
}

static uint32_t tap_semiwave(int32_t **buffer
                            ,uint32_t *buflen
                            ,uint32_t semiwave_len
                            ,uint32_t *consumed
                            ,uint32_t volume
                            ,int32_t (*get_val_func)(uint32_t, uint32_t, uint32_t)
                            ,unsigned char *negative
                            ){
  uint32_t samples_done = 0;

  for(;*buflen > 0 && *consumed < semiwave_len; (*buffer)++, (*buflen)--, (*consumed)++, samples_done++){
    **buffer = get_val_func(*consumed, semiwave_len, volume);
    if (*negative)
      **buffer = ~(**buffer);
  }

  if (*consumed == semiwave_len && samples_done != 0)
    *negative = !(*negative);

  return samples_done;
}  

struct tap_dec_t *tapdec_init2(uint8_t volume, uint8_t inverted, enum tapdec_waveform waveform){
  struct tap_dec_t *tap;

  tap = (struct tap_dec_t *)malloc(sizeof(struct tap_dec_t));
  if (tap==NULL)
    return NULL;
  tap->halfwaves = 0;
  tap->volume=volume<<23;
  tap->negative=inverted;
  switch (waveform){
  case TAPDEC_TRIANGLE:
  default:
    tap->get_val = tap_get_triangle_val;
    break;
  case TAPDEC_SQUARE:
    tap->get_val = tap_get_squarewave_val;
    break;
#ifdef HAVE_SINE_WAVE
  case TAPDEC_SINE:
    tap->get_val = tap_get_sinewave_val;
    break;
#endif
  }
  
  return tap;
}

void tapdec_set_pulse(struct tap_dec_t *tap, uint32_t pulse){
  tap->first_consumed=tap->second_consumed=0;
  if (pulse==1 && !tap->halfwaves)
    pulse=0;
  tap->second_semiwave = tap->halfwaves ? 0 : pulse / 2;
  tap->first_semiwave=pulse - tap->second_semiwave;
}

uint32_t tapdec_get_buffer(struct tap_dec_t *tap, int32_t *buffer, uint32_t buflen){
  uint32_t samples_done_first = tap_semiwave(&buffer, &buflen, tap->first_semiwave, &tap->first_consumed, tap->volume, tap->get_val, &tap->negative);
  uint32_t samples_done_second = tap_semiwave(&buffer, &buflen, tap->second_semiwave, &tap->second_consumed, tap->volume, tap->get_val, &tap->negative);

  return samples_done_first + samples_done_second;
}

void tapdec_enable_halfwaves(struct tap_dec_t *tap, uint8_t halfwaves){
  tap->halfwaves = halfwaves;
}

void tapdec_exit(struct tap_dec_t *tap)
{
  free(tap);
}
