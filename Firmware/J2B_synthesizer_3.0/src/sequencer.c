/*
Copyright 2014 Paul Soulsby www.soulsbysynths.com
    This file is part of Atmegatron.

    Atmegadrum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Atmegadrum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Atmegadrum.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "atmegatron.h"

#if PRODUCT==ATMEGADRUM

//{1000100010001000}      //kick1
//{0000000000000010}      //kick2
//{1000000010000000}      //kick3
//{0000100000000000}      //snare1
//{0000000000001000}      //snare2
//{0000000000000100}      //snare3
//{0000000000000011}      //snare4
//{0000000000000110}      //clap
//{0001000010000000}      //tom1
//{0000000000010000}      //tom2
//{1010101010101010}      //chh1
//{0100000001000000}      //chh2
//{1000000000000000}      //ohh1
//{0010001000100010}      //ohh2
//{1001000010010000}      //cowbell
//{0000011000000000}      //stick

uint16_t seq_patstore[16] =
{
	4369,   // 0x1111, ---X---X---X---X, kick1
	16384,  // 0x4000, -X--------------, kick2
	257,    // 0x0101, -------X-------X, kick3
	16,     // 0x0010, -----------X----, snare1
	4096,   // 0x1000, ---X------------, snare2
	8192,   // 0x2000, --X-------------, snare3
	49152,  // 0xc000, XX--------------, snare4
	24576,  // 0x6000, -XX-------------, clap
	4224,   // 0x1080, ---X--------X---, tom1
	16,     // 0x0010, -----------X----, tom2
	21845,  // 0x5555, -X-X-X-X-X-X-X-X, chh1
	514,    // 0x0202, ----------X---X-, chh2
	1,      // 0x0001, ---------------X, ohh1
	17476,  // 0x4444, -X---X---X---X--, ohh2
	2313,   // 0x0909, ----X--X----X--X, cow bell
	96      // 0x0060, ---------XX-----, stick
};
byte seq_volume[16] = {0,0,0,0,0,0,0,0,0,0,2,0,0,3,1,0};
byte seq_amplitude[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte seq_delaynext[16] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
byte seq_delaybeats[16] = {0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0};
	
unsigned long seq_ticklen = 2000;
unsigned long seq_clockticklen = 96;
unsigned long seq_starttick = 0;
unsigned long seq_curtick = 0;
unsigned long seq_tickpos; 
byte seq_curpos = 0;
byte seq_lastpos = 255;

boolean seq_playing = false;
byte seq_swingamt = 7;

byte seq_current_position = 0;


//lets and gets
void Seq_Let_Pattern(byte inst, int beat, boolean newval)
{
	if (newval==true)
	{
		seq_patstore[inst] |= (1 << beat);
	}
	else
	{
		seq_patstore[inst] &= ~(1 << beat);
	}
}


boolean Seq_Get_Pattern(byte inst, int beat)
{
	return (seq_patstore[inst] & (1 << beat)) != 0;
}


void Seq_Let_PatternRaw(byte inst, uint16_t pat)
{
	seq_patstore[inst] = pat;
}


uint16_t Seq_Get_PatternRaw(byte inst)
{
	return seq_patstore[inst];
}


void Seq_Let_Volume(byte inst, byte newval)
{
	seq_volume[inst] = newval;
}


byte Seq_Get_Volume(byte inst)
{
	return seq_volume[inst];
}


void Seq_Let_DelayBeats(byte inst, byte newval)
{
	seq_delaybeats[inst] = newval;
	if (newval==0)
	{
		seq_delaynext[inst] = 255;
	}
}


byte Seq_Get_DelayBeats(byte inst)
{
	return seq_delaybeats[inst];
}


void Seq_Let_Amplitude(byte inst, byte newval)
{
	seq_amplitude[inst] = newval;
}


byte Seq_Get_Amplitude(byte inst)
{   //post delay
	return seq_amplitude[inst];
}


void Seq_Let_Playing(boolean newval, boolean cont)
{
	if (newval!=seq_playing)
	{
		seq_playing = newval;
		if (newval==true)
		{
			if (MIDI_Get_ClockPresent()==false)
			{
				seq_starttick = master_tick;
			}
			if (cont==false)
			{
				LFO_Reset();
				seq_lastpos = 255;  //force play first index
			}
		}
	}
}


byte Seq_Get_Playing(void)
{
  return seq_playing;
}


void Seq_Let_Tempo(byte newtempo)
{
	float mult;
	mult = (float)seq_tickpos / (float)seq_ticklen;
	seq_ticklen = 240000 / newtempo;
	seq_tickpos = seq_ticklen * mult;
	seq_starttick = master_tick - seq_tickpos;
}


byte Seq_Get_Tempo(void)
{
	unsigned long tempo = 240000/seq_ticklen;
	return (byte)tempo;
}


//meat
void Seq_Clear(void)
{
	memset(seq_patstore,0,sizeof(seq_patstore));
	memset(seq_volume,0,sizeof(seq_volume));
	memset(seq_delaynext,255,sizeof(seq_delaynext));
	memset(seq_delaybeats,0,sizeof(seq_delaybeats));
	Pitch_Let_CurFreq(8000);
}


void Seq_Process(void)
{
	byte seq_curpos;
	byte i;
	if (seq_playing==true)
	{
		if (MIDI_Get_ClockPresent()==false)
		{
			seq_curtick = master_tick - seq_starttick;
			seq_tickpos = seq_curtick % seq_ticklen;
			seq_curpos =  (SEQ_LEN * seq_tickpos) / seq_ticklen;
		}
		else
		{
			seq_tickpos = MIDI_Get_ClockTick() % seq_clockticklen;
			seq_curpos =  (SEQ_LEN * seq_tickpos) / seq_clockticklen;
		}  
		Seq_Let_Current_Position(seq_curpos);
		if (seq_curpos!=seq_lastpos)
		{
			//display_invalidate();
			lcd_cursor(1,seq_curpos);

			if ((seq_curpos==0) && (Memory_Get_LoadOnBeat()!=255))
			{
				Memory_Load(Memory_Get_LoadOnBeat());
				Memory_Let_LoadOnBeat(255);
				display_invalidate();
				//Hardware_LED_CancelFlash(0);
			}
			for (i=0;i<16;i++)
			{
				if ((Seq_Get_Pattern(i,seq_curpos)==true) || (seq_delaynext[i]==seq_curpos))
				{
					master_index[i] = Wavestore_Get_Start(i);
					if (Seq_Get_Pattern(i,seq_curpos)==true)
					{
						seq_amplitude[i] = seq_volume[i];
					}
					if (seq_delaybeats[i]>0) 
					{
						if (seq_amplitude[i]<8)
						{			//if delaying
							seq_delaynext[i] = seq_curpos + seq_delaybeats[i];  //set beat that next delay occurs on
							if (seq_delaynext[i]>15)
							{
								seq_delaynext[i] -= 16;
							}
							if (Seq_Get_Pattern(i,seq_curpos)==false)
							{
								if (seq_amplitude[i]==0)
								{	//if start at max amp
									seq_amplitude[i] = 1;		//first delay = 1
								}
								else
								{
									seq_amplitude[i] += seq_amplitude[i];  // then halve each time
								}
							}
						}
						else
						{
							seq_delaynext[i] = 255;  //delay has finished
						}
					}
				}
			}
			seq_lastpos = seq_curpos;
		}
	}
}


inline void Seq_Let_Current_Position(byte pos)
{
	seq_current_position = pos;
}


inline byte Seq_Get_Current_Position(void)
{
	return seq_current_position;
}


#endif /* PRODUCT */
