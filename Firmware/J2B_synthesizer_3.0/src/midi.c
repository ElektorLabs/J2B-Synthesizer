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


unsigned int MIDI_clocktick=0;
boolean MIDI_clockpresent=false;              //is midi clock present?


#if PRODUCT==ATMEGADRUM

const byte MIDI_notemap[16] =
{
	MIDI_BASSDRUM1,
	MIDI_ACBASSDRUM,
	MIDI_LOWFLOORTOM,
	MIDI_ACSNARE,
	MIDI_ELECSNARE,
	MIDI_HIGHFLOORTOM,
	MIDI_LOWTOM,
	MIDI_HANDCLAP,
	MIDI_LOWMIDTOM,
	MIDI_HIMIDTOM,
	MIDI_CLHIHAT,
	MIDI_PEDHIHAT,
	MIDI_OPENHIHAT,
	MIDI_CRASH,
	MIDI_COWBELL,
	MIDI_STICK
};

byte incomingByte=0;
byte notebyte=0;
byte velocitybyte=0;
byte statusbuffer=0;
boolean firstbyte;

boolean MIDI_Get_ClockPresent(void)
{
  return MIDI_clockpresent;
}


void MIDI_NoteOn(byte notenum, byte velocity)
{
	byte i;
	for (i=0; i<16; i++)
	{
		if (notenum==MIDI_notemap[i])
		{
			Seq_Let_Amplitude(i,7-(velocity>>4));
			master_index[i] = Wavestore_Get_Start(i);
		}
	}
}

void MIDI_Poll(void)
{
	// TODO cpv : strange if-do-while construction, replace by while-do?
  if (Serial.available()>0)
  {
    do
    {
      // Read the incoming byte:
      incomingByte = Serial.read();
      if (incomingByte>247)
      {
        // This is where MIDI clock stuff is done.
        switch (incomingByte)
        {
          case 248:
            MIDI_ClockTick();
            break;

          case 250:
            MIDI_ClockStart();
            break;

          case 251:
            MIDI_ClockContinue();
            break;

          case 252:
            MIDI_ClockStop();
            break;
        }
      }
      else if (incomingByte>127)
      {
        statusbuffer = incomingByte;
        firstbyte = true;
        notebyte = 0;
        velocitybyte = 0;
      }
      else if (statusbuffer!=0)
      {
        if (firstbyte==true)
        {
          // Must be first byte.
          notebyte = incomingByte;
          firstbyte = false;
        }
        else
        {
          // So must be second byte then.
          velocitybyte = incomingByte;
          // Process the message here.
          if (statusbuffer==NOTE_ON && velocitybyte!=0)
          {
            Hardware_LED_StartFlash(1,1);
            MIDI_NoteOn(notebyte,velocitybyte);
          }
          // Now clear them for next note.
          notebyte = 0;
          velocitybyte = 0;
          firstbyte = true;
        }
      }
    }
    while (Serial.available()>0);
  }
}

#else /* PRODUCT */

#if PRODUCT!=DELAYERTRON

//frequency of all midi notes  (rounded to nearest integer)
const uint16_t MIDI_freqs[128] =
{
	 8, 9, 9,10,10,11,12,12,13,14,15,15,16,17,18,19,
	21,22,23,24,26,28,29,31,33,35,37,39,41,44,46,49,
	52,55,58,62,65,69,73,78,82,87,92,98,104,110,117,123,
	131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,
	330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,
	831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,
	2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,4435,4699,4978,
	5274,5588,5920,6272,6645,7040,7459,7902,8372,8870,9397,9956,10548,11175,11840,12544
};

boolean gettingSYSEX=false;                   //in the middle of getting sysex message
boolean memDumping=false;                     //in the middle of dumping memory to librarian software
byte curMemDump=0;                            //current stage of memory dump
byte SYSEXbytenum=0;                          //byte number of sysex message
byte SYSEXmesstype=0;                         //type of sysex message
boolean MIDI_SYSEXread=false;                 //is sysex read enabled?
#endif

boolean arp_triggernext=false;                //trigger arpeggiator note on next midi clock

