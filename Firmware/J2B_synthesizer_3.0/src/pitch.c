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

#if PRODUCT==DELAYERTRON
#define PITCH_DEFAULT  (60) /* MIDI note number. */
#else
#define PITCH_DEFAULT  (220) /* Frequency in Hz. */
#endif

byte pitch_lfolookup[256];                  //look up table to convert lfo output to pitch multiplier.  Lookup saves having to do v slow exp calculation every cycle
unsigned long pitch_lastfreq_calc = PITCH_DEFAULT;    //last value of pitch_freq_calc
float pitch_lfoamtf = 0;                    //lfo amount as float (0-1)
boolean pitch_changed = true;
byte pitch_lfoamt = 0;                      //amount lfo effects pitch

//lets and gets
#if PRODUCT==ATMEGADRUM
unsigned int pitch_curfreq = 8000;           //current frequency (must never be 0)
unsigned long pitch_freq_calc = 8000;        //like with the filter, this is just to stop curfreq changing when doing oc1a calc
#else
//unsigned long porta_starttick;              //portamento - tick glide started
unsigned int porta_curtick;                //portamento - current tick through glide
byte pitch_fenvlookup[256];                 //look up table to convert env output to pitch multiplier.  Lookup saves having to do v slow exp calculation every cycle
byte pitch_pbendlookup;                     //look up value to convert pitch bend to pitch multiplier.  Lookup saves having to do v slow exp calculation every cycle.  Don't need table, because it's a fixed value
float pitch_fenvamtf = 0;                   //env amount as float (0-1)
char pitch_lastpbend;
unsigned int pitch_lastfreq = PITCH_DEFAULT;          //last frequency set
unsigned int pitch_curfreq = PITCH_DEFAULT;           //current frequency (must never be 0)
unsigned int pitch_nextfreq = PITCH_DEFAULT;          //next frequency.  Frequency that current frequency is heading to (based on portamento level)
unsigned int pitch_portaticks = 0;          //portamento speed in ticks (ticks to glide from one note to another)
unsigned int pitch_portaspeed = 0;          //portamento speed in ticks not taking into account of distance between prev and next notes (propotional mode)
boolean pitch_propporta = false;            //portamento propotional mode
byte pitch_fenvamt = 0;                     //amount env effects pitch
volatile unsigned long pitch_freq_calc = PITCH_DEFAULT;        //master current frequency (after lfo, env, pitch wheel calculated)
#endif /* PRODUCT */


//lets and gets

#if PRODUCT==ATMEGADRUM

void Pitch_Let_CurFreq(unsigned int newfreq)
{
  pitch_curfreq = newfreq;
}

unsigned int Pitch_Get_CurFreq(void)
{
  //you should only ever get this.  Let Nextfreq for new note.
  return pitch_curfreq;
}

void Pitch_Process(void)
{
  unsigned long p;

  //set to curfreq
  p = (unsigned long)pitch_curfreq;
  //mult by LFO
  if (pitch_lfoamt>0)
  {
    p = p * (Pitch_Get_LFOGain() + 1) >> 6UL;  // this value (7) is tied in with unity gain=128 = *1, 7=128, 6=64, 5=32
  }

  pitch_freq_calc = p;

  if (pitch_freq_calc!=pitch_lastfreq_calc)
  {
    pitch_changed = true;
    pitch_lastfreq_calc = pitch_freq_calc;
  }
  else
  {
    pitch_changed = false;
  }
}

#else /* PRODUCT */

void Pitch_Let_NextFreq(unsigned int newfreq)
{  //set next frequency - i.e. the freq that the portamento heads towards
  if (newfreq!=pitch_nextfreq){                 //if new value != current, then update and refresh portamento
    pitch_nextfreq = newfreq;                   
    pitch_lastfreq = pitch_curfreq;
    Pitch_RefreshPortaTicks();
  }
}


unsigned int Pitch_Get_NextFreq(void)
{
  return pitch_nextfreq;
}


void Pitch_Let_Porta(int newporta)
{            //set portamento speed.  this is a preset value stored in hardware tab.
  if (newporta!=pitch_portaspeed){
    pitch_portaspeed = newporta;
    Pitch_RefreshPortaTicks();                 //if proportional mode is on, the actual porta speed (portaticks) needs calculating.
  }
}


int Pitch_Get_Porta(void)
{
  return pitch_portaspeed;
}


void Pitch_ResetPorta(void)
{
  //porta_starttick = master_tick;
	porta_curtick = 0;
}


