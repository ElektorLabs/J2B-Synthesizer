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

//***15 Biquad filter algorithms***

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON

const float pow_10_025 = 1.7782794100389228012254211951927; // pow(10,0.25)
const float pow_10_075 = 5.6234132519034908039495103977648; // pow(10,0.75)
const float pow_10_250 = 316.22776601683793319988935444327; // pow(10,2.5)

#if PRODUCT==DELAYERTRON
#define FILTER_SIZE  (4)
const char filter_multstore[128] =
{
	1,1,11,1,13,11,9,1,7,13,3,11,5,9,9,1,15,7,13,13,3,3,11,11,5,5,9,9,9,9,1,1,1,15,15,7,7,13,13,13,3,3,3,3,
	11,11,11,11,5,5,5,5,5,9,9,9,9,9,9,9,1,1,1,1,1,1,15,15,15,15,7,7,7,7,7,13,13,13,13,13,13,3,3,3,3,3,3,3,3,
	11,11,11,11,11,11,11,11,5,5,5,5,5,5,5,5,5,5,9,9,9,9,9,9,9,9,9,9,9,9,9,1,1,1,1,1,1,1,1
};
//{1,1,171,1,205,171,73,1,57,205,93,171,79,73,137,1,241,57,27,205,195,93,89,171,41,79,19,73,141,137,33,1,31,241,117,57,221,27,105,205,25,195,191,93,91,89,87,171,167,41,161,79,155,19,149,73,9,141,139,137,67,33,65,1,63,31,245,241,237,117,231,57,7,221,109,27,213,105,207,205,101,25,197,195,193,191,47,93,23,91,45,89,11,87,43,171,169,167,165,41,81,161,159,79,39,155,153,19,75,149,37,73,145,9,71,141,35,139,69,137,135,67,133,33,131,65,129,1};
const char filter_bsstore[128] =
{
	0,1,5,2,6,6,6,3,6,7,5,7,6,7,7,4,8,7,8,8,6,6,8,8,7,7,8,8,8,8,5,5,5,9,9,8,8,9,9,9,7,7,7,7,9,9,9,9,8,8,8,8,8,
	9,9,9,9,9,9,9,6,6,6,6,6,6,10,10,10,10,9,9,9,9,9,10,10,10,10,10,10,8,8,8,8,8,8,8,8,10,10,10,10,10,10,10,10,
	9,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,7,7,7,7,7,7,7,7
};
//{0,1,9,2,10,10,9,3,9,11,10,11,10,10,11,4,12,10,9,12,12,11,11,12,10,11,9,11,12,12,10,5,10,13,12,11,13,10,12,13,10,13,13,12,12,12,12,13,13,11,13,12,13,10,13,12,9,13,13,13,12,11,12,6,12,11,14,14,14,13,14,12,9,14,13,11,14,13,14,14,13,11,14,14,14,14,12,13,11,13,12,13,10,13,12,14,14,14,14,12,13,14,14,13,12,14,14,11,13,14,12,13,14,10,13,14,12,14,13,14,14,13,14,12,14,13,14,7};
float filt_lfoamtf = 0;
volatile boolean filt_postdelay = false;
char filter_buffer[FILTER_SIZE];
byte filter_pos;
int filter_total;
volatile byte filter_repeat = 1;
byte filter_a;
volatile boolean filter_postdelay = false;
#endif

//lets and gets
byte filt_fc = 255;              //filter cutoff - not the actual Fc, just a way to store knob position as byte (0-255)               
byte filt_q = 0;                 //filter resonance - not the actual Q, just a way to store knob position as byte (0-255)
byte filt_type = 1;              //filter type
boolean filt_gainadj = false;    //filter normalise mode (called gain adjust in code)
byte filt_fenvamt = 0;           //filter env amount
byte filt_lfoamt = 0;            //filter lfo amount

//local vars
#if PRODUCT!=DELAYERTRON

byte filt_lfolookup[256];        //look up table to convert lfo output to Fc multiplier.  Lookup saves having to do v slow exp calculation every cycle
byte filt_fenvlookup[256];       //look up table to convert env output to Fc multiplier.  Lookup saves having to do v slow exp calculation every cycle
float filt_fenvamtf = 0;         //filter env amount as float (0-1)
float filt_lfoamtf = 0;          //filter lfo amount as float (0-1)
float filt_fc_calc = 0;          //final Fc for use in biquad equation (after mult with env, lfo etc)
float filt_q_calc = 1;           //final Q for use in biquad equation (after mult with env, lfo etc)
float two_pi_over_sf= 0;         //2pi / sample freq (ie interrupt freq)
float pi_over_sf= 0;             //pi / sample freq 