//lets and gets
byte MIDI_curnote = 57;                       //current note pressed
byte MIDI_curbutnote = 60;                    //current note of test button
boolean MIDI_keydown[128];                    //store which keys are down for arpeggiator
char MIDI_totnoteson =0;                      //total notes held down 
char MIDI_lasttotnoteson = 0;                 //last total notes held down
unsigned int MIDI_clockticksperarpstep=12;
unsigned int MIDI_clocknextarp=0;
char MIDI_pbend_level=0; 
#if PRODUCT==ATMEGATRON
byte MIDI_channel=0;
#endif /* PRODUCT */

byte MIDI_Get_curNote(void)
{                            //get current midi note
  return MIDI_curnote;
}


#if PRODUCT!=DELAYERTRON
unsigned int MIDI_Get_Freq(byte notenum)
{          //get current midi frequency (potentially different to actual current frequency in pitch tab)
  return MIDI_freqs[notenum];
}
#endif


boolean MIDI_Get_KeyDown(byte notenum)
{            //find out if note is down
  return MIDI_keydown[notenum];
}


void MIDI_Let_ClockArpSpeed(unsigned int newspeed)
{                            //set clock arp speed.  not actual speed.  see Arp_Let_Speed for how this works
  MIDI_clockticksperarpstep = ((unsigned long)newspeed+1) * 24 >> 13;
}


#if PRODUCT!=DELAYERTRON
void MIDI_Let_SYSEXRead(boolean newval)
{          //set sysex read mode
  MIDI_SYSEXread = newval;
}
#endif

//boolean MIDI_Get_SYSEXRead(void)
//{
//  return MIDI_SYSEXread;
//}


boolean MIDI_Get_ClockPresent(void)
{                   //return if midi clock is preset
  return MIDI_clockpresent;
}


#if PRODUCT==ATMEGATRON
void MIDI_Set_Channel(byte newchannel)
{
	if (newchannel!=MIDI_channel)
	{
		MIDI_channel = newchannel;
		Memory_Channel_Write(newchannel);
	}
}
#endif /* PRODUCT */


void MIDI_NoteOn(byte notenum)
{
  MIDI_curnote = notenum;                                    //set current note = new note
  MIDI_lasttotnoteson = MIDI_totnoteson;                     //set last total notes on
  MIDI_totnoteson++;                                         //increment total notes on
  MIDI_keydown[notenum] = true;                              //set midi key down = true
  if (Arp_Get_Type()==0)
  {                                    //if arpeggiator off then
    MIDI_TriggerNote(notenum);                               //trigger note
  }
  else if (MIDI_lasttotnoteson==0 && MIDI_totnoteson==1) {   //if arpeggiator on and this is the first note pressed down
    Arp_Reset();                                             //reset arpeggiator and lfo, so they sync together       
    LFO_Reset();          
  }
}


//release key
void MIDI_NoteOff(byte notenum)
{
  MIDI_keydown[notenum] = false;                             //set midi key down = false
  MIDI_lasttotnoteson = MIDI_totnoteson;                     //set last total notes on
  MIDI_totnoteson--;                                         //decrement total notes on
  if (MIDI_totnoteson<=0){                                   //if total notes on = 0 (ie all keys released)  (use <= for safety)
    AEnv_Release();                                          //release amplitude envelope
    FEnv_Release();                                          //release filter envelope
    if (MIDI_totnoteson<0){                                  //safety - if tot notes on < 0
      MIDI_totnoteson = 0;                                   //reset to 0 (<0 not possible)
    }
  }
}


//*********TRIGGER A NOTE**************** (used by arpeggiator too)
void MIDI_TriggerNote(byte notenum)
{
#if PRODUCT==DELAYERTRON
	Pitch_Let_NextFreq(notenum);                //set next frequency (pitch)
#else
	Pitch_Let_NextFreq(MIDI_freqs[notenum]);                //set next frequency (pitch)
#endif
	Pitch_ResetPorta();                                                       //set portamento start tick
	AEnv_Trigger();                                                           //trigger amplitude envelope
	FEnv_Trigger();                                                           //trigger filter envelope
}


//reset all notes
void MIDI_Reset(void)
{
	byte i;
	for (i=0; i<=127; i++)
	{
		MIDI_NoteOff(i);
	}
}


