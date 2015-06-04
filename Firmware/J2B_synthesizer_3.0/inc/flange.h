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

#ifndef __FLANGER_H__
#define __FLANGER_H__


void Flange_Let_LFOAmt(byte newamt);
byte Flange_Get_LFOAmt(void);
void Flange_Process(void);
void Flange_ClearBuffer(void);
void Flange_Process_Channel2(void); // CPV


#endif // __FLANGER_H__
