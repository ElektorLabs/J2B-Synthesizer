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

#include "atmegatron.h"

#if PRODUCT==ATMEGADRUM
byte mem_loadonbeat = 255;

void Memory_Let_LoadOnBeat(byte newval)
{
  mem_loadonbeat = newval;
}

byte Memory_Get_LoadOnBeat(void)
{
  return mem_loadonbeat;
}
#endif /* PRODUCT */


//save patch
void Memory_Save(byte patchnum)
{
  byte i;
  byte lsb, msb;
  uint32_t addr;
  
#if PRODUCT==ATMEGADRUM
  uint16_t tmp;
#else /* PRODUCT */
  byte anal, out;
  byte bit_position;
#endif /* PRODUCT */
  
  cli();                                    //halt interrupts (i.e. audio engine).  don't want it interfering with eeprom writing
  
  addr = (uint32_t)patchnum * MEM_PATCHSIZE;          //find start address  (normal patch size = 19 bytes)

#if PRODUCT==ATMEGADRUM
  //sadly only enough room for 16 patterns :(
  for (i=0; i<16; i++)
  {
    tmp = Seq_Get_PatternRaw(i);
    lsb = ((tmp >> 0) & 0xff);
    msb = ((tmp >> 8) & 0xff);
    EEPROM.write(addr,lsb);
    addr += 1;
    EEPROM.write(addr,msb);
    addr += 1;
  }
#else

  /*v = 0;
  
  for (i=0;i<8;i++){                        //writes all 16 function values to memory
    lsb = Hardware_Get_Value(v);            //each function is value 0-15 = 4 bit value.
    msb = Hardware_Get_Value(v+1);          //so can store 2 function values in each byte (8 bits)
    msb = msb << 4;                         //this is done by bit-shifting every other value left 4 bits
    out = msb | lsb;                        //and ORing with other value
    EEPROM.write(addr+i, out);              //then write to eeprom
    v += 2;
  }*/

  // Write all 16 function values to memory
  for (i=0; i<8; i++)
  {
    lsb = Hardware_Get_Value(2*i);            //each function is value 0-15 = 4 bit value.
    msb = Hardware_Get_Value(2*i+1);          //so can store 2 function values in each byte (8 bits)
    EEPROM.write(addr,((msb<<4)&0xf0)|(lsb&0x0f));  //then write to eeprom
    addr += 1;
  }

  //write all ctrl values to memory
  anal = Hardware_Get_Ctrl(0, CTRL_FILT);   //first the 6 red bank ctrls
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(0, CTRL_Q);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(0, CTRL_ENV);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(0, CTRL_LFO);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(0, CTRL_AMP);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(0, CTRL_FX);
  EEPROM.write(addr, anal);
  addr += 1;
#if PRODUCT==DELAYERTRON
  anal = Hardware_Get_Ctrl(1, CTRL_FILT);    //then the 4 green bank ctrls
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(1, CTRL_Q);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(1, CTRL_LFO);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(1, CTRL_AMP); // dummy
#else
  anal = Hardware_Get_Ctrl(1, CTRL_ENV);    //then the 4 green bank ctrls
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(1, CTRL_LFO);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(1, CTRL_AMP);
  EEPROM.write(addr, anal);
  addr += 1;
  anal = Hardware_Get_Ctrl(1, CTRL_FX);
#endif
  EEPROM.write(addr, anal);
  addr += 1;
  
                                            //write function shift modes to memory.   
  out = 0;                                  //these are boolean, so 1 bit each.  so 8 can be store in each byte (i.e. 8 bits).  out is used to build up output byte
  bit_position = 0x01;
  if (Hard_Get_Shift(FUNC_WAVE)==true){     //get each shift mode and set/clear each bit of out.   First wavetable bank
    out |= bit_position;
  }
  bit_position <<= 1;
  if (Hard_Get_Shift(FUNC_FILT)==true){    //filter normalise mode
    out |= bit_position;
  }
  bit_position <<= 1;
  if (Hard_Get_Shift(FUNC_FENVA)==true){   //filt/pitch env invert mode
    out |= bit_position;
  }
  bit_position <<= 1;
  if (Hard_Get_Shift(FUNC_LFOTYPE)==true){  //lfo invert mode
    out |= bit_position;
  }
  bit_position <<= 1;
  if (Hard_Get_Shift(FUNC_ARPTYPE)==true){  //arp ping-pong mode
    out |= bit_position;
  }
  bit_position <<= 1;
  if (Hard_Get_Shift(FUNC_PORTA)==true){    //proportional portamento
    out |= bit_position;
  }
  bit_position <<= 1;
  if (Hard_Get_Shift(FUNC_BITCRUSH)==true){  //bitcrush before filter mode
    out |= bit_position;
  }
  EEPROM.write(addr, out);                //write compiled bits to eeprom
  addr += 1;

#endif /* PRODUCT */
  sei();                                     //restart interrupts (audio engine)
  
}