// filter constants - these are the constants used by biquad equation.  Whenever Fc, Q or filter type changes, they need recalculating
float a0=1;
float a1;
float a2;
float b0;
float b1;
float b2;
float A = 1.7782794100389228012254211951927; //pow(10, 0.25);

#endif

// lets and gets
//Filter cutoff frequency knob position (0-255)
void Filt_Let_Fc(byte newfc)
{
    filt_fc = newfc;
}
byte Filt_Get_Fc(void)
{
  return filt_fc;
}
//Filter resonance (Q) knob position (0-255)
void Filt_Let_Q(byte newq)
{
  filt_q = newq;
}
byte Filt_Get_Q(void)
{
  return filt_q;
}
//Filter type (0-15)
void Filt_Let_Type(byte newtype){
  if (newtype!=filt_type){        //set new filter type
    filt_type = newtype;
#if PRODUCT!=DELAYERTRON
    switch (filt_type){           //calculate var A for shelf and peak filters: A  = sqrt( 10^(dBgain/20) )
        case FILT_PEAK10:
            //A = pow(10, 0.25);
            A = pow_10_025;
            break;
        case FILT_PEAK30:
            //A = pow(10, 0.75);
            A = pow_10_075;
            break;
        case FILT_PEAK100:
            //A = pow(10, 2.5);
            A = pow_10_250;
            break; 
        case FILT_LS10:
            //A = pow(10, 0.25);
            A = pow_10_025;
            break;
        case FILT_LS30:
            //A = pow(10, 0.75);
            A = pow_10_075;
            break;
        case FILT_HS10:
            //A = pow(10, 0.25);
            A = pow_10_025;
            break;
        case FILT_HS30:
            //A = pow(10, 0.75);
            A = pow_10_075;
            break;      
    }
#endif
  }
}

inline byte Filt_Get_Type(void)
{
  return filt_type;
}

#if PRODUCT==DELAYERTRON
void Filt_Let_PostDelay(boolean newpd)
{
	if (newpd!=filt_postdelay)
	{
		filt_postdelay = newpd;
	}
}

inline boolean Filt_Get_PostDelay(void)
{
	return filt_postdelay;
}

void Filt_CalcFc(void)
{
	unsigned int newfc = filt_fc;
	if (filt_lfoamt>0)
	{
		//mult pitch by lfo gain and bit shift.  This is the fixed point maths way of mult pitch by lfo (i.e. not using floating point)
		newfc = newfc * (Filt_Get_LFOGain() + 1) >> FILT_BS;
		if (newfc>255) newfc = 255;
	}
	filter_a = 255 - newfc;
}

int Filter_Process(int samp_in)
{
	static unsigned int filter_out;
	unsigned int in;
	if (samp_in>127) samp_in = 127;
	else if (samp_in<-128) samp_in = -128;
	in = samp_in + 128;
	// CPV filter_out = ((in * (255-filter_a))>>8) + ((filter_out * filter_a)>>8);
	filter_out = (in*(255-filter_a) + filter_out*filter_a) >> 8;
	if (filt_type==1)
	{
		// Low pass.
		return filter_out - 128;
	}
	else if (filt_type==2)
	{
		// High pass.
		return samp_in - (filter_out - 128);
	}

	// CPV - Missing in original code.
	//       Do nothing (besides wasting processing power).
	return samp_in;
}

#else /* PRODUCT */

//set filter normalise mode (gainadj in code)
void Filt_Let_GainAdj(boolean newadj){
  filt_gainadj = newadj;
}
boolean Filt_Get_GainAdj(void)
{
  return filt_gainadj;
}

