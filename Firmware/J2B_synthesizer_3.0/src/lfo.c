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

//**********Bipolar LFO.  16 waveforms, each 64 samples long.*************** 

#include "atmegatron.h"

// LFO waveforms store.
#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
const signed char lfo_store[16][LFO_LEN] =
{
  {0,12,25,37,49,60,71,81,90,98,106,112,117,122,125,126,127,126,125,122,117,112,106,98,90,81,71,60,49,37,25,12,0,-12,-25,-37,-49,-60,-71,-81,-90,-98,-106,-112,-117,-122,-125,-126,-127,-126,-125,-122,-117,-112,-106,-98,-90,-81,-71,-60,-49,-37,-25,-12},  //sine
  {0,8,16,24,32,40,48,56,64,71,79,87,95,103,111,119,127,119,111,103,95,87,79,71,64,56,48,40,32,24,16,8,0,-8,-16,-24,-32,-40,-48,-56,-64,-71,-79,-87,-95,-103,-111,-119,-127,-119,-111,-103,-95,-87,-79,-71,-64,-56,-48,-40,-32,-24,-16,-8},                  //triangle
  {127,123,119,115,111,107,103,99,95,91,87,83,79,75,71,67,64,60,56,52,48,44,40,36,32,28,24,20,16,12,8,4,0,-4,-8,-12,-16,-20,-24,-28,-32,-36,-40,-44,-48,-52,-56,-60,-64,-67,-71,-75,-79,-83,-87,-91,-95,-99,-103,-107,-111,-115,-119,-123},                  //saw down
  {-127,-127,-127,-127,-127,-126,-126,-126,-126,-126,-126,-125,-125,-125,-125,-124,-124,-124,-123,-123,-122,-122,-121,-121,-120,-119,-118,-118,-117,-116,-115,-113,-112,-111,-109,-107,-105,-103,-101,-99,-96,-93,-90,-87,-83,-79,-74,-69,-64,-58,-52,-45,-37,-29,-20,-11,0,12,24,38,53,69,87,107},  //Exponential ramp up
  {0,25,48,70,88,102,112,118,120,118,114,108,101,95,89,86,85,86,89,95,101,108,114,118,120,118,112,102,88,70,48,25,0,-25,-48,-70,-88,-102,-112,-118,-120,-118,-114,-108,-101,-95,-89,-86,-85,-86,-89,-95,-101,-108,-114,-118,-120,-118,-112,-102,-88,-70,-48,-25},      //3rd sine harmonics
  {127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127},  //5%
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127},              //25%
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127},                              //50% 
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127},                                              //75%
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,-127,-127,-127,-127},                                                          //95%
  {0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,67,71,75,79,83,87,91,95,99,103,107,111,115,119,123,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127},    //Ramp up and hold
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,123,119,115,111,107,103,99,95,91,87,83,79,75,71,67,64,60,56,52,48,44,40,36,32,28,24,20,16,12,8,4},  //Hold and ramp down
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,-127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127},  //duh dugga
  {127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,-127,-127,-127,-127,-127,-127,-127,-127},          //dugga duh
  {-104,-72,-14,91,-35,38,-20,-21,83,47,22,-73,-111,-104,-28,27,79,25,34,-112,-45,20,36,-30,-124,-74,-95,56,63,-91,94,78,-121,-27,49,-110,-25,24,-9,11,-80,65,-10,-56,-101,116,105,58,-67,-108,-127,95,-71,88,54,-2,83,109,-37,59,40,-77,84,-28},    //noise
  {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127}    //DC offset                                                          //95%
};

byte lfo_curtable = 0;                     //current lfo waveform
boolean lfo_invert = false;                //lfo invert mode
#define LFO_TICK_SHIFT  (13)
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
const signed char lfo_store[LFO_LEN] =
{
  0,12,25,37,49,60,71,81,90,98,106,112,117,122,125,126,127,126,125,122,117,112,106,98,90,81,71,60,49,37,25,12,0,-12,-25,-37,-49,-60,-71,-81,-90,-98,-106,-112,-117,-122,-125,-126,-127,-126,-125,-122,-117,-112,-106,-98,-90,-81,-71,-60,-49,-37,-25,-12  //sine
};

