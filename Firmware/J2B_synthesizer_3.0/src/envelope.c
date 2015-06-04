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

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON

//lets and gets
unsigned int aenv_A = 0;         //attack (in ticks)
unsigned int aenv_D = 0;         //decay  (in ticks)
byte aenv_S = 255;      //sustain (0-255)
unsigned int aenv_R = 0;         //release (in ticks)
byte aenv_level = 0;    //current output level (0-255)

//local variables
unsigned int aenv_curtick;      //current tick through env shape
//unsigned long  aenv_starttick;    //tick that env started at
byte aenv_curstate = 0;           //0 = off, 1 = triggering, 2 = holding, 3 = releasing 
boolean aenv_changed = true;      //used to only calc Env when it's been triggered
byte aenv_Rstart = 0;             //incase key up before decay finishes

// lets and gets

//set attack (in ticks)
void Aenv_Let_A(unsigned int newA)
{
  if (newA!=aenv_A)
  {
    aenv_A = newA;
    aenv_changed = true;
  }
}


unsigned int Aenv_Get_A(void)
{
  return aenv_A;
}


//set decay (in ticks)
void Aenv_Let_D(unsigned int newD)
{
  if (newD!=aenv_D)
  {
    aenv_D = newD;
    aenv_changed = true;
  }
}


unsigned int Aenv_Get_D(void)
{
  return aenv_D;
}


//set sustain (0-255)
void Aenv_Let_S(byte newS)
{
  if (newS!=aenv_S)
  {
    aenv_S = newS;
    aenv_changed = true;
  }
}


byte Aenv_Get_S(void)
{
  return aenv_S;
}


//set release (in ticks)
void Aenv_Let_R(unsigned int newR)
{
  if (newR!=aenv_R)
  {
    aenv_R = newR;
    aenv_changed = true;
  }
}


unsigned int Aenv_Get_R(void)
{
  return aenv_R;
}


#if PRODUCT!=DELAYERTRON
//get current state of env (0 = off, 1 = triggering, 2 = holding, 3 = releasing)
byte Aenv_Get_State()
{
  return aenv_curstate;
}
#endif


//get current level of env (0-255)
inline byte Aenv_Get_Level(void)
{
  return aenv_level;  
}


//ENVELOPE MEAT
//trigger the (start of) envelope
void AEnv_Trigger(void)
{
  aenv_curtick = 0;              //reset curtick
  //aenv_starttick = master_tick;  //set starttick = master tick
  aenv_curstate = 1;             //set state to attack
  aenv_changed = true;           //so calcval does something
}


//trigger release stage of envelope
void AEnv_Release(void)
{
  aenv_curtick = 0;              //reset curtick
  //aenv_starttick = master_tick;  //set starttick = master tick
  aenv_curstate = 3;             //set state to release
  aenv_Rstart = aenv_level;      //set release start level = current level.  This may NOT be sustain level, if still in decay stage
  aenv_changed = true;           //so calcval does something
}


//calculate env output level
void AEnv_CalcVal(void)
{
  if (aenv_changed==true)
  {                                                      //is envelope actually doing something?
    switch (aenv_curstate)
    {
      case 0:                                                                   //idle
        aenv_level = 0;
        aenv_changed = false;
        break;
      case 1:                                                                   //triggering
        aenv_curtick += master_tickspassed;                            //update tick
        if (aenv_curtick>=aenv_A+aenv_D)
        {                                       //if attack + decay ticks passed, then onto sustain
          aenv_level = aenv_S;
          aenv_curstate = 2;
        }
        else if (aenv_curtick>=aenv_A)
        {                                          //if attack ticks passed, then onto decay
          aenv_level = map(aenv_curtick,aenv_A,aenv_A+aenv_D,255,aenv_S);        
        }
        else
        {
          aenv_level = map(aenv_curtick,0,aenv_A,0,255);                          //otherwise must be doing attack
        }
        break;
      case 2:                                                                     //holding
        aenv_level = aenv_S;                                                      //make sure level = sustain level
        aenv_changed = false;                                                     //only need to do this once
        break;
      case 3:
        aenv_curtick += master_tickspassed;  //releasing
        if (aenv_curtick>=aenv_R)
        {                                                //if release ticks passed, then go back to idle
          aenv_level = 0;
          aenv_curstate = 0;
        }
        else
        {                                                                      //otherwise must be releasing
          aenv_level = map(aenv_curtick,0,aenv_R,aenv_Rstart,0);
        }
        break;
    } 
  }
}

#endif /* PRODUCT */