// Filter meat
void Filt_CalcVals(void)
{
  unsigned long max_fc;                          //maximum cutoff frequency.  Must be < Sf/2.  Will ring at Sf/2.
  unsigned long f;                               //intermediate value for fict_fc_calc (before converting to float)

  if (filt_type>0){                              //if filter is on
    max_fc = Master_Get_SampleFreq() >> 1UL;     //calculate max cutoff frequency.  Sf / 2
    max_fc -= Master_Get_SampleFreq() >> 6UL;    //minus a bit more for safety.  reduce maxfc by proportion of Fs (about 1.5%)
    
    f = max_fc;                                  //start with Fc at maximum
    f = f * (filt_fc + 1) >> 8UL;                //then scale by filt cutoff knob position (use >>8 so knob=255 = max_fc)
  
    if (filt_lfoamt>0){                          //if filt lfo knob > 0
      f = f * (Filt_Get_LFOGain() + 1) >> FILT_BS;   //mult fc by lfo gain and bit shift.  This is the fixed point maths way of mult fc by lfo (i.e. not using floating point)
    }

    if (filt_fenvamt>0){
      f = f * (Filt_Get_FenvGain() + 1) >> FILT_BS;   //mult fc by env gain and bit shift.  This is the fixed point maths way of mult fc by env (i.e. not using floating point)
    }
    if (f > max_fc){                              //sort out if f > max_fc
      f = max_fc;         
    }
    
    filt_fc_calc = (float)f;                      //convert to float ready for biquad calculation
    filt_q_calc = (float)filt_q * MULTQ + MINQ;   //calculate q based on knob position. 0.5-20
    pi_over_sf = PI/Master_Get_SampleFreq();      //used in biquad equations
    two_pi_over_sf = 2 * pi_over_sf;              //used in biquad equations
    
    switch (filt_type){                           //calulate biquad values
        case FILT_OFF:
          break;
        case FILT_LPF:
            LPValCalculator();  
            break;
        case FILT_HPF:
            HPValCalculator();  
            break;
        case FILT_BPF:
            BPSkirtValCalculator();
            break;
        case FILT_NOTCH:
            NotchValCalculator();  
            break;
        case FILT_PEAK10:
            PeakingEQValCalculator();    
            break;
        case FILT_PEAK30:
            PeakingEQValCalculator();    
            break;
        case FILT_PEAK100:
            PeakingEQValCalculator();    
            break; 
        case FILT_LS10:
            LowShelfValCalculator();  
            break;
        case FILT_LS30:
            LowShelfValCalculator();   
            break;
        case FILT_HS10:
            HighShelfValCalculator();
            break;
        case FILT_HS30:
            HighShelfValCalculator();
            break;
        case FILT_BUTLPF:
            ButterworthLPCalculator();  
            break;
        case FILT_BUTHPF:
            ButterworthHPCalculator();  
            break;
        case FILT_BESLPF:
            BesselLPCalculator();  
            break;
        case FILT_BESHPF:
            BesselHPCalculator();  
            break;        
    }
    
    //This divide would normally be done in the Biquad_Process. However doing the time consuming divide here means that there's only 5 divides, rather than 32 (in Biquad_Process)
    
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;

  }
}

// Calculate biquad filter constants for filter
void LPValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float alpha = sin(w) / filt_q_calc * 0.5;
    float cs = cos(w);
    b0 = (1 - cs) / 2;
    b1 = 1 - cs;
    b2 = b0;
    a0 = 1 + alpha;
    a1 = -2 * cs;
    a2 = 1 - alpha;
}

void HPValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float alpha = sin(w) / filt_q_calc * 0.5;
    float cs = cos(w);
    b0 = (1 + cs) / 2;
    b1 = -(1 + cs);
    b2 = b0;
    a0 = 1 + alpha;
    a1 = -2 * cs;
    a2 = 1 - alpha;
}

void BPSkirtValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float alpha = sin(w) / filt_q_calc * 0.5;
    float sn = sin(w);
    float cs = cos(w);
    b0 =   sn/2;
    b1 =   0;
    b2 =  -b0;
    a0 =   1 + alpha;
    a1 =  -2*cs;
    a2 =   1 - alpha;
}
void NotchValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float alpha = sin(w) / filt_q_calc * 0.5;  
    float cs = cos(w);
    b0 =   1;
    b1 =  -2*cs;
    b2 =   1;
    a0 =   1 + alpha;
    a1 =   b1;
    a2 =   1 - alpha;
}

void PeakingEQValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float alpha = sin(w) / filt_q_calc * 0.5;
    float cs = cos(w);
    b0 =   1 + alpha*A;
    b1 =  -2*cs;
    b2 =   1 - alpha*A;
    a0 =   1 + alpha/A;
    a1 =   b1;
    a2 =   1 - alpha/A;
}
void LowShelfValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float cs = cos(w);
    float srA = sqrt(A);
    float alpha = sin(w) / filt_q_calc * 0.5;
    float amcs = (A-1)*cs;
    float apcs = (A+1)*cs;
    float sraaplpha = 2*srA*alpha;
    b0 =    A*( (A+1) - amcs + sraaplpha );
    b1 =  2*A*( (A-1) - apcs             );
    b2 =    A*( (A+1) - amcs - sraaplpha );
    a0 =        (A+1) + amcs + sraaplpha  ;
    a1 =   -2*( (A-1) + apcs             );
    a2 =        (A+1) + amcs - sraaplpha  ;
}
void HighShelfValCalculator(void)
{
    float w = two_pi_over_sf * filt_fc_calc;
    float cs = cos(w);
    float srA = sqrt(A);
    float alpha = sin(w) / filt_q_calc * 0.5;
    float amcs = (A-1)*cs;
    float apcs = (A+1)*cs;
    float sraaplpha = 2*srA*alpha;
    b0 =    A*( (A+1) + amcs + sraaplpha );
    b1 = -2*A*( (A-1) + apcs             );
    b2 =    A*( (A+1) + amcs - sraaplpha );
    a0 =        (A+1) - amcs + sraaplpha  ;
    a1 =    2*( (A-1) - apcs             );
    a2 =        (A+1) - amcs - sraaplpha  ;
}
void ButterworthLPCalculator(void)
{
    float k = tan(filt_fc_calc * pi_over_sf);
    b2 = k * k;
    b0 = b2;
    b1 = 2 * b0;
    a0 = b0 + (SQRT2 * k) + 1;
    a1 = 2 * (b0 - 1);
    a2 = b0 - (SQRT2 * k) + 1;
}
void ButterworthHPCalculator(void)
{
    float k = tan(filt_fc_calc * pi_over_sf);
    float k2p1 = k * k + 1;
    b0 = 1;
    b2 = b0;
    b1 = -2;
    a0 = k2p1 + (SQRT2 * k);
    a1 = 2 * (k2p1 - 2);
    a2 = k2p1 - (SQRT2 * k);
}
void BesselLPCalculator(void)
{
    float w = tan(pi_over_sf * filt_fc_calc);
    b2 = 3 * w * w;
    b0 = b2;
    b1 = 2 * b0;
    a0 = 1 + 3 * w + b0;
    a1 = -2 + b1;
    a2 = 1 - 3 * w + b0;
}
void BesselHPCalculator(void)
{
    float w = tan(pi_over_sf * filt_fc_calc);
    float w2 = w * w;
    b2 = 3;
    b0 = b2;
    b1 = -6;
    a0 = w2 + 3 * w + 3;
    a1 = 2 * w2 - 6;
    a2 = w2 - 3 * w + 3;
}

//Process the wavetable
void Filt_Process(void)
{
  byte i;
  sample_t out, maxout;
  float in, fout, multb, bout;
  
  if (filt_type>0){  
    maxout = 0;                                                        //used by filter normalise mode
    for (i=0;i<WAVE_LEN;i++){                                          //cycle through each sample
        if (filt_gainadj==true){                                       //if filter normalise mode on
          bout = Biquad_process((float)(Wave_Get_Process(i) >> 2));    //get sample and reduce amplitude.  (default 2 = amp / 4), then perform biquad process
        }
        else {    
          bout = Biquad_process((float)Wave_Get_Process(i));           //otherwise just get sample and perform biquad process
        }
        if (bout>127){                                                 //constrain biquad output to max/min char value (by clipping)
          out = 127;
        }
        else if (bout<-128){
          out = -128;
        }
        else{
          out = (sample_t)bout;
        }       
        Wave_Let_Process(i,out);                                       //write sample back to wavetable
        if (filt_gainadj==true){                                       //if filter normalise mode on
          out = abs(out);                                              //see if sample is greatest value in table and set maxout if it is
          if (out > maxout){
            maxout = out;
          }
        }
    }
    if (filt_gainadj==true){                                           //if filter normalise mode
      multb = 128 / (float)maxout;                                     //calculate multiplier to normalise waveform
      for (i=0;i<WAVE_LEN;i++){                                        //cycle through each sample                 
        in = (float)Wave_Get_Process(i);                               //get sample
        fout = in * multb;                                             //multiply by normalise multiplier
        if (fout>127){                                                 //clip sample if necessary
          out = 127;
        }
        else if (fout<-128){
          out = -128;
        }
        else{
          out = (sample_t)fout;
        }
        Wave_Let_Process(i,out);                                       //write back again
      }
    }
  }  
}
//this is the standard biquad filter equation.  normally would divide by a0, but this has already been done in filt_calcvals, to save the number of time consuming divisions required
float Biquad_process(float bi0)
{
  static float bi1;
  static float bi2;
  float bo0;
  static float bo1;
  static float bo2;
    bo0 = b0 * bi0 + b1 * bi1 + b2 * bi2 - a1 * bo1 - a2 * bo2;
    bi2 = bi1;
    bi1 = bi0;
    bo2 = bo1;
    bo1 = bo0;
    return bo0;
}

