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

#ifndef __WAVEFORMS_H__
#define __WAVEFORMS_H__


// CPV - Made extern to speed up processing
extern sample_t wave_process[WAVE_LEN];  //temp wavetable used to process wavetable
extern sample_t wave_process_ch2[WAVE_LEN];  //temp wavetable used for channel two

#if PRODUCT==ATCYCLOTRON
	void Wave_Let_Table(byte wavenum, byte newtable);
	byte Wave_Get_Table(byte wavenum);
	void Wave_Let_Bank(byte wavenum, byte newbank);
	byte Wave_Get_Bank(byte wavenum);
#else /* PRODUCT */
	void Wave_Let_Table(byte newtable);
	byte Wave_Get_Table(void);
	void Wave_Let_Bank(byte newbank);
	byte Wave_Get_Bank(void);
#endif /* PRODUCT */

#if PRODUCT!=DELAYERTRON
	// CPV - inlined to speed up processing
	inline void Wave_Let_Process(byte index, sample_t newval);
	// CPV - inlined to speed up processing
	inline sample_t Wave_Get_Process(byte index);
	void Wave_Let_UserMode(boolean newval);
	boolean Wave_Get_UserMode(void);
	void Wave_Let_UserWave(byte index, sample_t newval);
	sample_t Wave_Get_UserWave(byte index);
	void Init_Waves(void);
	void Wave_Process(void);
	void Wave_Clear(void);
#endif /* PRODUCT */


#endif // __WAVEFORMS_H__