//press the test button (value knob)
void MIDI_TestButtonDown(void)
{
#if PRODUCT==DELAYERTRON
	Pitch_Let_NextFreq(MIDI_curbutnote);       //set next frequency
#else
	Pitch_Let_NextFreq(MIDI_freqs[MIDI_curbutnote]);       //set next frequency
#endif
	Pitch_ResetPorta();                                                      //set portamento start tick
	AEnv_Trigger();                                                          //trigger amplitude envelope
	FEnv_Trigger();                                                          //trigger filter envelope
}


//release the test button (value knob)
void MIDI_TestButtonUp(void)
{
  if (MIDI_totnoteson<=0){                                                 //prevent stuck notes
    AEnv_Release();                                                        //release amplitude envelope
    FEnv_Release();                                                        //release filter envelope
  }  
}


//increment test button note (clockwise turn)
void MIDI_TestButtonInc(void)
{
	if (MIDI_curbutnote<126)
	{                                                //if cur test button note < max midi note
		MIDI_curbutnote++;                                                     //increment
#if PRODUCT==DELAYERTRON
		Pitch_Let_NextFreq(MIDI_curbutnote);     //set next frequency (pitch)
#else
		Pitch_Let_NextFreq(MIDI_freqs[MIDI_curbutnote]);     //set next frequency (pitch)
#endif
	}
}


//decrement test button note (anti clockwise turn)
void MIDI_TestButtonDec(void)
{
#if PRODUCT==DELAYERTRON
	if (MIDI_curbutnote>0)
	{                                                  //if cur test button note > min midi note
		MIDI_curbutnote--;                                                     //decrement
		Pitch_Let_NextFreq(MIDI_curbutnote);     //set next frequency (pitch)
	}
#else
	if (MIDI_curbutnote>1)
	{                                                  //if cur test button note > min midi note
		MIDI_curbutnote--;                                                     //decrement
		Pitch_Let_NextFreq(MIDI_freqs[MIDI_curbutnote]);     //set next frequency (pitch)
	}
#endif
}


//get pitch bend  (only using msb of pitch bend data)
char MIDI_Get_PitchBend_Level(void)
{
#if PRODUCT==DELAYERTRON
	if (MIDI_pbend_level>0)
	{
		return MIDI_pbend_level+1;
	}
	else
#endif
	{
		return MIDI_pbend_level;
	}
}


#if PRODUCT!=DELAYERTRON

