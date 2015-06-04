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

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON

boolean wave_usermode = false;                     //user mode

//lets and gets
#if PRODUCT==ATCYCLOTRON
byte wave_curtable[TOT_WAVES];                            //current wavetable number
byte wave_curbank[TOT_WAVES];                             //current bank number
byte wave_curtablecalc[TOT_WAVES];                //wavetable actually used in calculation
boolean wave_noise[4];                                //is the wavetable = noise (gnerated in real time)
#else /* PRODUCT */
byte wave_curtable = 0;                            //current wavetable number
byte wave_curbank = 0;                             //current bank number
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON
boolean wave_noise;                                //is the wavetable = noise (gnerated in real time)
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
sample_t wave_process[WAVE_LEN];                       //temp wavetable used to process wavetable
sample_t wave_process_ch2[WAVE_LEN];  //temp wavetable used for channel two
sample_t wave_user[WAVE_LEN];                          //wavetable of current user wavetable (flash mem too slow to stream from)
boolean wave_cleared;                              //if all keys up, has wave_process been blanked?
#endif /* PRODUCT */

//lets and gets
#if PRODUCT==ATCYCLOTRON
void Wave_Let_Table(byte wavenum, byte newtable)                 //sets the wavetable number
{
	if (newtable!=wave_curtable[wavenum])                     //if new val is different:
	{
		wave_curtable[wavenum] = newtable;                      //set new num
		if (wave_curtable[wavenum]==15 && wave_curbank[wavenum]==0)      //bank 0, wave 15 = noise
		{
			wave_noise[wavenum] = true;
		}
		else
		{
			wave_noise[wavenum] = false;
		}
	}
}


byte Wave_Get_Table(byte wavenum)
{
	return wave_curtable[wavenum];
}


void Wave_Let_Bank(byte wavenum, byte newbank)                   //set the bank number
{
	if (newbank!=wave_curbank[wavenum])                       //if new val is different:
	{
		wave_curbank[wavenum] = newbank;                        //set new bank
		if (wave_curtable[wavenum]==15 && wave_curbank[wavenum]==0)      //bank 0, wave 15 = noise
		{
			wave_noise[wavenum] = true;
		}
		else
		{
			wave_noise[wavenum] = false;
		}
	}
}


byte Wave_Get_Bank(byte wavenum)
{
	return wave_curbank[wavenum];
}
#else /* PRODUCT */

void Wave_Let_Table(byte newtable)                 //sets the wavetable number
{
	if (newtable!=wave_curtable)                     //if new val is different:
	{
		wave_curtable = newtable;                      //set new num
#if PRODUCT==ATMEGATRON
		if (wave_curtable==15 && wave_curbank==0)      //bank 0, wave 15 = noise
		{
			wave_noise = true;
		}
		else
		{
			wave_noise = false;
		}
#endif
#if PRODUCT==DELAYERTRON
		master_curtable = newtable;
#endif
	}
}


byte Wave_Get_Table(void)
{
	return wave_curtable;
}


void Wave_Let_Bank(byte newbank)                   //set the bank number
{
	if (newbank!=wave_curbank)                       //if new val is different:
	{
		wave_curbank = newbank;                        //set new bank
#if PRODUCT==ATMEGATRON
		if (wave_curtable==15 && wave_curbank==0)      //bank 0, wave 15 = noise
		{
			wave_noise = true;
		}
		else
		{
			wave_noise = false;
		} 
#endif
#if PRODUCT==DELAYERTRON
		master_curbank = newbank;
#endif
	}
}


byte Wave_Get_Bank(void)
{
	return wave_curbank;
}
#endif /* PRODUCT */


#if PRODUCT!=DELAYERTRON

inline void Wave_Let_Process(byte index, sample_t newval)     //set sample in processing wavetable.  used by all the other tabs
{
	wave_process[index] = newval;
}


inline sample_t Wave_Get_Process(byte index)
{
	return wave_process[index];
}


void Wave_Let_UserMode(boolean newval)             //set user mode
{
	wave_usermode = newval;
}


boolean Wave_Get_UserMode(void)
{
	return wave_usermode;
}


void Wave_Let_UserWave(byte index, sample_t newval)    //set sample of user wavetable.  Used when loading userwave from flash or sysex dump in librarian
{
	wave_user[index] = newval;
}


sample_t Wave_Get_UserWave(byte index)
{
	return wave_user[index];
}


