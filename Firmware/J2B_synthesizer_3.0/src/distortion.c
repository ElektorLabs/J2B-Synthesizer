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

// Very simple clipping distortion

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON

//lets and gets
byte dist_amt;


void Distortion_Let_Amt(byte newamt)
{
	dist_amt = newamt;
}


byte Distortion_Get_Amt(void)
{
	return dist_amt;
}


//process the wavetable
void Distortion_Process(void)
{
	byte i;
	int s;
	for (i=0; i<WAVE_LEN; i++)        //cycle through wavetable
	{
		s = (int)Wave_Get_Process(i);  //get wavetable sample
		s = s << dist_amt;             //shift by dist_amt bits (0-7)
		if (s > 127)                   //clip sample if necessary
		{
			s = 127;
		}
		else if (s < -128)
		{
			s = -128;
		}
		Wave_Let_Process(i, (sample_t)s);  //write back to wavetable
	}
}

#endif /* PRODUCT */

