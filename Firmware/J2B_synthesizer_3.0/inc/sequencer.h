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

#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__


#define SEQ_LEN 16

void Seq_Let_Pattern(byte inst, int beat, boolean newval);
boolean Seq_Get_Pattern(byte inst, int beat);
void Seq_Let_PatternRaw(byte inst, uint16_t pat);
uint16_t Seq_Get_PatternRaw(byte inst);
void Seq_Let_Volume(byte inst, byte newval);
byte Seq_Get_Volume(byte inst);
void Seq_Let_DelayBeats(byte inst, byte newval);
byte Seq_Get_DelayBeats(byte inst);
void Seq_Let_Amplitude(byte inst, byte newval);
byte Seq_Get_Amplitude(byte inst);
void Seq_Let_Playing(boolean newval, boolean cont);
byte Seq_Get_Playing(void);
void Seq_Let_Tempo(byte newtempo);
byte Seq_Get_Tempo(void);
void Seq_Clear(void);
void Seq_Process(void);
inline void Seq_Let_Current_Position(byte pos);
inline byte Seq_Get_Current_Position(void);


#endif /* __SEQUENCER_H__ */