#endif /* PRODUCT */

//Filter LFO amount.  This is stored as a byte representing knob position and a float (0-FILT_MAX)
void Filt_Let_LFOAmt(byte newamt)
{
  if (newamt!=filt_lfoamt){                                //if new value different to current
    filt_lfoamt = newamt;                                  //set new value
    filt_lfoamtf = (float)filt_lfoamt * FILT_MAX / 255;    //calculate new float value used to calculate LFO lookup table (0-FILT_MAX)
#if PRODUCT!=DELAYERTRON
    memset(filt_lfolookup,0,sizeof(filt_lfolookup));       //clear the lookup table  (quicker than loop)
#endif
  }
}
byte Filt_Get_LFOAmt(void)
{
  return filt_lfoamt;
}
//Return the 'gain' of LFO.  This is stored in a lookup table to save time consuming calculations each time.  See the forums for further explanation of the 'gain' term
byte Filt_Get_LFOGain(void)
{
  float f, g;
#if PRODUCT!=DELAYERTRON
  byte index;
  index = LFO_Get_Level() + 127;                          //get the current output level of LFO (-127 - 128) and add offset (can't lookup negative array indexes)
  if (filt_lfolookup[index]==0){                          //if index of lookup table hasn't yet been calculated:
#endif
	  f = (float)LFO_Get_Level() / 127.0;                   //convert LFO output to float (0 - 1)
      g = round(exp(f * filt_lfoamtf) * FILT_MULT - 1);   //calculate lookup table.  e.g. lfoamt = 0: exp(0) * 64 - 1 = 63   e.g.  lfoamt = 1 and lfo level = 127: exp(ln(4)) * 64 - 1 = 255  e.g. lfoamt = 1 and lfo = -127: exp(-ln(4)) * 64 - 1 = 15  
#if PRODUCT==DELAYERTRON
      return (byte)g;                    //write value to lookup table
#else
      filt_lfolookup[index] = (byte)g;                    //write value to lookup table
  }
  return filt_lfolookup[index];                           //return lookuptable value
#endif
}

#if PRODUCT!=DELAYERTRON
//Filter Env amount.   This is stored as a byte representing knob position and a float (0-FILT_MAX)
void Filt_Let_FenvAmt(byte newamt)
{
  if (newamt!=filt_fenvamt){                                //if new value different to current
    filt_fenvamt = newamt;                                  //set new value
    filt_fenvamtf = (float)filt_fenvamt * FILT_MAX / 255;   //calculate new float value used to calculate env lookup table (0-FILT_MAX)
    memset(filt_fenvlookup,0,sizeof(filt_fenvlookup));      //clear the lookup table  (quicker than loop)
  }
}
byte Filt_Get_FenvAmt(void)
{
  return filt_fenvamt;
}

//Return the 'gain' of env.  This is stored in a lookup table to save time consuming calculations each time.  See the forums for further explanation of the 'gain' term
byte Filt_Get_FenvGain(void)
{
  byte index;
  float f, g;
  index = Fenv_Get_Level() + 127;                          //get the current output level of env (-127 - 128) and add offset (can't lookup negative array indexes)
  if (filt_fenvlookup[index]==0){                          //if index of lookup table hasn't yet been calculated:
      f = (float)Fenv_Get_Level() / 127;                   //convert env output to float (0 - 1)
      g = round(exp(f * filt_fenvamtf) * FILT_MULT - 1);   //calculate lookup table.  e.g. envamt = 0: exp(0) * 64 - 1 = 63   e.g.  envamt = 1 and lfo level = 127: exp(ln(4)) * 64 - 1 = 255  e.g. envamt = 1 and env = -127: exp(-ln(4)) * 64 - 1 = 15  
      filt_fenvlookup[index] = (byte)g;                    //write value to lookup table
  }
  return filt_fenvlookup[index];                           //return lookuptable value
}
#endif

#endif /* PRODUCT */
