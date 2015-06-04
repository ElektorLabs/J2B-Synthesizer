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

#ifndef __MIDI_H__
#define __MIDI_H__


//frequency of all midi notes  (rounded to nearest integer)
extern const uint16_t MIDI_freqs[128];

//midi constants
#define NOTE_OFF 128
#define NOTE_ON 144
#define PITCH_WHEEL 224
#define CONTROLLER 176
#define SYSEXBEGIN 240
#define SYSEXEND 247
#define CC_PITCHLFO 1
#define CC_PORTAMENTO 5
#define CC_FILTERENV 16
#define CC_DISTORTION 17
#define CC_FILTCUTOFF 74 
#define CC_AMPENVR 72
#define CC_AMPENVA 73
#define CC_FILTRES 71
#define CC_AMPENVD 75
#define CC_LFOCLOCKDIV 79
#define CC_PWM 91
#define CC_AMPLFO 92
#define CC_FILTLFO 93
#define CC_PITCHENV 94
#define CC_FLANGE 95
#define CC_ALLNOTESOFF 123

#if PRODUCT==ATMEGADRUM
#define MIDI_ACBASSDRUM 35
#define MIDI_BASSDRUM1 36
#define MIDI_STICK 37
#define MIDI_ACSNARE 38
#define MIDI_HANDCLAP 39
#define MIDI_ELECSNARE 40
#define MIDI_LOWFLOORTOM 41
#define MIDI_CLHIHAT 42
#define MIDI_HIGHFLOORTOM 43
#define MIDI_PEDHIHAT 44
#define MIDI_LOWTOM 45
#define MIDI_OPENHIHAT 46
#define MIDI_LOWMIDTOM 47
#define MIDI_HIMIDTOM 48
#define MIDI_CRASH 49
#define MIDI_COWBELL 56
#endif /* PRODUCT */

//sysex message types, used by librarian software
#define SYSEX_PATCH 0
#define SYSEX_WAVE 1
#define SYSEX_MEM 2
#define SYSEX_CALLPATCH 3
#define SYSEX_CALLWAVE 4

#if PRODUCT==ATMEGADRUM
boolean MIDI_Get_ClockPresent(void);
void MIDI_NoteOn(byte notenum, byte velocity);
#else /* PRODUCT */
byte MIDI_Get_curNote(void);
boolean MIDI_Get_KeyDown(byte notenum);
void MIDI_Let_ClockArpSpeed(unsigned int newspeed);
boolean MIDI_Get_ClockPresent(void);
#if PRODUCT==ATMEGATRON
	void MIDI_Set_Channel(byte newchannel);
#endif
void MIDI_NoteOn(byte notenum);
void MIDI_NoteOff(byte notenum);
void MIDI_TriggerNote(byte notenum);
void MIDI_Reset(void);
void MIDI_TestButtonDown(void);
void MIDI_TestButtonUp(void);
void MIDI_TestButtonInc(void);
void MIDI_TestButtonDec(void);
char MIDI_Get_PitchBend_Level(void);
#if PRODUCT!=DELAYERTRON
	unsigned int MIDI_Get_Freq(byte notenum);
	void MIDI_Let_SYSEXRead(boolean newval);
	void MIDI_SYSEX_read(byte databyte);
	void MIDI_SYSEX_write_patch(void);
#endif /* PRODUCT */
#endif /* PRODUCT */

void MIDI_Init(void);
void MIDI_ClockStart(void);
void MIDI_ClockContinue(void);
void MIDI_ClockStop(void);
void MIDI_ClockTick(void);
unsigned long MIDI_Get_ClockTick(void);
void MIDI_Poll(void);


#endif // __MIDI_H__
