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

#include <stdint.h>

#if !defined TAPDECODER_DECLARE_HERE
#define EXTERN extern
#elif __GNUC__ >= 4
#define EXTERN __attribute__ ((visibility ("hidden")))
#else
#define EXTERN
#endif

struct tap_dec_t;
enum tapdec_waveform {
  TAPDEC_TRIANGLE,
  TAPDEC_SQUARE,
  TAPDEC_SINE
};

EXTERN struct tap_dec_t *(*tapdec_init2)(uint32_t volume, uint8_t inverted, enum tapdec_waveform waveform);
EXTERN void (*tapdec_set_pulse)(struct tap_dec_t *tap, uint32_t pulse);
EXTERN uint32_t (*tapdec_get_buffer)(struct tap_dec_t *tap, int32_t *buffer, unsigned int buflen);
EXTERN void (*tapdec_enable_halfwaves)(struct tap_dec_t *tap, uint8_t halfwaves);
EXTERN void (*tapdec_exit)(struct tap_dec_t *tap);