//***********SYSEX stuff***********
//byte of sysex data received
void MIDI_SYSEX_read(byte databyte)
{
  //int addr;
  int samp;

  switch (SYSEXbytenum){                                          //SYSEXbytenum - counter used to log position in sysex message
    case 1:                                                       //Manufactuer ID 125 (one day I'll get my own!)
      if (databyte!=125){
        gettingSYSEX = false;
        Hardware_LED_SetState(4,LOW);
      }
      break;
    case 2:                                                       //Product ID   (0 for Atmegatron)
      if (databyte!=0){
        gettingSYSEX = false;
        Hardware_LED_SetState(4,LOW);
      }
      break;
    case 3:                                                      //Message type ID (0 = patch, 1 = waveform, 2 = mem dump)
      SYSEXmesstype = databyte;
      break;
    default:
      if (SYSEXmesstype==SYSEX_PATCH && MIDI_SYSEXread==true){   //********Patch********  
        if (SYSEXbytenum>=4 && SYSEXbytenum<=19){                //set relevent value (decided by SYSEXbytenum (position in message))
          Hardware_Let_Value(SYSEXbytenum-4,databyte);
        }
        if (SYSEXbytenum>=20 && SYSEXbytenum<=25){
          Hardware_Let_Ctrl(0,SYSEXbytenum-20,databyte<<1);      //values that are byte (0-255) are *2, because sysex max value is 127
        }      
        if (SYSEXbytenum>=26 && SYSEXbytenum<=29){
          Hardware_Let_Ctrl(1,SYSEXbytenum-24,databyte<<1);
        }
        if (SYSEXbytenum==30){
          Hard_Let_Shift(FUNC_WAVE,databyte);
        }
        if (SYSEXbytenum==31){
          Hard_Let_Shift(FUNC_FILT,databyte);
        }
        if (SYSEXbytenum==32){
          Hard_Let_Shift(FUNC_FENVA,databyte);
        }
        if (SYSEXbytenum==33){
          Hard_Let_Shift(FUNC_LFOTYPE,databyte);
        }
        if (SYSEXbytenum==34){
          Hard_Let_Shift(FUNC_ARPTYPE,databyte);
        }
        if (SYSEXbytenum==35){
          Hard_Let_Shift(FUNC_PORTA,databyte);
        }
        if (SYSEXbytenum==36){                                   //this is the last byte
          Hard_Let_Shift(FUNC_BITCRUSH, databyte);
          Hardware_LED_StartFlash(0,5);                          //Flash function knob led.  this means all bytes received (different to end of sysex message (F7))
          if(memDumping==true){                                  //****if this is part of a mem dump receive:****
            Serial.flush();                                      //flush serial buffer - need all the mem clear for next message
            Memory_Save(curMemDump);                             //save current received patch to flash mem
            curMemDump++;                                        //increment mem dump counter (patch num to request)
            Serial.write(SYSEXBEGIN);                            //0 begin sysex message
            Serial.write(125);                                   //1 manufacturer ID
            Serial.write(0);                                     //2 product ID
            if (curMemDump<16){                                  //first 16 dumps are patches, if more patches to recieve:  request next patch
              Serial.write(SYSEX_CALLPATCH);                     //3 message type
            }
            else{                                                //or request first user wave
              Serial.write(SYSEX_CALLWAVE);                      //3 message type
              curMemDump = 0;                                    //reset counter for user waves
            }
            Serial.write(curMemDump);                            //4 mem dump number (so librarian knows which patch/wave)
            Serial.write(SYSEXEND);                              //end message
          }
        }
      }    
      
      if (SYSEXmesstype==SYSEX_WAVE && MIDI_SYSEXread==true){    //***********User Wave**************
        if (SYSEXbytenum>=4 && SYSEXbytenum<=35){                //bytes of sample data
          samp = (int)databyte;                                  //convert to integer
          samp *= 2;                                             //*2 (librarian outputs (-63 - 63), we need (-127 - 128, actually use -126 - 126)
          samp -= 126;                                           //apply offset (byte transmitted as positive integers)        
          Wave_Let_UserWave(SYSEXbytenum-4,(signed char)samp);          //convert to char and set current user wave
        }
        if (SYSEXbytenum==35){                                   //last byte of data
          Hardware_LED_StartFlash(0,5);                          //Flash function knob led.  this means all bytes received (different to end of sysex message (F7))
          if(memDumping==true){                                  //****if this is part of a mem dump receive:****
            Serial.flush();                                      //flush serial buffer - need all the mem clear for next message
            Memory_UserWave_Write(curMemDump);                   //save current received user wave to flash mem
            curMemDump++;                                        //increment mem dump counter (user wave to request)
            if (curMemDump<6){                                   //if more user waves still to receive:  request next user wave
              Serial.write(SYSEXBEGIN);                          //0 begin message
              Serial.write(125);                                 //1 manufacturer ID
              Serial.write(0);                                   //2 product ID
              Serial.write(SYSEX_CALLWAVE);                      //3 message type
              Serial.write(curMemDump);                          //4 mem dump number (so librarian knows which patch/wave)
              Serial.write(SYSEXEND);                            //end message
            }
            else{                                                //finished final user wave
              memDumping = false;                                //stop dumping
            }
          }
        }
      }
      
      if (SYSEXmesstype==SYSEX_MEM && MIDI_SYSEXread==true){     //*************Memory Dump**************
        curMemDump = 0;                                          //counter used to track which patch/wave is being dumped
        memDumping = true;                                       //memory dump started
        Serial.write(SYSEXBEGIN);                                //request first patch from librarian
        Serial.write(125);                                       //1 manufacturer ID
        Serial.write(0);                                         //2 product ID
        Serial.write(SYSEX_CALLPATCH);                           //3 message type
        Serial.write(curMemDump);                                //4 mem dump number (patch 0)
        Serial.write(SYSEXEND);                                  //end message        
      }
  }
}


