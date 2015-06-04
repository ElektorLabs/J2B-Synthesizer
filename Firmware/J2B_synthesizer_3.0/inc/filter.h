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

//***15 Biquad filter algorithms***

#ifndef __FILTER_H__
#define __FILTER_H__


//the filter types
#define FILT_OFF  (0)
#define FILT_LPF  (1)
#define FILT_HPF  (2)
#define FILT_BPF  (3)
#define FILT_NOTCH  (4)
#define FILT_PEAK10  (5)
#define FILT_PEAK30  (6)
#define FILT_PEAK100  (7)
#define FILT_LS10  (8)
#define FILT_LS30  (9)
#define FILT_HS10  (10)
#define FILT_HS30  (11)
#define FILT_BUTLPF  (12)
#define FILT_BUTHPF  (13)
#define FILT_BESLPF  (14)
#define FILT_BESHPF  (15)

//These constants are used to define the MAXIMUM amount the env and lfo can effect the filter cutoff.
//The default max (for both env and lfo) is: 
//********multiplier = 4***********
#define FILT_MAX 1.386294361119891f  //ln(multiplier)
#define FILT_MULT 64                 //multipler used for calculating lookup table  (256 / multiplier)  
#define FILT_BS 6UL                  //bitshift amount when multiplying fc by filt lfo amt.  log2(FILT_MULT)

#define MINQ 0.5f                  //minimum value of Q (default = 0.5)
#define MULTQ 0.076470588235294f   //multiplier to convert from filt_q(0-255) to real Q (0.5-20) = (20 - 0.5)/255;


void Filt_Let_Fc(byte newfc);
byte Filt_Get_Fc(void);
void Filt_Let_Q(byte newq);
byte Filt_Get_Q(void);
inline void Filt_Let_Type(byte newtype);
byte Filt_Get_Type(void);
#if PRODUCT==DELAYERTRON
	int Filter_Process(int samp_in);
#else
	void Filt_Let_GainAdj(boolean newadj);
	boolean Filt_Get_GainAdj(void);
	void Filt_CalcVals(void);
	void LPValCalculator(void);
	void HPValCalculator(void);
	void BPSkirtValCalculator(void);
	void NotchValCalculator(void);
	void PeakingEQValCalculator(void);
	void LowShelfValCalculator(void);
	void HighShelfValCalculator(void);
	void ButterworthLPCalculator(void);
	void ButterworthHPCalculator(void);
	void BesselLPCalculator(void);
	void BesselHPCalculator(void);
	void Filt_Process(void);
	float Biquad_process(float bi0);
	void Filt_Let_FenvAmt(byte newamt);
	byte Filt_Get_FenvAmt(void);
	byte Filt_Get_FenvGain(void);
#endif
void Filt_Let_LFOAmt(byte newamt);
byte Filt_Get_LFOAmt(void);
byte Filt_Get_LFOGain(void);

#if PRODUCT==DELAYERTRON
void Filt_Let_PostDelay(boolean newpd);
inline boolean Filt_Get_PostDelay(void);
void Filt_CalcFc(void);
#endif


#endif // __FILTER_H__
