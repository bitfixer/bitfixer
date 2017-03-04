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

struct tap_dec_t;
enum tapdec_waveform {
  TAPDEC_TRIANGLE,
  TAPDEC_SQUARE,
  TAPDEC_SINE
};

struct tap_dec_t *tapdec_init2(uint8_t volume, uint8_t inverted, enum tapdec_waveform waveform);
void tapdec_set_pulse(struct tap_dec_t *tap, uint32_t pulse);
uint32_t tapdec_get_buffer(struct tap_dec_t *tap, int32_t *buffer, unsigned int buflen);
void tapdec_enable_halfwaves(struct tap_dec_t *tap, uint8_t halfwaves);
void tapdec_exit(struct tap_dec_t *tap);