void Pitch_RefreshPortaTicks(void)
{
  unsigned long p;
  if (pitch_portaspeed==0){                                                                        //if portamento is off, speed = 0 ticks
    pitch_portaticks = 0;
  }
  else if (pitch_propporta==true) {                                                                //if propotional mode is on, speeed is proportional to diff between current and next frequencies
    if (pitch_nextfreq>pitch_curfreq){
      p = (unsigned long)(pitch_nextfreq-pitch_curfreq) * pitch_portaspeed / pitch_curfreq;
    }
    else{
      p = (unsigned long)(pitch_curfreq-pitch_nextfreq) * pitch_portaspeed / pitch_nextfreq;
    }
    pitch_portaticks = (unsigned int)p;
  }
  else{
    pitch_portaticks = pitch_portaspeed;  //if proportional mode is off, speed is just = portaspeed
  }
}


void Pitch_Let_PropPorta(boolean newprop)
{
  //set proportional portamento mode.  refresh portaticks.
  if (newprop!=pitch_propporta){
    pitch_propporta = newprop;
    Pitch_RefreshPortaTicks();
  }
}


boolean Pitch_Get_PropPorta(void)
{
  return pitch_propporta;
}


//pitch Env amount.   This is stored as a byte representing knob position and a float (0-PITCH_ENVMAX)
void Pitch_Let_FenvAmt(byte newamt)
{
  if (newamt!=pitch_fenvamt){                                  //if new value different to current
    pitch_fenvamt = newamt;                                    //set new value
#if PRODUCT!=DELAYERTRON
    pitch_fenvamtf = (float)pitch_fenvamt * PITCH_ENVMAX / 255;//calculate new float value used to calculate env lookup table (0-PITCH_ENVMAX)
    memset(pitch_fenvlookup,0,sizeof(pitch_fenvlookup));       //clear the lookup table  (quicker than loop)
#endif
  }
}


byte Pitch_Get_FenvAmt(void)
{
  return pitch_fenvamt;
}


//Return the 'gain' of env.  This is stored in a lookup table to save time consuming calculations each time.  See the forums for further explanation of the 'gain' term
#if PRODUCT==DELAYERTRON
byte Pitch_Get_FenvGain(void)
{
	int env;
	env = (int)Fenv_Get_Level();
	if (env<0)
	{
		env++;
	}
	return (((env * pitch_fenvamt) >> 8) * 13) / 127;
}
#else
byte Pitch_Get_FenvGain(void)
{
  byte index;
  float f, g;
  index = Fenv_Get_Level() + 127;                                //get the current output level of env (-127 - 128) and add offset (can't lookup negative array indexes)
  if (pitch_fenvlookup[index]==0){                               //if index of lookup table hasn't yet been calculated:
      f = (float)Fenv_Get_Level() / 127;                         //convert env output to float (0 - 1)
      g = round(exp(f * pitch_fenvamtf) * PITCH_ENVMULT - 1);    //calculate lookup table.  e.g. envamt = 0: exp(0) * 64 - 1 = 63   e.g.  envamt = 1 and lfo level = 127: exp(ln(4)) * 64 - 1 = 255  e.g. envamt = 1 and env = -127: exp(-ln(4)) * 64 - 1 = 15
      pitch_fenvlookup[index] = (byte)g;                         //write value to lookup table
  }
  return pitch_fenvlookup[index];
}
#endif

#if PRODUCT!=DELAYERTRON
//Return the 'gain' of the pitch bend.   This does not require a lookup table, as it's a single value (rather than waveform or env shape)
byte Pitch_Get_MIDIPbendGain(void)
{
  float f, g;
  if (MIDI_Get_PitchBend_Level()!=pitch_lastpbend){   //has pitchbend value actually changed?
    f = (float)MIDI_Get_PitchBend_Level() / 63;       //convert to floating point.
    g = round(exp(f * PITCH_LFOMAX) * 128 - 1);       //Change 128 to change unity value (eg 64)
    pitch_pbendlookup = (byte)g;                      //convert to byte
    pitch_lastpbend = MIDI_Get_PitchBend_Level();     //set last pbend value
  }
  return pitch_pbendlookup;
}
#endif

