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

#ifndef __PWM_H__
#define __PWM_H__


void PWM_Let_LFOAmt(byte newamt);
byte PWM_Get_LFOAmt(void);
void PWM_Process(void);


#endif // __PWM_H__
