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

#include "atmegatron.h"

#if PRODUCT==ATCYCLOTRON

//local
byte cycle_inc = WAVE_LEN - 1;    //number of samples to play before incrementing to next wave
byte cycle_rep = 0;               //number of times to repeat each wave before incrementing to next wave
byte cycle_lfoamt = 0;            //amount of lfo effecting inc and rep
byte cycle_fenvamt = 0;           //amount of env effecting inc and rep

void Cycle_Let_Inc(byte newamt)
{
	//0-31   (can play between 1 and full 32 samples of each wave)
	cycle_inc = newamt;
}


byte Cycle_Get_Inc(void)
{
	return cycle_inc;
}


void Cycle_Let_Rep(byte newamt)
{
	//0-31  (can repeat each wave 1 - 32 times)
	cycle_rep = newamt;                                    //set new value
}


byte Cycle_Get_Rep(void)
{
	return cycle_rep;
}

//cycle Env amount.   This is stored as a byte representing knob position 
void Cycle_Let_FenvAmt(byte newamt)
{
	if (newamt!=cycle_fenvamt)
	{
		//if new value different to current
		cycle_fenvamt = newamt;                                    //set new value
	}
}


byte Cycle_Get_FenvAmt(void)
{
	return cycle_fenvamt;
}

//cycle LFO amount.  This is stored as a byte representing knob position
void Cycle_Let_LFOAmt(byte newamt)
{
	if (newamt!=cycle_lfoamt)
	{
		//if new value different to current
		cycle_lfoamt = newamt;                                      //set new value
	}
}


byte Cycle_Get_LFOAmt(void)
{
	return cycle_lfoamt;
}

// Cycle meat
void Cycle_CalcVals(void)
{
	//the lfo and env effects just inc when <= 31.  It effects both inc and rep when > 31.

	int inc, rep;

	inc = cycle_inc;            //start with initial value of inc
	rep = cycle_rep;            //start with initial value of rep

	inc = inc + Cycle_Get_LFOGain();    //add current lfo gain to inc (0-63)

	if (inc>31)
	{
		//if inc > 31 we effect rep aswell as inc
		rep += inc - 32;          //add amount > 31 to rep
		inc = 31;                 //inc can't ever be more than 31
	}

	inc = inc + Cycle_Get_EnvGain();    //add current env gain to inc (0-63)

	if (inc>31)
	{
		//if inc > 31 we effect rep as well as inc
		rep += inc - 32;          //add amount > 31 to rep
		inc = 31;                 //inc can't every be more than 31
	}

	master_cycleinc = inc + 1;  //set master inc to temp inc value + 1 (+1 coz of way code in ISR(TIMER1_COMPA_vect) is ordered)
	master_cyclerep = rep + 1;   //set master rep to temp rep value + 1 (+1 coz of way code in ISR(TIMER1_COMPA_vect) is ordered)
}

//calculate current cycle LFO gain
byte Cycle_Get_LFOGain(void)
{
	unsigned int lfo_gain = 0;
	lfo_gain = (cycle_lfoamt * ((unsigned int)LFO_Get_Level() + 127)) >> 10;  //convert to monopolar, mult by amt and then scale to 0-63 (which is what's need for calc)
	return (byte)lfo_gain;
}

//calculate current cycle env gain
byte Cycle_Get_EnvGain(void)
{
	unsigned int env_gain = 0;
	env_gain = (cycle_fenvamt * ((unsigned int)abs(Fenv_Get_Level()))) >> 10;  //convert to monopolar, mult by amt and then scale to 0-63 (which is what's need for calc)
	return (byte)env_gain;
}

#endif /* PRODUCT */
