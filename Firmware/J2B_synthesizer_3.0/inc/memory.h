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

//******Read and write patches to flash memory**************

#ifndef __MEMORY_H__
#define __MEMORY_H__


//save patch
void Memory_Save(byte patchnum);
bool Memory_Load(byte patchnum);


#if PRODUCT==ATMEGADRUM
	void Memory_Let_LoadOnBeat(byte newval);
	byte Memory_Get_LoadOnBeat(void);
#else
	#define SYSEXBEGIN 240
	#define SYSEXEND 247
	bool Memory_Load_Patch(uint8_t *p_dst, uint32_t src); // CPV
	void Memory_Vanilla(void);
	#if PRODUCT!=DELAYERTRON
		void Memory_UserWave_Write(byte patchnum);
		void Memory_UserWave_Read(byte patchnum);
		void Memory_SYSEX_write_mem(void);
		void Memory_Channel_Write(byte channel);
		byte Memory_Channel_Read(void);
	#endif /* PRODUCT */
#endif /* PRODUCT */


#endif // __MEMORY_H__