#define LFO_TICK_SHIFT  (11)
#endif /* PRODUCT */

unsigned long  lfo_ticklen = 500;          //ticks per LFO cycle.  only used when MIDI clock not present.  default = 500 ticks, therefore 1 cycle per beat
unsigned long  lfo_clockticklen = 24;      //clock ticks per LFO cylcle.  only used when MIDI clock present.  24 ticks per beat, therefore 1 cycle per beat  
unsigned long lfo_starttick = 0;           //tick lfo starts oscillating at
unsigned long lfo_curtick = 0;             //cur tick of lfo

//lets and gets
unsigned int lfo_speed = 8191;             //speed of lfo.  not actual ticks, just a reference in a lookup table that is then divided to equal actual ticks.
signed char lfo_level = 0;                        //lfo output (-127 - 127)

//lets and gets
//LFO speed.  Stored as a lookup table of multipliers, not actual ticks
void LFO_Let_Speed(unsigned int newspeed)
{
  lfo_speed = newspeed;
  // Calculate lfo tick length.  This method is a way of calculating ticks without 
  // using slow (and memory consuming) floating point maths.  
  // e.g. 1 cycle per beat = 8192 * 500 >> 13 = 500 
  lfo_ticklen = ((unsigned long)lfo_speed+1) * MASTER_BEATTICK >> LFO_TICK_SHIFT;    
  if (lfo_ticklen<2)
  {
    lfo_ticklen = 2;  // Speed must be 2 or great otherwise divide by 0 in LFO_calcval.
  }
  // Same as above, this time for when MIDI clock is present.
  lfo_clockticklen = ((unsigned long)lfo_speed+1) * 24 >> LFO_TICK_SHIFT;
  if (lfo_clockticklen<2)
  {
    lfo_clockticklen = 2;  // Speed must be 2 or great otherwise divide by 0 in LFO_calcval.
  }
}


unsigned int LFO_Get_Speed(void)
{
  return lfo_speed;
}


#if PRODUCT!=ATMEGADRUM

//set and get lfo waveform
void LFO_Let_Type(byte newtable)
{
  lfo_curtable = newtable;
}

byte LFO_Get_Type(void)
{
  return lfo_curtable;
}

//set and get invert mode
void LFO_Let_Invert(boolean newinv)
{
  lfo_invert = newinv;
}

boolean LFO_Get_Invert(void)
{
  return lfo_invert;
}

#endif /* PRODUCT */


//return current lfo level (-127 - 127)
signed char LFO_Get_Level(void)
{
#if PRODUCT!=ATMEGADRUM
  if (lfo_invert==true)
  {
    return -lfo_level;      //invert output if invert mode on
  }
  else
#endif /* PRODUCT */
  {
    return lfo_level;  
  }
}

//LFO meat
//reset lfo start tick.  lfo is reset when first key is pressed
void LFO_Reset(void)
{
	if (MIDI_Get_ClockPresent()==false)
	{
		//lfo_starttick = master_tick;
		lfo_curtick = 0;
	}
}

//calculate the current output of the lfo
void LFO_CalcVal(void)
{
  unsigned long pos; 
  int index;
  //char lfoval;
  if (MIDI_Get_ClockPresent()==false)
  {
    // If midi clock not used:
    lfo_curtick += master_tickspassed;  // Get ticks since lfo started.
    pos = lfo_curtick % lfo_ticklen;  // Calculate position within cycle in ticks.
    index = (pos * LFO_LEN) / lfo_ticklen;  // Convert to position within wavetable array
  }
  else
  {
    pos = MIDI_Get_ClockTick() % lfo_clockticklen;  // Calculate position within cycle in clock ticks.
    index = (pos * LFO_LEN) / lfo_clockticklen;  // Convert to position within wavetable array.
  }
  
#if PRODUCT==ATMEGADRUM
  lfo_level = lfo_store[index];      //set lfo level
#else
  lfo_level = lfo_store[lfo_curtable][index];      //set lfo level
#endif /* PRODUCT */
}

