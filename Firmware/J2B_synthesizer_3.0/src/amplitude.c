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

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON

//lets and gets
byte amp_lfoamt = 0;  

//process wavetable
void Amplitude_Process(void)
{
  byte i;
  sample_t samp;
  for (i=0; i<WAVE_LEN; i++)
  {
    samp = Wave_Get_Process(i);                //get wavetable sample
    samp = (samp * Aenv_Get_Level()) >> 8;     //multiply sample by current amp env level (0-255) and / 255
    if (amp_lfoamt>0)
    {
      samp = (samp * Amplitude_Get_LFOGain()) >> 8;   //multiply sample by current amp lfo level (0-255) and / 255
    }
    Wave_Let_Process(i,(sample_t)samp);  //write back to wavetable
  }
}

//lets and gets
void Amplitude_Let_LFOAmt(byte newamt)             //amp lfo amount (0-255)
{
	amp_lfoamt = newamt;
}


byte Amplitude_Get_LFOAmt(void)
{
	return amp_lfoamt;
}

//calculate current amp LFO level
byte Amplitude_Get_LFOGain(void)
{
  byte lfo_gain = 0;     //255  = unity,
  lfo_gain = 253 - (amp_lfoamt  * (127 - LFO_Get_Level()) >> 8);    //253 - (lfo amount * inverse current LFO level (-127to127)) / 256  (253, not 255, coz LFO range is (-127to127)
  return lfo_gain;
}

#endif /* PRODUCT */
