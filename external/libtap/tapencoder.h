/* TAP shared library: a library for converting audio data (as a stream of
 * PCM signed 32-bit samples, mono) to raw TAP data and back
 *
 * TAP specification was written by Per Håkan Sundell and is available at
 * http://www.computerbrains.com/tapformat.html
 *
 * The algorithm for TAP encoding was originally written by Janne Veli
 * Kujala, based on the one written by Andreas Matthies for Tape64.
 * Some modifications and adaptation to library format by Fabrizio Gennari
 *
 * Copyright (c) Fabrizio Gennari, 2003-2012
 *
 * The program is distributed under the GNU Lesser General Public License.
 * See file LESSER-LICENSE.TXT for details.
 */

#include <stdint.h>

struct tap_enc_t;

struct tap_enc_t *tapenc_init2(uint32_t min_duration, uint8_t sensitivity, uint8_t initial_threshold, uint8_t inverted);
uint32_t tapenc_get_pulse(struct tap_enc_t *tap, int32_t *buffer, uint32_t buflen, uint32_t *pulse);
uint32_t tapenc_flush(struct tap_enc_t *tap);
int32_t tapenc_get_max(struct tap_enc_t *tap);
uint32_t tapenc_get_last_trigger(struct tap_enc_t *tap);
void tapenc_invert(struct tap_enc_t *tap);
void tapenc_toggle_trigger_on_both_edges(struct tap_enc_t *tap, uint8_t both_edges);
void tapenc_set_silence_threshold(struct tap_enc_t *tap,
                                  uint8_t silence_threshold,
                                  uint32_t min_non_silence_duration);
void tapenc_exit(struct tap_enc_t *tap);
