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

//******UNIPOLAR ENVELOPE - for amplitude***********

#ifndef __ENVELOPE_H__
#define __ENVELOPE_H__


void Aenv_Let_A(unsigned int newA);
unsigned int Aenv_Get_A(void);
void Aenv_Let_D(unsigned int newD);
unsigned int Aenv_Get_D(void);
void Aenv_Let_S(byte newS);
byte Aenv_Get_S(void);
void Aenv_Let_R(unsigned int newR);
unsigned int Aenv_Get_R(void);
#if PRODUCT!=DELAYERTRON
byte Aenv_Get_State(void);
#endif
inline byte Aenv_Get_Level(void);
void AEnv_Trigger(void);
void AEnv_Release(void);
void AEnv_CalcVal(void);


#endif // __ENVELOPE_H__
