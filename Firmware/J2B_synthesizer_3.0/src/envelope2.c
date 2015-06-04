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

//****** BIPOLAR ENVELOPE - for filter and pitch***********

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON

#if PRODUCT==ATCYCLOTRON
//This now works as a set of 16 presets. The preset values are below
const byte envA_preset[16] =  { 0, 0, 0, 0, 0, 3, 3, 5, 5, 7, 7, 7,11,11,15,15};
const byte  envDR_preset[16] = { 3, 5, 7, 7,11, 0, 3, 0, 7, 0, 3, 7, 0, 5, 0,15};
const byte  envS_preset[16] =  { 0, 0, 0, 7, 0, 0, 0,15, 0, 0, 0, 7,15, 0,15, 0};
byte fenv_preset = 2;
#define FENV_DR  (381)
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
#define FENV_DR  (400)
#endif /* PRODUCT */

//lets and gets 
unsigned int fenv_A = 0;                //attack (in ticks)
unsigned int fenv_DR = FENV_DR;             //decay AND release (in ticks)  (only reason for this is because there's not enough functions on function dial for both)
signed char fenv_S = 0;        //sustain (-127 - 127)
signed char fenv_level = 0;    //the current output level of the env.  between -127 and 127
boolean fenv_invert = false;   //envelope invert mode

//local filt env vars
unsigned int fenv_curtick;   //current tick through env shape
//unsigned long  fenv_starttick; //tick that env started at
byte fenv_curstate = 0;        //0 = off, 1 = triggering, 2 = holding, 3 = releasing ;
boolean fenv_changed = true;   //used to only calc Env when it's been triggered
signed char fenv_Rstart = 0;   //incase key up before decay finishes

//lets and gets

#if PRODUCT==ATCYCLOTRON
//fenv preset
void Fenv_Let_Preset(byte newamt)
{      //0-31
	if (newamt!=fenv_preset)
	{
		fenv_preset = newamt;
		fenv_A = Hardware_Get_envADR(envA_preset[fenv_preset]);
		fenv_DR = Hardware_Get_envADR(envDR_preset[fenv_preset]);
		fenv_S = Hardware_Get_envS(envS_preset[fenv_preset]);
		fenv_changed = true;
	}
}

byte Fenv_Get_Preset(void)
{
	return fenv_preset;
}
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON

//set attack (in ticks)
void Fenv_Let_A(unsigned int newA)
{
	if (newA!=fenv_A)
	{
		fenv_A = newA;
		fenv_changed = true;
	}
}


unsigned int Fenv_Get_A(void)
{
	return fenv_A;
}


//set decay and release (in ticks)
void Fenv_Let_DR(unsigned int newDR)
{
	if (newDR!=fenv_DR)
	{
		fenv_DR = newDR;
		fenv_changed = true;
	}
}


unsigned int Fenv_Get_DR(void)
{
	return fenv_DR;
}


//set sustain (0-127) -127 - 0 is obtained by turning on invert mode
void Fenv_Let_S(signed char newS)
{
	if (newS!=fenv_S)
	{  
		fenv_S = newS;
		fenv_changed = true;
	}
}


signed char Fenv_Get_S(void)
{
	return fenv_S;
}
#endif /* PRODUCT */


//set invert mode
void Fenv_Let_Invert(boolean newinv)
{
	if (newinv!=fenv_invert)
	{
		fenv_invert = newinv;
		fenv_changed = true;
	}
}


boolean Fenv_Get_Invert(void)
{
	return fenv_invert;
}


//get current level of env (-127 - 127)
signed char Fenv_Get_Level(void)
{
	//invert output value if invert mode is on
	if (fenv_invert==true)
	{    
		return -fenv_level;
	}
	else
	{
		return fenv_level;  
	}
}


//meat
//trigger the (start of) envelope
void FEnv_Trigger(void)
{
	fenv_curtick = 0;
	//fenv_starttick = master_tick;
	fenv_curstate = 1;
	fenv_changed = true;
}


//trigger release stage of envelope
void FEnv_Release(void)
{
	fenv_curtick = 0;
	//fenv_starttick = master_tick;
	fenv_Rstart = fenv_level;
	fenv_curstate = 3;
	fenv_changed = true;
}


//calculate env output level
void FEnv_CalcVal(void)
{
  if (fenv_changed==true){                                                    //is envelope actually doing something?
    switch (fenv_curstate){
      case 0:                                                                 //idle
        fenv_level = 0;
        fenv_changed = false;
        break;
      case 1:
        fenv_curtick += master_tickspassed;                          //triggering
        if (fenv_curtick>=fenv_A+fenv_DR){                                    //update tick
          fenv_level = fenv_S;                                                //if attack + decay ticks passed, then onto sustain
          fenv_curstate = 2;
        }
        else if (fenv_curtick>=fenv_A){                                       //if attack ticks passed, then onto decay
          fenv_level = map(fenv_curtick,fenv_A,fenv_A+fenv_DR,127,fenv_S);
        }
        else{                                                                 //otherwise must be doing attack
          fenv_level = map(fenv_curtick,0,fenv_A,0,127);
        }
        break;
      case 2:                                                                 //holding
        fenv_level = fenv_S;                                                  //make sure level = sustain level
        fenv_changed = false;                                                 //only need to do this once
        break;
      case 3:                                                                 //releasing
        fenv_curtick += master_tickspassed;
        if (fenv_curtick>=fenv_DR){                                           //if release ticks passed, then go back to idle
          fenv_level = 0;
          fenv_curstate = 0;
        }
        else{                                                                 //otherwise must be releasing
          fenv_level = map(fenv_curtick,0,fenv_DR,fenv_Rstart,0);
        }
        break;
    }
  }
}

#endif /* PRODUCT */
