///*
//Copyright 2014 Paul Soulsby www.soulsbysynths.com
    //This file is part of Atmegatron.
//
    //Atmegatron is free software: you can redistribute it and/or modify
    //it under the terms of the GNU General Public License as published by
    //the Free Software Foundation, either version 3 of the License, or
    //(at your option) any later version.
//
    //Atmegatron is distributed in the hope that it will be useful,
    //but WITHOUT ANY WARRANTY; without even the implied warranty of
    //MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    //GNU General Public License for more details.
//
    //You should have received a copy of the GNU General Public License
    //along with Atmegatron.  If not, see <http://www.gnu.org/licenses/>.
//*/

#include "atmegatron.h"

#if PRODUCT==DELAYERTRON

#define DELAY_LFOMAX 0.693147180559945f  //ln(multiplier)
#define DELAY_LFOMULT 128                //multipler used for calculating lookup table  (256 / multiplier)
#define DELAY_LFOBS 7UL                  //bitshift amount when multiplying pitch by pitch lfo amt.  log2(PITCH_MULT)
#define DELAY_ENVMAX 1.386294361119891f  //ln(multiplier)
#define DELAY_ENVMULT 64                 //multipler used for calculating lookup table  (256 / multiplier)
#define DELAY_ENVBS 6UL                  //bitshift amount when multiplying pitch by pitch env amt.  log2(PITCH_MULT)

volatile int delay_writepos = DELAY_SIZE-1;
volatile int delay_readpos = 0;
volatile sample_t delay_buffer[DELAY_SIZE];
volatile byte delay_fbmsb = 8;
volatile byte delay_fblsb = 8;

const byte FB_RShiftMSB[16] = {8,7,6,4,4,3,2,2,2,1,1,1,0,0,0,0};
const byte FB_RShiftLSB[16] = {8,7,6,7,4,4,5,3,2,3,2,1,3,2,1,0};
// CPV - when master_wm equals 7 or 8, master_dm equals 8 or 7,
//       Mix_RShift[7] Mix_RShift[8] are both 0...
const byte Mix_RShift[16] = {8,5,4,3,2,1,1,0,0,0,0,0,0,0,0,0};
byte delay_pos = 0;
byte delay_fb = 0;
byte delay_drymix = 15;
byte delay_wetmix = 0;
byte delay_lfoamt = 0;
float delay_lfoamtf = 0;
byte delay_fenvamt = 0;
float delay_fenvamtf = 0;                    //lfo amount as float (0-1)
byte delay_sf = 50;
unsigned int delay_oc1a = 2047;
boolean delay_clipmode = false;

void Delay_Let_Position(byte newposition)
{                //sets the wavetable number
	int off;
	if (newposition!=delay_pos){                    //if new val is different:
		delay_pos = newposition;                      //set new num
		if (newposition==15){
			off = DELAY_SIZE - 1;
		}
		else{
			off = (DELAY_SIZE >> 4) * delay_pos;
			off++;
		}
		delay_readpos = delay_writepos - off;
		if (delay_readpos < 0) {                //if at end of wavetable, go back to start
			delay_readpos  = delay_readpos + DELAY_SIZE;
		}
	}
}

byte Delay_Get_Position(void)
{
	return delay_pos;
}

void Delay_Let_Feedback(byte newfb)
{                //sets the wavetable number
	delay_fb = newfb;
	delay_fbmsb = FB_RShiftMSB[delay_fb];
	delay_fblsb = FB_RShiftLSB[delay_fb];
}

inline byte Delay_Get_Feedback(void)
{
	return delay_fb;
}

void Delay_Let_WetMix(byte newwm)
{
	if (newwm!=delay_wetmix)
	{
		delay_wetmix = newwm;
		master_wm = Mix_RShift[delay_wetmix];
	}
}

byte Delay_Get_WetMix(void)
{
	return delay_wetmix;
}

