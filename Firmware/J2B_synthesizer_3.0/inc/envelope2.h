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

//******PIBOLAR ENVELOPE - for filter and pitch***********

#ifndef __ENVELOPE_2_H__
#define __ENVELOPE_2_H__


#if PRODUCT==ATCYCLOTRON
void Fenv_Let_Preset(byte newamt);
byte Fenv_Get_Preset(void);
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
void Fenv_Let_A(unsigned int newA);
unsigned int Fenv_Get_A(void);
void Fenv_Let_DR(unsigned int newDR);
unsigned int Fenv_Get_DR(void);
void Fenv_Let_S(signed char newS);
signed char Fenv_Get_S(void);
#endif /* PRODUCT */

void Fenv_Let_Invert(boolean newinv);
boolean Fenv_Get_Invert(void);
signed char Fenv_Get_Level(void);
void FEnv_Trigger(void);
void FEnv_Release(void);
void FEnv_CalcVal(void);


#endif // __ENVELOPE_2_H__