//Wavetable meat
void Init_Waves(void)                            //initialise the wavetables.  would be really annoying to boot to silent sound!
{
	byte i;
	for (i=0;i<WAVE_LEN;i++)
	{
#if PRODUCT==ATCYCLOTRON
		wave_process[i] = wave_store[wave_curbank[0]][wave_curtable[0]][i];
		wave_user[i] = wave_store[wave_curbank[0]][wave_curtable[0]][i];
#else /* PRODUCT */
		wave_process[i] = wave_store[wave_curbank][wave_curtable][i];
		wave_user[i] = wave_store[wave_curbank][wave_curtable][i];
#endif /* PRODUCT */
	}
}


//******************PROCESS THE WAVETABLE*******************
//****This is where the wavetbale is filtered/shaped/etc****
void Wave_Process(void)
{
	byte i;
#if PRODUCT==ATCYCLOTRON
	byte w;
	for (w=0;w<TOT_WAVES;w++)
	{
#endif /* PRODUCT */
	if (wave_usermode==true)                 //if userwave is on, initialise process wavetable to userwave wavetable
	{
		for (i=0;i<WAVE_LEN;i++)
		{
			wave_process[i] = wave_user[i];
		}
	}
	else if (wave_noise
#if PRODUCT==ATCYCLOTRON
		[w]
#endif /* PRODUCT */
			==true)               //if noise, generate the noise
	{
		// TODO cpv: this creates aliased noise, fix it.
		for (i=0;i<WAVE_LEN;i++)
		{
			wave_process[i] = random(-128,127);
		}
	}
	else                                     //otherwise init the process wavetable from progmem
	{
		for (i=0;i<WAVE_LEN;i++)
		{
#if PRODUCT==ATCYCLOTRON
			wave_process[i] = wave_store[wave_curbank[w]][wave_curtable[w]][i];
#else /* PRODUCT */
			wave_process[i] = wave_store[wave_curbank][wave_curtable][i];
#endif /* PRODUCT */
		}
	}
	if (PWM_Get_LFOAmt()>0)                                          //1.  Apply PWM to wavetable
	{
		PWM_Process();
	}
	if (BitCrush_Get_Type()>0 && BitCrush_Get_PreFilt()==true)       //2.  Apply wave crusher if pre-filt mode on
	{
		BitCrush_Process();
	}
#if PRODUCT==ATMEGATRON
	if (Filt_Get_Type()>0)                                           //3.  Apply filter
	{
		Filt_Process();
	}
#endif /* PRODUCT */
	Amplitude_Process();                                             //4.  Apply amplitude (always do this)
	if (Flange_Get_LFOAmt()>0)                                       //5.  Apply phaser
	{
		Flange_Process();
	}
	if (BitCrush_Get_Type()>0 && BitCrush_Get_PreFilt()==false)      //6.  Apply wave crusher if pre-filt mode off
	{
		BitCrush_Process();
	}
	if (Distortion_Get_Amt()>0)                                      //7.  Apply distortion
	{
		Distortion_Process();
	}
	
	for (i=0;i<WAVE_LEN;i++)                                         //8.  Write processed wave to output
	{
#if PRODUCT==ATCYCLOTRON
		master_output[w][i] = wave_process[i]+128;
		//master_output_ch2[w][i] = wave_process_ch2[i]+128;  // CPV
#else /* PRODUCT */
		master_output[i] = wave_process[i]+128;
		//master_output_ch2[i] = wave_process_ch2[i]+128;  // CPV
#endif /* PRODUCT */
	}

#if PRODUCT==ATCYCLOTRON
	}
#endif /* PRODUCT */

	wave_cleared=false;                                               //wavetable isn't blank
}


void Wave_Clear(void)                                              //this is used to make sure that synth is silent when all keys are up
{
	byte i;                                                          //the main issue was the phaser + distortion would feedback endlessly, even with keys up
#if PRODUCT==ATCYCLOTRON
	byte j;                                                          //the main issue was the phaser + distortion would feedback endlessly, even with keys up
#endif /* PRODUCT */
	if (wave_cleared==false)
	{
		for (i=0; i<WAVE_LEN; i++)
		{
			wave_process[i] = 0;
			wave_process_ch2[i] = 0;
#if PRODUCT==ATCYCLOTRON
			for (j=0; j<TOT_WAVES; j++)
			{
				master_output[j][i] = 128;
				//master_output_ch2[j][i] = 128;
			}
#else /* PRODUCT */
			master_output[i] = 128;
			//master_output_ch2[i] = 128;
#endif /* PRODUCT */
		}
		wave_cleared = true;
	}
}

#endif /* PRODUCT */
#endif /* PRODUCT */
