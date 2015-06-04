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

//*******THIS IS THE WAVE CRUSHER***************

#ifndef __BITCRUSHER_H__
#define __BITCRUSHER_H__


void BitCrush_Let_Type(byte newtype);
byte BitCrush_Get_Type(void);
void BitCrush_Let_PreFilt(boolean newprefilt);
boolean BitCrush_Get_PreFilt(void);
void BitCrush_Process(void);


#endif // __BITCRUSHER_H__
