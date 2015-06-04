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

#ifndef __ATMEGATRON_H__
#define __ATMEGATRON_H__

#include "config.h"


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef uint8_t byte;
typedef bool boolean;
typedef uint8_t uchar;
#if PRODUCT==ATMEGATRON
typedef int16_t sample_t;
#else /* PRODUCT */
typedef int8_t sample_t;
#endif /* PRODUCT */

//digital control nums
#define CTRL_FILT  (0)  // Atmegatron & Delayertron
#define CTRL_TEMPO  (0)  // Atmegadrum
#define CTRL_WAVE_CYCLE_LENGTH  (0)  // Atcyclotron
#define CTRL_Q  (1)  // Atmegatron
#define CTRL_TUNE  (1)  // Atmegadrum
#define CTRL_WAVE_CYCLE_LOOP  (1)  // Atcyclotron
#define CTRL_SAMPLE_RATE_ENV  (1)  // Delayertron
#define CTRL_ENV  (2)  // Atmegatron
#define CTRL_LFO_LEVEL  (2)  // Atmegadrum
#define CTRL_WAVE_CYCLE_ENV  (2)  // Atcyclotron
#define CTRL_FEEDBACK  (2)  // Delayertron
#define CTRL_LFO  (3)  // Atmegatron
#define CTRL_LFO_SPEED  (3)  // Atmegadrum
#define CTRL_WAVE_CYCLE_LFO  (3)  // Atcyclotron
#define CTRL_SAMPLE_RATE_LFO  (3)  // Delayertron
#define CTRL_AMP  (4)  // Atmegatron
#define CTRL_DELAY  (4)  // Atmegadrum
#define CTRL_DELAY_SAMPLE_RATE  (4)  // Delayertron
#define CTRL_FX  (5)  // Atmegatron
#define CTRL_VOLUME  (5)  // Atmegadrum
#define CTRL_MIX  (5)  // Delayertron
//function nums
#define FUNC_WAVE  (0)
#define FUNC_FILT  (1)
#define FUNC_FENVA  (2)
#define FUNC_FENVDR  (3)
#define FUNC_FENVS  (4)
#define FUNC_AENVA  (5)
#define FUNC_AENVD  (6)
#define FUNC_AENVS  (7)
#define FUNC_AENVR  (8)
#define FUNC_LFOTYPE  (9)
#define FUNC_LFOSPEED  (10)
#define FUNC_ARPTYPE  (11)
#define FUNC_ARPSPEED  (12)
#define FUNC_PORTA  (13)
#define FUNC_BITCRUSH  (14)
#define FUNC_MEM  (15)        
#define TOT_WAVES  (4)
#define POLL_HARD  (1)        //poll the function/val dials and digital controls
#define POLL_MIDI  (1)        //poll the midi input
//#define POLL_TEST  (1)        //test mode

#define FUNC_MAX  (16)
#define CTRL_MAX  (6)
#define RED  false
#define GREEN  true


#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
#define WAVE_LEN  (32)        //length of wavetable
#define MEM_PATCHSIZE  (19)    //size of flash mem patch
#include "amplitude.h"
#include "arpeggiator.h"
#include "bitcrush.h"
#include "distortion.h"
#include "envelope.h"
#include "envelope2.h"
#include "flange.h"
#include "pwm.h"
#include "testmode.h"
#include "waveforms.h"
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON
#include "filter.h"
#endif /* PRODUCT */

#if PRODUCT==DELAYERTRON
#define WAVE_LEN  (256)        //length of wavetable
#define MEM_PATCHSIZE  (19)    //size of flash mem patch
#include "delay.h"
#include "amplitude.h"
#include "arpeggiator.h"
#include "envelope.h"
#include "envelope2.h"
#include "filter.h"
#include "testmode.h"
#include "waveforms.h"
#endif /* PRODUCT */

#if PRODUCT==ATCYCLOTRON
#include "cycle.h"
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
#define MEM_PATCHSIZE  (32)    //size of flash mem patch
#include "sequencer.h"
#endif /* PRODUCT */

#include "eeprom.h"
#include "hardware.h"
#include "keyboard.h"
#include "lcd.h"
#include "lfo.h"
#include "master.h"
#include "memory.h"
#include "midi.h"
#include "pitch.h"
#include "serial.h"
#include "systick.h"
#include "user_interface.h"
#include "util.h"
#include "wavestore.h"


#endif // __ATMEGATRON_H__