bool Memory_Load_Patch(uint8_t *p_dst, uint32_t src)
{
	int i;
	bool patch_not_empty = false;
	for (i=0; i<MEM_PATCHSIZE; i++)
	{
		// We use special EEPROM library that doesn't care about interrupts.
		p_dst[i] = EEPROM.read(src+i);
		if (p_dst[i]!=0 && p_dst[i]!=0xff)
		{
			patch_not_empty = true;
		}
	}
	return patch_not_empty;
}


//load patch
#if PRODUCT==ATMEGADRUM
bool Memory_Load(byte patchnum)
{
	byte i, lsb, msb;
	uint16_t addr, tmp;

	addr = patchnum * MEM_PATCHSIZE;
	//16 patterns
	for (i=0; i<16; i++)
	{
		lsb = EEPROM.read(addr);
		msb = EEPROM.read(addr+1);
		tmp = ((lsb<<0) & 0xff) + ((msb<<8) & 0xff00);
		Seq_Let_PatternRaw(i,tmp);
		addr += 2;
	}
	return true;
}

#else

bool Memory_Load(byte patchnum)
{
  int i;
  byte in;
  uint8_t eeprom_buffer[MEM_PATCHSIZE];
  
  if (Memory_Load_Patch(eeprom_buffer,patchnum*MEM_PATCHSIZE)==false) return false;

   //set function values
  for (i=0; i<8; i++)
  {
    in = eeprom_buffer[i];
    // Two function values in each byte. First one can be obtained by blanking top 4 bits.
    Hardware_Let_Value(2*i,in&0x0f);
    Hardware_Let_Value(2*i+1,(in>>4)&0x0f);
  }

  Hardware_Let_Ctrl(0, CTRL_FILT, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(0, CTRL_Q, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(0, CTRL_ENV, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(0, CTRL_LFO, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(0, CTRL_AMP, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(0, CTRL_FX, eeprom_buffer[i++]);
#if PRODUCT==DELAYERTRON
  Hardware_Let_Ctrl(1, CTRL_FILT, eeprom_buffer[i++]);      //the the 4 green bank ctrls
  Hardware_Let_Ctrl(1, CTRL_Q, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(1, CTRL_LFO, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(1, CTRL_AMP, eeprom_buffer[i++]);
#else
  Hardware_Let_Ctrl(1, CTRL_ENV, eeprom_buffer[i++]);      //the the 4 green bank ctrls
  Hardware_Let_Ctrl(1, CTRL_LFO, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(1, CTRL_AMP, eeprom_buffer[i++]);
  Hardware_Let_Ctrl(1, CTRL_FX, eeprom_buffer[i++]);
#endif

  in = eeprom_buffer[i++];                 //read entire byte
  Hard_Let_Shift(FUNC_WAVE,(in&0x01)!=0);          //and set wavetable bank accordingly
  Hard_Let_Shift(FUNC_FILT,(in&0x02)!=0);
  Hard_Let_Shift(FUNC_FENVA,(in&0x04)!=0);         //filter/pitch env invert mode
  Hard_Let_Shift(FUNC_LFOTYPE,(in&0x08)!=0);       //lfo invert mode
  Hard_Let_Shift(FUNC_ARPTYPE,(in&0x10)!=0);        //arpeggiator pingpong
  Hard_Let_Shift(FUNC_PORTA,(in&0x20)!=0);          //proportional portamento
  Hard_Let_Shift(FUNC_BITCRUSH,(in&0x40)!=0);       //filter before bit crusher

  return true;
}

#if PRODUCT!=DELAYERTRON

//***** User wave ******
//save user wave
void Memory_UserWave_Write(byte patchnum)
{
  int i, addr;
  cli();                                                   //stop interrupts (audio engine)
  addr = ((int)patchnum * WAVE_LEN) + (MEM_PATCHSIZE*16);  //calc start address. 288 bytes is the offset from 16 mem patches
  for (i=0;i<WAVE_LEN;i++){
    EEPROM.write(addr + i, Wave_Get_UserWave(i));          //write each sample to memory
  }  
  sei();                                                   //restart interrupts (audio engine)
}


//load user wave
void Memory_UserWave_Read(byte patchnum)
{
  int i, addr;
  cli();                                                   //stop interrupts (audio engine)
  addr = ((int)patchnum * WAVE_LEN) + (MEM_PATCHSIZE*16);  //calc start address. 288 bytes is the offset from 16 mem patches
  for (i=0;i<WAVE_LEN;i++){
    Wave_Let_UserWave(i,EEPROM.read(addr + i));            //read each sample value from memory
  }
  sei();                                                   //restart interrupts (audio engine)
}

#endif

//initialise atmegatron with vanilla patch
void Memory_Vanilla(void)
{
  Hardware_Let_Value(FUNC_WAVE, 0);        //square wave
  Hardware_Let_Value(FUNC_FILT, 1);        //lpf
#if PRODUCT==ATCYCLOTRON
  Hardware_Let_Value(FUNC_FENVA, 2);
  Hardware_Let_Value(FUNC_FENVDR, 3);
  Hardware_Let_Value(FUNC_FENVS, 2);
#else /* PRODUCT */
  Hardware_Let_Value(FUNC_FENVA, 0);       //'donk' shape filter env
  Hardware_Let_Value(FUNC_FENVDR, 6);    
  Hardware_Let_Value(FUNC_FENVS, 0);
#endif /* PRODUCT */
  Hardware_Let_Value(FUNC_AENVA, 0);       //full sustain amp env with short decay if sustain lowered
  Hardware_Let_Value(FUNC_AENVD, 6);
  Hardware_Let_Value(FUNC_AENVS, 15);
  Hardware_Let_Value(FUNC_AENVR, 0);
  Hardware_Let_Value(FUNC_LFOTYPE, 0);     //sine LFO
  Hardware_Let_Value(FUNC_LFOSPEED, 8);    //semi-quavers
  Hardware_Let_Value(FUNC_ARPTYPE, 0);     //arp off
  Hardware_Let_Value(FUNC_ARPSPEED, 5);    //quavers
#if PRODUCT==DELAYERTRON
  Hardware_Let_Value(FUNC_PORTA, 4);       //porta short
  Hardware_Let_Value(FUNC_BITCRUSH, 15);   //longest delay time
  Hardware_Let_Ctrl(0,CTRL_FILT,255);      //filter full open
  Hardware_Let_Ctrl(1,CTRL_FILT,0);        //filter lfo off
  Hardware_Let_Ctrl(0,CTRL_Q,0);           //samp freq env off
  Hardware_Let_Ctrl(1,CTRL_Q,0);           //pitch env off
  Hardware_Let_Ctrl(0,CTRL_ENV,120);       //Feedback = 10
  Hardware_Let_Ctrl(0,CTRL_LFO,0);		   //no sf lfo
  Hardware_Let_Ctrl(1,CTRL_LFO,0);		   //no pitch lfo
  Hardware_Let_Ctrl(0,CTRL_AMP,64);		   //highish samp freq
  Hardware_Let_Ctrl(0,CTRL_FX,80);         //little bit of delay
#else
  Hardware_Let_Value(FUNC_PORTA, 0);       //porta off   
  Hardware_Let_Value(FUNC_BITCRUSH, 0);    //bitcrush off  
#if PRODUCT==ATCYCLOTRON
  Hardware_Let_Ctrl(0,CTRL_FILT,0);
#else /* PRODUCT */
  Hardware_Let_Ctrl(0,CTRL_FILT,255);      //filter full open
#endif /* PRODUCT */
  Hardware_Let_Ctrl(0,CTRL_Q,0);           //resonance at min (0.5)
  Hardware_Let_Ctrl(0,CTRL_ENV,0);         //All Envs and LFO amts = 0
  Hardware_Let_Ctrl(1,CTRL_ENV,0);  
  Hardware_Let_Ctrl(0,CTRL_LFO,0);
  Hardware_Let_Ctrl(1,CTRL_LFO,0);
  Hardware_Let_Ctrl(0,CTRL_AMP,0);
  Hardware_Let_Ctrl(1,CTRL_AMP,0);
  Hardware_Let_Ctrl(0,CTRL_FX,0);          //no distortion 
  Hardware_Let_Ctrl(1,CTRL_FX,0);          //no phaser
#endif
  Hard_Let_Shift(FUNC_WAVE, false);        //all shift modes off   
  Hard_Let_Shift(FUNC_FILT, false); 
  Hard_Let_Shift(FUNC_FENVA, false);
  Hard_Let_Shift(FUNC_LFOTYPE, false);
  Hard_Let_Shift(FUNC_ARPTYPE, false);
  Hard_Let_Shift(FUNC_PORTA, false);
  Hard_Let_Shift(FUNC_BITCRUSH, false);
}

#if PRODUCT!=DELAYERTRON

//read patch from memory and output as sysex
void Memory_SYSEX_write_mem(void)
{
  byte data;
  int i;
  
  Serial.write(SYSEXBEGIN);
  Serial.write(125);    //  1  manufacturer ID
  Serial.write(0);      //  2  product ID
  Serial.write(SYSEX_MEM);      //  3  message type
  for (i=0;i<496;i++){  //496 bytes of the mem used
    cli();
    data = EEPROM.read(i);
    sei();
    Serial.write(data >> 1);  //MSB
    Serial.write(data & 0x01);  //LSB
    Serial.flush();
  }
  Serial.write(SYSEXEND);              
  
}


void Memory_Channel_Write(byte channel)
{
  EEPROM.write(511, channel);
}


byte Memory_Channel_Read(void)
{
	byte ch;
	ch = EEPROM.read(511);
	if (ch>15)
	{
		return 0;
	}
	else
	{
		return ch;
	}
}

#endif
#endif /* PRODUCT */
