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

//*****Pulse Width Modulation.  This sets a portion of the waveform to 0.   The amount of the waveform blanked is set by the LFO and LFO amount******

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON

//lets and gets
byte pwm_lfoamt = 0;                
//LFO amount  - stored as byte 0-31.  represents the maximum proportion of waveform that can be blanked.  
void PWM_Let_LFOAmt(byte newamt)
{
  pwm_lfoamt = newamt;
}


byte PWM_Get_LFOAmt(void)
{
  return pwm_lfoamt;
}

//PWM meat
void PWM_Process(void)
{
  byte i;
  long curindex, maxindex;
  if (pwm_lfoamt > 0) {                                                                        //pwm on:
    maxindex = (WAVE_LEN - pwm_lfoamt);                                                        //intermediate value - min possible index to start blanking
    curindex = ((127 - LFO_Get_Level()) * (long)pwm_lfoamt * 8225 >> 21) + maxindex;           //then multiply by lfo level (8225 >> 21 is a way of doing / 255, without using a slow divide)
    for(i=curindex;i<WAVE_LEN;i++){                                                            //blank the relevant part of the waveform
      Wave_Let_Process(i, 0);
    }
  }
}

#endif /* PRODUCT */
