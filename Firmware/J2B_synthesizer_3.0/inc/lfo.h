/*
Copyright 2013 Paul Soulsby www.soulsbysynths.com
    This file is part of Atmegatron.

    Atmegatron is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Atmegatron is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Atmegatron.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __LFO_H__
#define __LFO_H__


#define LFO_LEN 64

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
// Bipolar LFO. 16 waveforms, each 64 samples long.
extern const signed char lfo_store[16][LFO_LEN];
void LFO_Let_Type(byte newtable);
byte LFO_Get_Type(void);
void LFO_Let_Invert(boolean newinv);
boolean LFO_Get_Invert(void);
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
// Bipolar LFO. 1 waveform, 64 samples long.
extern const signed char lfo_store[LFO_LEN];
#endif /* PRODUCT */

void LFO_Let_Speed(unsigned int newspeed);
unsigned int LFO_Get_Speed(void);
signed char LFO_Get_Level(void);
void LFO_Reset(void);
void LFO_CalcVal(void);


#endif // __LFO_H__
