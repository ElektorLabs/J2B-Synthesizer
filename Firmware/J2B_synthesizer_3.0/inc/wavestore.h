/*
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WAVESTORE_H__
#define __WAVESTORE_H__


#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
extern const sample_t wave_store[2][16][WAVE_LEN];
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
inline sample_t Wavestore_Get_Sample(uint16_t index);
inline uint16_t Wavestore_Get_Start(byte index);
#endif /* PRODUCT */

#if PRODUCT==DELAYERTRON
extern const sample_t wave_store[2][16][WAVE_LEN];
extern const uint8_t MIDI_mult[WAVE_LEN/2];
extern const uint8_t MIDI_bs[WAVE_LEN/2];
#endif /* PRODUCT */


#endif /* __WAVESTORE_H__ */