//Write patch to librarian
void MIDI_SYSEX_write_patch(void)
{
  byte data;
  byte i;
  //boolean b;
  
  Serial.write(SYSEXBEGIN);
  Serial.write(125);                          //  1  manufacturer ID
  Serial.write(0);                            //  2  product ID
  Serial.write(SYSEX_PATCH);                  //  3  message type
  for (i=0;i<16;i++){                         //  4-19  function vals
    Serial.write(Hardware_Get_Value(i));
  }
  for (i=0;i<6;i++){                          //  20-25  ctrl vals bank=0  
    data =  Hardware_Get_Ctrl(0, i) >> 1;
    Serial.write(data); 
  }
  for (i=2;i<6;i++){                          //  26-29  ctrl vals bank=1
    data =  Hardware_Get_Ctrl(1, i) >> 1;
    Serial.write(data); 
  }
  Serial.write(Hard_Get_Shift(FUNC_WAVE));    //30-36 shift mode for functions
  Serial.write(Hard_Get_Shift(FUNC_FILT));
  Serial.write(Hard_Get_Shift(FUNC_FENVA));
  Serial.write(Hard_Get_Shift(FUNC_LFOTYPE));
  Serial.write(Hard_Get_Shift(FUNC_ARPTYPE));
  Serial.write(Hard_Get_Shift(FUNC_PORTA));
  Serial.write(Hard_Get_Shift(FUNC_BITCRUSH));
  Serial.write(SYSEXEND);                     //37 end message
}

#endif /* PRODUCT */

