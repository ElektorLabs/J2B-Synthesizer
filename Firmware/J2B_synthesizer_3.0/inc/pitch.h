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

#ifndef __PITCH_H__
#define __PITCH_H__


//These constants are used to define the MAXIMUM amount the env can effect the pitch.
//The default max for env is: 
//********multiplier = 4***********
#define PITCH_ENVMAX 1.386294361119891f  //ln(multiplier)
#define PITCH_ENVMULT 64                 //multipler used for calculating lookup table  (256 / multiplier)  
#define PITCH_ENVBS 6UL                  //bitshift amount when multiplying pitch by pitch env amt.  log2(PITCH_MULT)
#define PITCH_LFOBS 7UL                  //bitshift amount when multiplying pitch by pitch lfo amt.  log2(PITCH_MULT)

//These constants are used to define the MAXIMUM amount the lfo can effect the pitch.
//The default max for lfo is: 
//********multiplier = 2 (1 octave)***********
#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
#define PITCH_LFOMAX 0.693147180559945f  //ln(2)
#define PITCH_LFOMULT  (128)                //multipler used for calculating lookup table  (256 / multiplier)
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
#define PITCH_LFOMAX 1.386294361119891f  //ln(4)
#define PITCH_LFOMULT  (64)                //multipler used for calculating lookup table  (256 / multiplier)
#endif /* PRODUCT */

void Pitch_Let_NextFreq(unsigned int newfreq);
unsigned int Pitch_Get_NextFreq(void);
void Pitch_Let_Porta(int newporta);
int Pitch_Get_Porta(void);
void Pitch_ResetPorta(void);
void Pitch_RefreshPortaTicks(void);
void Pitch_Let_PropPorta(boolean newprop);
boolean Pitch_Get_PropPorta(void);
void Pitch_Let_FenvAmt(byte newamt);
byte Pitch_Get_FenvAmt(void);
byte Pitch_Get_FenvGain(void);
inline unsigned long Pitch_Get_FreqCalc(void);
boolean Pitch_Get_PitchChanged(void);
void Pitch_Process(void);
void Pitch_Let_LFOAmt(byte newamt);
byte Pitch_Get_LFOAmt(void);
byte Pitch_Get_LFOGain(void);

#if PRODUCT==ATMEGADRUM
	void Pitch_Let_CurFreq(unsigned int newfreq);
	unsigned int Pitch_Get_CurFreq(void);
#endif

#if PRODUCT!=DELAYERTRON
	byte Pitch_Get_MIDIPbendGain(void);
#endif /* PRODUCT */


#endif // __PITCH_H__
