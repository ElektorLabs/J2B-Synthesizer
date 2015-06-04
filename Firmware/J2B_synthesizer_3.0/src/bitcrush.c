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

#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON

//local vars
byte sampshft = 8;
byte samphold = 32;

//local lets and gets
byte bitcrush_type = 0;   //0=off
boolean bitcrush_prefilt = false;  

//lets and gets
//set the bitcrush parameters
void BitCrush_Let_Type(byte newtype)
{
  if (newtype!=bitcrush_type){
    bitcrush_type = newtype;
    switch (bitcrush_type){
      case 0:              //Bit crush off
        break;
      case 1:                        
        sampshft = 5;      //sampshft decreases bit depth
        samphold = 1;      //sampshft decrease sample frequency
        break;
      case 2:    
        sampshft = 6;
        samphold = 1;
        break;
      case 3:    
        sampshft = 7;
        samphold = 1;
        break;        
      case 4:    
        sampshft = 4;
        samphold = 2;
        break;
      case 5:    
        sampshft = 5;
        samphold = 2;
        break;
      case 6:    
        sampshft = 6;
        samphold = 2;
        break;
      case 7:    
        sampshft = 7;
        samphold = 2;
        break;
      case 8:    
        sampshft = 4;
        samphold = 4;
        break;
      case 9:    
        sampshft = 5;
        samphold = 4;
        break;        
      case 10:    
        sampshft = 6;
        samphold = 4;
        break;        
      case 11:    
        sampshft = 7;
        samphold = 4;
        break;       
      case 12:    
        sampshft = 4;
        samphold = 8;
        break;
      case 13:    
        sampshft = 5;
        samphold = 8;
        break;
      case 14:    
        sampshft = 6;
        samphold = 8;
        break;
      case 15:    
        sampshft = 7;
        samphold = 8;
        break; 
    }
  }
}


byte BitCrush_Get_Type(void)
{
  return bitcrush_type;
}


//set pre-filter mode.  this processes the bitcrush before the filter (rather than before distortion)
void BitCrush_Let_PreFilt(boolean newprefilt)
{
  bitcrush_prefilt = newprefilt;
}


boolean BitCrush_Get_PreFilt(void)
{
  return bitcrush_prefilt;
}

//process the wavetable
void BitCrush_Process(void)
{
  byte i;
  sample_t samp = 0;
  for (i=0;i<WAVE_LEN;i++){        //cycle through wavetable
    if (i % samphold == 0){        //every samphold samples
      samp = Wave_Get_Process(i);  //get the wavetable sample
      samp = samp >> sampshft;     //shift sampshft bits, to reduce bit rate
      samp = samp << sampshft;     //shift back again to restore amplitude
    }
    Wave_Let_Process(i, samp);     //write back to wavetable 
  }
}

#endif /* PRODUCT */
