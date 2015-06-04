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


#ifndef __ARPEGGIATOR_H__
#define __ARPEGGIATOR_H__


//****THIS IS WHERE THE ARPEGGIATOR PATTERNS ARE STORED*******
//The way its is by up the notes currently being held down
//So if C,D,E,F,G  were held down and pattern was {5,3,2,4,1},  
//the arp would be G,E,D,F,C.  Minus nums count downwards

#define PATLEN 16 

extern const signed char arp_patstore[16][16];


void Arp_Let_Type(byte newpat);
byte Arp_Get_Type(void);
void Arp_Let_Speed(unsigned int newspeed);
unsigned int Arp_Get_Speed(void);
void Arp_Let_PingPong(boolean newpingpong);
boolean Arp_Get_PingPong(void);
void Arp_Reset(void);
void Arp_Poll(void);
void Arp_TriggerStep(void);
byte Arp_FindNote(signed char pos);
void Arp_IncPatPos(void);


#endif // __ARPEGGIATOR_H__
