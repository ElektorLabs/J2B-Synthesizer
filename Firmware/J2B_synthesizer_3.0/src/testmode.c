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
//local vars  - uncomment all this is test mode is ever needed again.

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON

#ifdef POLL_TEST

#define SEQLEN 7


//****test mode - automatically triggers midi notes every second*******
byte test_pos = 0;  //position in test sequence
const byte test_seq[7] = 
{
	21, 33, 45, 57, 69, 81, 93
};


//lets and gets
unsigned long testmode_nexttick = 0;


void Testmode_Let_NextTick(unsigned long newtick)
{
 	testmode_nexttick=newtick;
}


long Testmode_Get_NextTick(void)
{
 	return testmode_nexttick;
}


void Testmode_ProcessNext(void)
{
	byte i;
	MIDI_NoteOn(test_seq[test_pos]);  //fire note
	test_pos++;  //increment pos
	if (test_pos>=SEQLEN)
	{            
		//if hit end of seq
		test_pos=0;  //reset pos
		for (i=0;i<SEQLEN;i++)
		{
			//turn all notes off
			MIDI_NoteOff(test_seq[i]);
		}
	}
	testmode_nexttick += 1000;  //set tick for next trigger
}

#endif

#endif /* PRODUCT */