//**************MIDI POLL ********************* 
void MIDI_Poll(void)
{
  byte incomingByte=0;   
  static byte notebyte=0;        //these are all static because there's a small chance that the subroutine is exited before the 2 bytes of MIDI needed for note_on are received.
  static byte velocitybyte=0;
  static byte statusbuffer=0;
  static boolean firstbyte;
  if (Serial.available() > 0) {                    //if there is something in the serial input buffer
    do {
      incomingByte = Serial.read();                // read the incoming byte:
      if (incomingByte>247) {                      // ****this is where MIDI clock stuff is done***
        switch (incomingByte){
          case 248:                                //MIDI clock tick
            MIDI_ClockTick();
            break;
          case 250:                                //MIDI clock start
            MIDI_ClockStart();
            break;
          case 251:                                //MIDI clock continue (not implemented)
            MIDI_ClockContinue();
            break;
          case 252:                                //MIDI clock stop
            MIDI_ClockStop();
            MIDI_Reset();
            break;
        }
      }
#if PRODUCT!=DELAYERTRON
      else if (incomingByte>239) {                 //*****this is where SYSEX stuff is done*****
        statusbuffer = 0;
        if (incomingByte==SYSEXBEGIN){             //start of sysex message
          gettingSYSEX = true;
          SYSEXbytenum = 0;                        //reset message byte counter
          Hardware_LED_SetState(4,HIGH);           //turn midi light on while receiving
        }
        if (incomingByte==SYSEXEND){               //end of sysex message
          gettingSYSEX = false; 
          Hardware_LED_SetState(4,LOW);            //turn midi light off (this doesn't guarantee that message was successfully read though!  Func knob flashes for that)
        }
      }
#endif
      else if (incomingByte>127) {                 //*****this is where all other message types are done ******
        statusbuffer = incomingByte;               //status buffer stores what kind of buffer it is
        firstbyte = true;                          //it must be the first byte of pair
        notebyte = 0;                              //so reset the note byte
        velocitybyte = 0;                          //and velocity byte, ready incase it's a note
#if PRODUCT!=DELAYERTRON
        gettingSYSEX = false;                      //safety - note a sysex message
      }
      else if (gettingSYSEX==true){                //****SYSEX data*******  if getting sysex, it HAS to be sysex data if  < 128
        SYSEXbytenum++;                            //inc message byte counter
        MIDI_SYSEX_read(incomingByte);             //process the incoming byte
#endif
      }
      else if (statusbuffer!=0) {                  //******all other message data*******
        if (firstbyte == true) {                   //if it's first byte of pair
          notebyte = incomingByte;                 //set note = incoming byte
          firstbyte = false;                       //and set = false, so ready for second byte
        }
        else
        {                                     // so must be second byte of pair then
          velocitybyte = incomingByte;                                                            //set velocitybyte as incoming byte (even if not note data)
#if PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
          if (statusbuffer == NOTE_ON && velocitybyte != 0)                                     //is it a note on?
#endif /* PRODUCT */
#if PRODUCT==ATMEGATRON
          if (statusbuffer == (NOTE_ON | MIDI_channel) && velocitybyte != 0)                                     //is it a note on?
#endif /* PRODUCT */
          {
        	Hardware_LED_StartFlash(4,1);                                                         //flash midi led
            MIDI_NoteOn(notebyte);                                                                //trigger note
          }
#if PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
          else if (statusbuffer == NOTE_OFF || (statusbuffer == NOTE_ON && velocitybyte == 0))  //is it a note off?  (there's 2 ways to trigger note off:  note_off or Note_on with velocity =0)
#endif /* PRODUCT */
#if PRODUCT==ATMEGATRON
          else if (statusbuffer == (NOTE_OFF | MIDI_channel) || (statusbuffer == (NOTE_ON | MIDI_channel) && velocitybyte == 0))  //is it a note off?  (there's 2 ways to trigger note off:  note_off or Note_on with velocity =0)
#endif /* PRODUCT */
          {
        	Hardware_LED_StartFlash(4,1);                                                         //flash midi led
            MIDI_NoteOff(notebyte);                                                               //release note
          }
#if PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
          else if (statusbuffer == PITCH_WHEEL)                                                  //is it pitch wheel?
#endif /* PRODUCT */
#if PRODUCT==ATMEGATRON
          else if (statusbuffer == (PITCH_WHEEL | MIDI_channel))                                                  //is it pitch wheel?
#endif /* PRODUCT */
          {
        	  MIDI_pbend_level = velocitybyte - 64;                                                 //set pbend (may aswell do the offset here).  only msb of pitch bend used
          }
#if PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
          else if (statusbuffer == CONTROLLER)                                                   //is it a controller message?  I have attempted to use controller numbers appoximating the spec of official controller list.  Used "MISC" controllers where necessary
#endif /* PRODUCT */
#if PRODUCT==ATMEGATRON
          else if (statusbuffer == (CONTROLLER | MIDI_channel))                                                   //is it a controller message?  I have attempted to use controller numbers appoximating the spec of official controller list.  Used "MISC" controllers where necessary
#endif /* PRODUCT */
          {
        	switch (notebyte)
        	{
              case CC_PITCHLFO:                                                                   //pitch lfo (mod wheel).  This is just an override of the pitch lfo knob
                Hardware_Let_Ctrl(1,CTRL_LFO,velocitybyte >> 1);                                  //default uses >>1 for typical mod wheel vibrato.  use  << 1 for full range (default full range = 1 octave)
                break;
#if PRODUCT!=DELAYERTRON
              case CC_PORTAMENTO:                                                                 //portamento
                Hardware_Let_Value(FUNC_PORTA,velocitybyte >> 3);                                 //scale to values 0-15
                break;
              case CC_FILTERENV:                                                                  //filter envelope
                Hardware_Let_Ctrl(0,CTRL_ENV,velocitybyte << 1);                                  //scale to values 0-255
                break;
              case CC_DISTORTION:                                                                 //distortion
                Hardware_Let_Ctrl(0,CTRL_FX,velocitybyte << 1);                                   //scale to values 0-255
                break;
              case CC_FILTCUTOFF:                                                                 //filter cutoff frequency
                Hardware_Let_Ctrl(0,CTRL_FILT,velocitybyte << 1);                                 //scale to values 0-255
                break;
              case CC_AMPENVR:                                                                    //amplitude envelope release
                Hardware_Let_Value(FUNC_AENVR,velocitybyte >> 3);                                 //scale to values 0-15
                break;
              case CC_AMPENVA:                                                                    //amplitude envelope attack
                Hardware_Let_Value(FUNC_AENVA,velocitybyte >> 3);                                 //scale to values 0-15
                break;
              case CC_FILTRES:                                                                    //filter resonance
                Hardware_Let_Ctrl(0,CTRL_Q,velocitybyte << 1);                                    //scale to values 0-255
                break;
              case CC_AMPENVD:                                                                    //amplitude envelope decay
                Hardware_Let_Value(FUNC_AENVD,velocitybyte >> 3);                                 //scale to values 0-15
                break;
              case CC_LFOCLOCKDIV:                                                                //lfo rate (speed).    ******DUBSTEP PRODUCERS THIS IS CONTROLLER 79!!!********
                Hardware_Let_Value(FUNC_LFOSPEED,velocitybyte >> 3);                              //scale to values 0-15 
                break;
              case CC_PWM:                                                                        //pulse width modulation
                Hardware_Let_Ctrl(1,CTRL_AMP,velocitybyte << 1);                                  //scale to values (0-255)
                break;
              case CC_AMPLFO:                                                                     //amplitude lfo
                Hardware_Let_Ctrl(0,CTRL_AMP,velocitybyte << 1);                                  //scale to values (0-255)
                break;
              case CC_FILTLFO:                                                                    //filter lfo
                Hardware_Let_Ctrl(0,CTRL_LFO,velocitybyte << 1);
                break;
              case CC_PITCHENV:                                                                   //pitch envelope
                Hardware_Let_Ctrl(1,CTRL_ENV,velocitybyte << 1);
                break;
              case CC_FLANGE:                                                                     //phaser
                Hardware_Let_Ctrl(1,CTRL_FX,velocitybyte << 1);
                break;
#endif
              case CC_ALLNOTESOFF:
				MIDI_Reset();
            }
          }                                                                       
          notebyte = 0;                                                                           //now reset them ready for next note
          velocitybyte = 0; 
          firstbyte = true;         
        }
      }
    }
    while (Serial.available() > 0);                                                             //loop until serial buffer is empty

    if (arp_triggernext==true && MIDI_totnoteson>0){                                              //midi clock tick was received and it's time to fire an arpeggiator note!
      do{                                                                                         //nasty  do loop.  this is important, incase midi clock ticks are faster than Atmegatron polling loop and multiple notes need to be triggered now
        Arp_TriggerStep();                                                                        //trigger arpeggiator
        MIDI_clocknextarp += MIDI_clockticksperarpstep;                                           //MIDI_clockticksperarpstep  MUST never be 0
      } while (MIDI_clocknextarp<MIDI_clocktick);                                                 //loop until all notes triggered that need to be are
      arp_triggernext = false;                                                                    //triggered all notes, so set false
    }
  } 
}