void Delay_Let_DryMix(byte newdm)
{
	if (newdm!=delay_drymix)
	{
		delay_drymix = newdm;
		master_dm = Mix_RShift[delay_drymix];
	}
}

byte Delay_Get_DryMix(void)
{
	return delay_drymix;
}

void Delay_Let_LFOAmt(byte newamt)
{
	if (newamt!=delay_lfoamt){                                    //if new value different to current
		delay_lfoamt = newamt;                                      //set new value
		delay_lfoamtf = (float)delay_lfoamt * DELAY_LFOMAX / 255;
	}
}

byte Delay_Get_LFOAmt(void)
{
	return delay_lfoamt;
}

void Delay_Let_FenvAmt(byte newamt)
{
	if (newamt!=delay_fenvamt){                                  //if new value different to current
		delay_fenvamt = newamt;                                    //set new value
		delay_fenvamtf = (float)delay_fenvamt * DELAY_ENVMAX / 255;//calculate new float value used to calculate env lookup table (0-PITCH_ENVMAX)
	}
}

byte Delay_Get_FenvAmt(void)
{
	return delay_fenvamt;
}

byte Delay_Get_SF(void)
{
	return delay_sf;
}

void Delay_Let_SF(byte newamt)
{
	if (newamt!=delay_sf){                                    //if new value different to current
		delay_sf = newamt;                                      //set new value
	}
}

boolean Delay_Get_ClipMode(void)
{
	return delay_clipmode;
}

void Delay_Let_ClipMode(boolean newcm)
{
	if (newcm!=delay_clipmode){                                    //if new value different to current
		delay_clipmode = newcm;                                      //set new value
		if(delay_clipmode==true){
			master_clipbs = 0;
		}
		else{
			master_clipbs = 1;
		}
	}
}

void Delay_CalcSF(void)
{
	unsigned long newsf;
	newsf = delay_sf << 5;
	if (delay_lfoamt>0)
	{
		// Multiply pitch by lfo gain and bit shift.
		// This is the fixed point maths way of mult pitch by lfo (i.e. not using floating point)
		newsf = newsf * (Delay_Get_LFOGain() + 1) >> DELAY_LFOBS;
	}
	if (delay_fenvamt>0)
	{
		// Multiply pitch by lfo gain and bit shift.
		// This is the fixed point maths way of mult pitch by lfo (i.e. not using floating point)
		newsf = newsf * (Delay_Get_FenvGain() + 1) >> DELAY_ENVBS;
	}
	if (newsf < 1024)
	{
		newsf = 1024;
	}
	if (newsf > 8192)
	{
		newsf = 8192;
	}
	if (newsf != delay_oc1a)
	{
		//OCR1A = (unsigned int)newsf;
		master_ocr1 = (unsigned int)newsf;
		delay_oc1a = (unsigned int)newsf;
	}
}

byte Delay_Get_LFOGain(void)
{
	float f, g;
	f = (float)-LFO_Get_Level() / 127;                       //convert LFO output to float (0 - 1)
	g = round(exp(f * delay_lfoamtf) * DELAY_LFOMULT - 1);  //calculate lookup table.  e.g. lfoamt = 0: exp(0) * 64 - 1 = 63   e.g.  lfoamt = 1 and lfo level = 127: exp(ln(4)) * 64 - 1 = 255  e.g. lfoamt = 1 and lfo = -127: exp(-ln(4)) * 64 - 1 = 15
	return (byte)g;
}

byte Delay_Get_FenvGain(void)
{
	float f, g;
	f = (float)-Fenv_Get_Level() / 127;                         //convert env output to float (0 - 1)
	g = round(exp(f * delay_fenvamtf) * DELAY_ENVMULT - 1);    //calculate lookup table.  e.g. envamt = 0: exp(0) * 64 - 1 = 63   e.g.  envamt = 1 and lfo level = 127: exp(ln(4)) * 64 - 1 = 255  e.g. envamt = 1 and env = -127: exp(-ln(4)) * 64 - 1 = 15
	return (byte)g;
}

#endif /* PRODUCT */
