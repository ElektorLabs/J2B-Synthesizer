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

//*******PHASER - referred to as Flanger in code***************
//Adds delayed copy of waveform to output + feedback

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON

byte flange_lfoamt = 0;                //set maximum wave offset (0-31)
//signed char flange_store[WAVE_LEN*2];         //buffer to store 2x waveform + feedback.  need 2x waveform when delay = max (31)
sample_t flange_store[WAVE_LEN*2];         //buffer to store 2x waveform + feedback.  need 2x waveform when delay = max (31)
sample_t flange_store_ch2[WAVE_LEN*2];         //buffer to store 2x waveform + feedback.  need 2x waveform when delay = max (31)

//lets and gets
void Flange_Let_LFOAmt(byte newamt)
{
	if (newamt!=flange_lfoamt)     //if new value different to current
	{
		if (flange_lfoamt==0)       //if new value is off:
		{
			Flange_ClearBuffer();      //clear flange buffer.
		}
		flange_lfoamt = newamt;
	}
}


byte Flange_Get_LFOAmt(void)
{
	return flange_lfoamt;
}


//Flanger meat
void Flange_Process(void)
{
	byte i;
	sample_t s, o;
	long curindex, maxindex;
	//if flanger (phaser) on:
	if (flange_lfoamt > 0) 
	{
		maxindex = (WAVE_LEN - flange_lfoamt);                                                    //intermediate value (don't -1 from maxindex, so min flangestore offset is 1 (not 0) i.e. always offset by some amount)
		curindex = ((127 - LFO_Get_Level()) * (long)flange_lfoamt * 8225 >> 21) + maxindex;             //calculate delay length of delayed waveform (8225 >> 21 is way of doing / 255, without using slow divide)
		//cycle through wave form
		for (i=0; i<WAVE_LEN; i++)
		{
			s = Wave_Get_Process(i);                            //get sample
			o = (s >> 1) + (flange_store[i+curindex] >> 1);     //divide amp of sample / 2. Then add offset sample from flange_store / 2.   (both samples divided / 2 to prevent output clipping when zero offset) 
			Wave_Let_Process(i, o);                             //write back to wave table
		}
		//now waveform processed, cycle through waveform again writing to flange_store
		for (i=0; i<WAVE_LEN; i++)
		{
			flange_store[i] = Wave_Get_Process(i);
			flange_store[i+WAVE_LEN] = flange_store[i];
		}
	}
}


void Flange_ClearBuffer(void)                          
{
	// Clear buffer. Used to stop feedback making sound when key released 
	// (this was a problem when flange was combined with distortion).
	memset(flange_store,0,sizeof(flange_store));     //quicker than loop
	// CPV addition.
	memset(flange_store_ch2,0,sizeof(flange_store_ch2));     //quicker than loop
}


// CPV addition.
void Flange_Process_Channel2(void)
{
	long curindex;
	//long maxindex;
	//if flanger (phaser) on:
	//if (flange_lfoamt>0)
	{
		//intermediate value (don't -1 from maxindex, so min flangestore offset is 1 (not 0) i.e. always offset by some amount)
//		maxindex = (WAVE_LEN - flange_lfoamt);
		//calculate delay length of delayed waveform (8225 >> 21 is way of doing / 255, without using slow divide)
//		curindex = ((127 - LFO_Get_Level()) * (long)flange_lfoamt * 8225 >> 21) + maxindex;
		curindex = 15; //((uint8_t)(127-LFO_Get_Level())) >> 4;
		memcpy(wave_process_ch2,flange_store_ch2+curindex,WAVE_LEN*sizeof(sample_t));
		memcpy(flange_store_ch2,wave_process,WAVE_LEN*sizeof(sample_t));
		memcpy(flange_store_ch2+WAVE_LEN,wave_process,WAVE_LEN*sizeof(sample_t));
	}
}

#endif /* PRODUCT */