#endif /* PRODUCT */


//MIDI meat
//initialise serial port
void MIDI_Init(void)
{
  // Enable MIDI.
#if PRODUCT==ATMEGTRON
  MIDI_channel = Memory_Channel_Read();
#endif
  Serial.begin(31250);
}


//*******MIDI clock stuff********
//MIDI clock start received
void MIDI_ClockStart(void)
{
  MIDI_clockpresent = true;       //set clock present
  MIDI_clocktick = 0;             //reset clock tick counter
#if PRODUCT==ATMEGADRUM
  Seq_Let_Playing(true,false);
#else /* PRODUCT */
  MIDI_clocknextarp = 0;          //reset next arpeggiator note tick
  if (Arp_Get_Type()!=0)
  {        //if arpeggiator on
    Arp_Reset();                  //reset it (so synced with clock)
  }
  LFO_Reset();                    //reset lfo (so synced with clock)
#endif /* PRODUCT */
}


void MIDI_ClockContinue(void)
{
#if PRODUCT==ATMEGADRUM
  MIDI_clockpresent = true;
  Seq_Let_Playing(true,true);
#else /* PRODUCT */
  // No implementation of clock continue at present, just does clock start.
  MIDI_ClockStart();
#endif /* PRODUCT */
}


// MIDI clock stop received.
void MIDI_ClockStop(void)
{
  MIDI_clockpresent = false;  // Set clock not present.
#if PRODUCT==ATMEGADRUM
  Seq_Let_Playing(false,false);
#endif /* PRODUCT */
}


//MIDI clock tick received
void MIDI_ClockTick(void)
{
  if (MIDI_clockpresent==true)
  {
#if PRODUCT!=ATMEGADRUM
    // Check clock has actually been started (not rogue data).
    if (MIDI_clocktick>=MIDI_clocknextarp && Arp_Get_Type()!=0)
    {
      // If MIDI clock counter > MIDI clock value for next arpeggiator trigger (and arpeggiator on) :
      arp_triggernext = true;  // Trigger arpeggiator at end of current MIDI receive (bottom of MIDI_poll).
    }
#endif /* PRODUCT */
    if (MIDI_clocktick%24 == 0)
    {
      // Every 24 clock ticks (every beat):
      Hardware_LED_StartFlash(4,1);  //flash the MIDI led.
    }
    MIDI_clocktick++;  // Increment MIDI clock counter.
  }

}


//get midi clock counter
unsigned long MIDI_Get_ClockTick(void)
{
  return MIDI_clocktick;
}


