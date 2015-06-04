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

#ifndef __CYCLE_H__
#define __CYCLE_H__


void Cycle_Let_Inc(byte newamt);
byte Cycle_Get_Inc(void);
void Cycle_Let_Rep(byte newamt);
byte Cycle_Get_Rep(void);
void Cycle_Let_FenvAmt(byte newamt);
byte Cycle_Get_FenvAmt(void);
void Cycle_Let_LFOAmt(byte newamt);
byte Cycle_Get_LFOAmt(void);
void Cycle_CalcVals(void);
byte Cycle_Get_LFOGain(void);
byte Cycle_Get_EnvGain(void);


#endif /* __CYCLE_H__ */