//************PITCH PROCESS****************
void Pitch_Process(void)
{
#if PRODUCT==DELAYERTRON
  int p;
#else
  unsigned long p;
#endif
 
  if (pitch_curfreq!=pitch_nextfreq)
  {                                                              //is current frequency = next freq (i.e. the freq we want it to eventually be).  first calculate the portamento
    if (pitch_portaticks==0)
    {                                                                      //if portamento is off
      pitch_curfreq = pitch_nextfreq;                                                              //curfreq = next freq (no glide)
    }
    else
    {                                                                                          //if portamento is on
      porta_curtick += master_tickspassed;                                               //calc ticks passed
      if (porta_curtick>pitch_portaticks)
      {                                                         //curtick > portaticks, curfreq = next freq (i.e. glide has finished)
        pitch_curfreq = pitch_nextfreq;
      }
      else
      {                                                                                        //otherwise
        pitch_curfreq = map(porta_curtick,0,pitch_portaticks,pitch_lastfreq,pitch_nextfreq);       //map current frequency to ticks  (uses arduino map function:   curtick*(nextfreq - lastfreq) / portticks + lastfreq)
      }  
    }
  }
  else if (pitch_lastfreq!=pitch_curfreq)
  {                                                         //if curfreq at next freq, but last freq != cur freq, make sure it is.
    pitch_lastfreq=pitch_curfreq;
  }

#if PRODUCT==DELAYERTRON
  p = (int)pitch_curfreq;                                                                //now mult pitch by lfo, env and pitch wheel
	p = p + Pitch_Get_LFOGain();
	p = p + Pitch_Get_FenvGain();
	p = p + (MIDI_Get_PitchBend_Level() >> 3) + (MIDI_Get_PitchBend_Level() >> 4);
	if (p<0)
	{
		pitch_freq_calc = 0;
	}
	else if (p>127)
	{
		pitch_freq_calc = 127;
	}
	else
	{
		pitch_freq_calc = p;
	}
#else
	p = (unsigned long)pitch_curfreq;                                                                //now mult pitch by lfo, env and pitch wheel
	if (pitch_lfoamt>0)
	{
		p = p * (Pitch_Get_LFOGain() + 1) >> PITCH_LFOBS;                                              //mult pitch by lfo gain and bit shift.  This is the fixed point maths way of mult pitch by lfo (i.e. not using floating point)
	}
	if (pitch_fenvamt>0)
	{
		p = p * (Pitch_Get_FenvGain() + 1) >> PITCH_ENVBS;                                             //mult pitch by env gain and bit shift.  This is the fixed point maths way of mult pitch by env (i.e. not using floating point)
	}

	if (MIDI_Get_PitchBend_Level()!=0)
	{
		p = p * (Pitch_Get_MIDIPbendGain() + 1) >> 7UL;                                                //mult pitch by pbend wheel and bit shift.  This is the fixed point maths way of mult pitch by pbend (i.e. not using floating point)
	}                                                                                                //pbend is MSB only, so 0-127, hence bit shift by 7 bits
	pitch_freq_calc = p;                                                                             //set pitch_freq_calc, for use in interrupt
#endif
  if (pitch_freq_calc!=pitch_lastfreq_calc)
  {                                                       //see if pitch has actually changed since last calculation (to save unneccessary calc in interrupt)
    pitch_changed = true;
    // CPV - Original Atdelayertron code seems to have a bug here.
    pitch_lastfreq_calc = pitch_freq_calc;
  }
  else
  {
    pitch_changed = false;
  }
}

#endif /* PRODUCT */


inline unsigned long Pitch_Get_FreqCalc(void)
{
  return pitch_freq_calc;                                                                            //needed by interrupt loop to set frequency
}


boolean Pitch_Get_PitchChanged(void)
{
  return pitch_changed;
}


//Pitch LFO amount.  This is stored as a byte representing knob position and a float (0-PITCH_LFOMAX)
void Pitch_Let_LFOAmt(byte newamt)
{
  if (newamt!=pitch_lfoamt){                                    //if new value different to current
    pitch_lfoamt = newamt;                                      //set new value
#if PRODUCT!=DELAYERTRON
    pitch_lfoamtf = (float)pitch_lfoamt * PITCH_LFOMAX / 255;   //calculate new float value used to calculate LFO lookup table (0-PITCH_LFOMAX)
    memset(pitch_lfolookup,0,sizeof(pitch_lfolookup));          //clear the lookup table  (quicker than loop)
#endif
  }
}


byte Pitch_Get_LFOAmt(void)
{
  return pitch_lfoamt;
}


//Return the 'gain' of LFO.  This is stored in a lookup table to save time consuming calculations each time.  See the forums for further explanation of the 'gain' term
#if PRODUCT==DELAYERTRON
byte Pitch_Get_LFOGain(void)
{
	int lfo;
	lfo = (int)LFO_Get_Level();
	if (lfo<0)
	{
		lfo++;
	}
	return (((lfo * pitch_lfoamt) >> 8) * 13) / 127;
}
#else
byte Pitch_Get_LFOGain(void)
{
  byte index;
  float f, g;
  index = LFO_Get_Level() + 127;                              //get the current output level of LFO (-127 - 128) and add offset (can't lookup negative array indexes)
  if (pitch_lfolookup[index]==0){                             //if index of lookup table hasn't yet been calculated:
      f = (float)LFO_Get_Level() / 127;                       //convert LFO output to float (0 - 1)
      g = round(exp(f * pitch_lfoamtf) * PITCH_LFOMULT - 1);  //calculate lookup table.  e.g. lfoamt = 0: exp(0) * 64 - 1 = 63   e.g.  lfoamt = 1 and lfo level = 127: exp(ln(4)) * 64 - 1 = 255  e.g. lfoamt = 1 and lfo = -127: exp(-ln(4)) * 64 - 1 = 15   
      pitch_lfolookup[index] = (byte)g;                       //write value to lookup table
  }
  return pitch_lfolookup[index];
}
#endif


