/*
 * @brief User interface
 *
 * @note
 * Copyright (C) Elektor, 2014
 * All rights reserved.
 *
 * @par
 * This software is supplied "AS IS" without any warranties of any kind,
 * and Elektor and its licensor disclaim any and all warranties, express
 * or implied, including all implied warranties of merchantability,
 * fitness for a particular purpose and non-infringement of intellectual
 * property rights.  Elektor assumes no responsibility or liability for
 * the use of the software, conveys no license or rights under any patent,
 * copyright, mask work right, or any other intellectual property rights in
 * or to any products. Elektor reserves the right to make changes in the
 * software without notification. Elektor also makes no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under Elektor's and its licensor's
 * relevant copyrights in the software, without fee.  This copyright,
 * permission, and disclaimer notice must appear in all copies of this code.
 */

#include "lcd.h"
#include "atmegatron.h"
#include "board.h"


uint8_t display_encoder = 255;
uint8_t display_page = (uint8_t)-1;
boolean display_page_changed = false;


#if PRODUCT==ATMEGADRUM

const char *functions_str[FUNC_MAX][2] =
{
	{ "Kick1", " 0" },
	{ "Kick2", " 1" },
	{ "Kick3", " 2" },
	{ "Snare1", " 3" },
	{ "Snare2", " 4" },
	{ "Snare3", " 5" },
	{ "Snare4", " 6" },
	{ "Clap", " 7" },
	{ "Tom1", " 8" },
	{ "Tom2", " 9" },
	{ "CHH1", "10" },
	{ "CHH2", "11" },
	{ "OHH1", "12" },
	{ "OHH2", "13" },
	{ "Bell", "14" },
	{ "Rim", "15" },
};

// A 1D array declared as a 2D array. The second dimension is for Shifted functions,
// but the Atmegadrum doesn't have these.
const char *encoder_str[8][1] =
{
	{ "Tempo" },
	{ "Tune" },
	{ "LFO level" },
	{ "LFO speed" },
	{ "Delay" },
	{ "Volume" },
	{ "Instrument" },
	{ "Beat" },
};

void display_volume(byte inst);
void display_delay(byte inst);
void display_tempo(void);
void display_tune(void);
void display_lfo_speed(void);
void display_lfo_level(void);
void display_sequencer_pattern(byte inst);

#else /* PRODUCT */

const char *functions_str[FUNC_MAX][2] =
{
#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
	{ "Waveform ", "bank " },
#if PRODUCT==ATMEGATRON
	{ "Filter ", "(norming)" },
#else
	{ "Filter ", "(post)" },
#endif
	{ "Env1 attack", "Inverted" },
	{ "Env1 dec/rel", "Inverted" },
	{ "Env1 sustain", "Inverted" },
#endif
#if PRODUCT==ATCYCLOTRON
	{ "Wave1 bank ", "" },
	{ "Wave2 bank ", "" },
	{ "Wave3 bank ", "" },
	{ "Wave4 bank ", "" },
	{ "C/P Env ", "(inv)" },
#endif
	{ "Env2 attack", "SysEx write" },
	{ "Env2 decay", "SysEx read" },
	{ "Env2 sustain", "User wave" },
	{ "Env2 release", "SysEx bank" },
	{ "LFO shape ", "(inv)" },
	{ "LFO rate ", "" },
	{ "Arpeggio ", "(pp)" },
	{ "Arpeggio rate", "" },
	{ "Portamento", "Proportional" },
#if PRODUCT==DELAYERTRON
	{ "Delay", "Clipping" },
#else
	{ "Crusher ", "pre-filter" },
#endif
	{ "Patch number", "Empty" },
};


const char *wave_str[FUNC_MAX][2] =
{
#if PRODUCT==ATMEGATRON
	{ "Pure square", "Metallic 1" },
	{ "Octave square", "Metallic 2" },
	{ "Square fifths", "Metallic 3" },
	{ "RP2A07 square", "Metallic 4" },
	{ "Pure saw", "Vocal 1" },
	{ "Buzz saw", "Vocal 2" },
	{ "Saw fifths", "Vocal 3" },
	{ "Octave saw", "Brass" },
	{ "Subsine + square", "Reed organ" },
	{ "Pure sine", "Electric piano" },
	{ "PPG sine", "Reed" },
	{ "Warped sine", "Resonant saw" },
	{ "CZ101 pulse", "Bell" },
	{ "Bassoon", "Chord" },
	{ "Bass", "Overtones" },
	{ "Noise", "Saw thirds" },
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
	{ "Pure square", "Metallic 1" },
	{ "Pure saw", "Metallic 2" },
	{ "Pure sine", "Metallic 3" },
	{ "Pure triangle", "Metallic 4" },
	{ "IE 1", "Vocal 1" },
	{ "IE 2", "Vocal 2" },
	{ "Quack", "Vocal 3" },
	{ "Inharmonic 1", "Brass" },
	{ "Inharmonic 2", "Reed organ" },
	{ "PWM", "Electric piano" },
	{ "Trash", "Reed" },
	{ "Evil", "Resonant saw" },
	{ "Moog (ish) 1", "Bell" },
	{ "Moog (ish) 2", "Chord" },
	{ "Pokey", "Overtones" },
	{ "Noise", "Saw thirds" },
#endif /* PRODUCT */
};


#if PRODUCT==ATMEGATRON
const char *filter_str[FUNC_MAX] =
{
	"Bypassed",
	"Low pass",
	"High pass",
	"Band pass",
	"Notch",
	"Param EQ 10 dB",
	"Param EQ 30 dB",
	"Param EQ 100 dB",
	"LO shelf 10 dB",
	"LO shelf 30 dB",
	"HI shelf 10 dB",
	"HI shelf 30 dB",
	"Butter LP (no Q)",
	"Butter HP (no Q)",
	"Bessel LP (no Q)",
	"Bessel HP (no Q)",
};
#endif /* PRODUCT */
#if PRODUCT==DELAYERTRON
const char *filter_str[FUNC_MAX] =
{
	"Bypassed",
	"Low pass",
	"High pass",
	"-3-",
	"-4-",
	"-5-",
	"-6-",
	"-7-",
	"-8-",
	"-9-",
	"-A-",
	"-B-",
	"-C-",
	"-D-",
	"-E-",
	"-F-",
};
#endif /* PRODUCT */


const char *lfo_str[FUNC_MAX] =
{
	"Sine",
	"Triangle",
	"Ramp down (lin)",
	"Ramp up (exp)",
	"Sine + 3rd",
	"Pulse  5%",
	"Pulse 25%",
	"Pulse 50%",
	"Pulse 75%",
	"Pulse 95%",
	"Ramp up & hold",
	"Hold & ramp down",
	"Duh dugga",
	"Dugga duh",
	"Noise-like",
	"DC",
};


const char *arp_str[FUNC_MAX] =
{
	"Off",
	"Up",
	"Down",
	"Octave",
	"Fifths",
	"Skip one up",
	"Skip one down",
	"I feel love",
	"Horton",
	"Etheridge",
	"Meads",
	"Palmer",
	"gwEm 1",
	"gwEm 2",
	"Triplet up",
	"Robyn",
};

const char *rate_str[FUNC_MAX] =
{
	"4",
	"2",
	"1",
	"3/4",
	"2/3",
	"1/2",
	"3/8",
	"1/3",
	"1/4",
	"3/16",
	"1/8",
	"3/32",
	"1/16",
	"3/64",
	"1/32",
	"1/64",
};


const char *crusher_str[FUNC_MAX] =
{
	"Off",
	"3 bit",
	"2 bit",
	"1 bit",
	"4 bit, SR/2",
	"3 bit, SR/2",
	"2 bit, SR/2",
	"1 bit, SR/2",
	"4 bit, SR/4",
	"3 bit, SR/4",
	"2 bit, SR/4",
	"1 bit, SR/4",
	"4 bit, SR/8",
	"3 bit, SR/8",
	"2 bit, SR/8",
	"1 bit, SR/8",
};

#endif /* PRODUCT */


/*const char *controls_str[CTRL_MAX][2] =
{
	// Digital controls (potentiometers)
	{ "Filt cutoff", "Filt cutoff" },
	{ "Filter Q", "Filter Q" },
	{ "Filter env", "Pitch env" },
	{ "Filter LFO", "Pitch LFO" },
	{ "Amp LFO", "PW LFO" },
	{ "Distortion", "Phase LFO" },
};*/

//void display_show_help(void);

void display_init(void)
{
	// Setup LCD.
	lcd_init();
	display_page_set(PAGE_SPLASH,255);
}


uint8_t display_print_value(uint8_t line, uint8_t position, int value, boolean ignore_sign)
{
	int scale = 100;
	uint8_t i = 0;

	lcd_clear_to_eol(line,position);
	if (ignore_sign==false)
	{
		if (value<0)
		{
			lcd_putc('-');
			value = -value;
		}
		else lcd_putc(' ');
		i++;
	}

	if (value<100)
	{
		lcd_putc(' ');
		scale = 10;
		i++;
	}
	if (value<10)
	{
		lcd_putc(' ');
		scale = 1;
		i++;
	}

	do
	{
		int temp = value/scale;
		lcd_putc(temp+'0');
		value -= temp*scale;
		scale /= 10;
		i++;
	}
	while (scale>0);

	return i;
}


uint8_t display_print_value_f(uint8_t line, uint8_t position, float value, uint8_t decimals)
{
	uint8_t p, sign = 0;
	if (value<0.0)
	{
		value *= -1.0;
		sign = 1;
	}
	p = display_print_value(line,position,(int)value,false);
	value -= (int)value;
	lcd_putc('.');
	p++;
	while (p+position<16 && decimals!=0)
	{
		value -= (int)value;
		value *= 10.0;
		lcd_putc((int)value+'0');
		p++;
		decimals--;
	}
	if (sign)
	{
		lcd_cursor_move(line,2);
		lcd_putc('-');
	}
	return p;
}

extern float a0;
extern float a1;
extern float a2;
extern float b0;
extern float b1;
extern float b2;
void display_debug(void);

void display_debug(void)
{
	lcd_clear();
	display_print_value_f(0,0,a0,10);
	display_print_value_f(1,0,a1,10);
	SysTick_Delay(2500);
	lcd_clear();
	display_print_value_f(0,0,a2,10);
	display_print_value_f(1,0,b0,10);
	SysTick_Delay(2500);
	lcd_clear();
	display_print_value_f(0,0,b1,10);
	display_print_value_f(1,0,b2,10);
	SysTick_Delay(2500);
}


void display_page_splash(void)
{
	lcd_clear();
	lcd_font(ST7032_FUNC_HEIGHT_DOUBLE);
	lcd_puts("J2B Synthesizer");
	SysTick_Delay(1500);
	lcd_font(ST7032_FUNC_HEIGHT_NORMAL);
	lcd_clear();
	lcd_cursor_move(0,0);
	lcd_puts(PRODUCT_NAME);
	lcd_cursor_move(1,0);
	lcd_puts(FIRMWARE_VERSION);
	SysTick_Delay(1500);
	/*lcd_clear();
	lcd_cursor_move(0,0);
	lcd_puts(__DATE__);
	SysTick_Delay(1500);*/
	display_page_set(PAGE_FUNCTION,255);
}

#if PRODUCT==ATMEGADRUM

void display_volume(byte inst)
{
	uint8_t value = 7-Seq_Get_Volume(inst);
	lcd_cursor_move(0,14);
	lcd_putc('v');
	lcd_putc(value+'0');
	lcd_clear_to_eol(1,0);
	lcd_puts((char*)encoder_str[display_encoder][0]);
	display_print_value(1,6,value,true);
}

void display_delay(byte inst)
{
	uint8_t value = Seq_Get_DelayBeats(inst);
	lcd_cursor_move(0,11);
	lcd_putc('d');
	lcd_putc(value+'0');
	lcd_clear_to_eol(1,0);
	lcd_puts((char*)encoder_str[display_encoder][0]);
	display_print_value(1,5,value,true);
}

void display_tempo(void)
{
	uint8_t value = Seq_Get_Tempo();
	lcd_clear_to_eol(1,0);
	lcd_puts((char*)encoder_str[display_encoder][0]);
	display_print_value(1,6,value,true);
}

void display_tune(void)
{
	uint8_t value = map(Pitch_Get_CurFreq(),master_minocr1,16000,0,255);
	lcd_clear_to_eol(1,0);
	lcd_puts((char*)encoder_str[display_encoder][0]);
	display_print_value(1,5,value,true);
}

extern const uint16_t speed_store[16];

void display_lfo_speed(void)
{
	uint8_t value = 0;
	while (LFO_Get_Speed()!=speed_store[value]) value++;
	lcd_clear_to_eol(1,0);
	lcd_puts((char*)encoder_str[display_encoder][0]);
	display_print_value(1,9,value,true);
}

void display_lfo_level(void)
{
	uint8_t value = Pitch_Get_LFOAmt();
	lcd_clear_to_eol(1,0);
	lcd_puts((char*)encoder_str[display_encoder][0]);
	display_print_value(1,10,value,true);
}

void display_sequencer_pattern(byte inst)
{
	int i;
	//byte curpos = Seq_Get_Current_Position();
	uint8_t value = Hardware_Get_Value();

	lcd_cursor(1,0);
	for (i=0; i<SEQ_LEN; i++)
	{
		if (i==value)
		{
			// Show beat insert/delete position
			if (Seq_Get_Pattern(inst,i)==false) lcd_putc(6); // Empty beat.
			else lcd_putc(7); // Special char at beat position
		}
		else
		{
			if (Seq_Get_Pattern(inst,i)==false) lcd_putc(0); // Show empty beat.
			else lcd_putc(3); // Show beat.
		}
	}
	// Set cursor at play position.
	//lcd_cursor(1,curpos);
}

void display_page_function(void)
{
	static uint8_t function = 255;
	if (function!=Hardware_Get_Function())
	{
		lcd_clear();
		function = Hardware_Get_Function();
	}
	lcd_puts((char*)functions_str[function][0]);
	display_volume(function);
	display_delay(function);
	if (display_page==PAGE_FUNCTION)
	{
		display_sequencer_pattern(function);
	}
	else
	{
		display_page_ctrl();
	}
}

#else /* PRODUCT */

void display_page_function(void)
{
	uint8_t function = Hardware_Get_Function();
	boolean shift = Hard_Get_Shift(function);
	uint8_t value = Hardware_Get_Value(function);

	lcd_clear();
	if (function>=FUNC_MAX)
	{
		lcd_puts((char*)"undefined");
	}
	else
	{
		lcd_puts((char*)functions_str[function][0]);
		switch (function)
		{
#if PRODUCT==ATCYCLOTRON
			extern const byte envA_preset[16];
			extern const byte envDR_preset[16];
			extern const byte envS_preset[16];

			case FUNC_FENVS:
			{
				char str[10];
				if (shift==GREEN) lcd_puts((char*)functions_str[function][1]);
				lcd_cursor(1,0);
				lcd_puts("A=");
				lcd_puts(itoa(envA_preset[value],str,10));
				if (envA_preset[value]<10) lcd_putc(' ');
				lcd_puts(" DR=");
				lcd_puts(itoa(envDR_preset[value],str,10));
				if (envDR_preset[value]<10) lcd_putc(' ');
				lcd_puts(" S=");
				lcd_puts(itoa(envS_preset[value],str,10));
			}
			break;

			case FUNC_FILT:
			case FUNC_FENVA:
			case FUNC_FENVDR:
#endif /* PRODUCT */
#if PRODUCT!=DELAYERTRON
			case FUNC_WAVE:
				lcd_puts((char*)functions_str[function][1]);
				if (shift==GREEN) lcd_putc('2');
				else lcd_putc('1');
				lcd_cursor(1,0);
				lcd_puts((char*)wave_str[value][shift]);
				break;
#else
			case FUNC_WAVE:
				display_print_value(0,FUNCTION_VALUE_OFFSET,value+1,true);
				lcd_cursor(1,0);
				lcd_puts("Bank ");
				if (shift==GREEN) lcd_putc('2');
				else lcd_putc('1');
				break;
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
			case FUNC_FILT:
				if (shift==GREEN) lcd_puts((char*)functions_str[function][1]);
				lcd_cursor(1,0);
				lcd_puts((char*)filter_str[value]);
				break;
#endif /* PRODUCT */

			case FUNC_LFOTYPE:
				if (shift==GREEN) lcd_puts((char*)functions_str[function][1]);
				lcd_cursor(1,0);
				lcd_puts((char*)lfo_str[value]);
				break;

			case FUNC_LFOSPEED:
			case FUNC_ARPSPEED:
				if (shift==GREEN) lcd_puts((char*)functions_str[function][1]);
				lcd_cursor(1,0);
				lcd_puts((char*)rate_str[value]);
				break;

			case FUNC_ARPTYPE:
				if (shift==GREEN) lcd_puts((char*)functions_str[function][1]);
				lcd_cursor(1,0);
				lcd_puts((char*)arp_str[value]);
				break;

#if PRODUCT!=DELAYERTRON
			case FUNC_BITCRUSH:
				if (shift==GREEN) lcd_puts("(pre)");
				else lcd_puts("(post)");
				lcd_cursor(1,0);
				lcd_puts((char*)crusher_str[value]);
				break;
#endif

			case FUNC_MEM:
			{
				uint8_t buffer[MEM_PATCHSIZE];
				display_print_value(0,FUNCTION_VALUE_OFFSET,value+1,true);
				if (Memory_Load_Patch(buffer,value*MEM_PATCHSIZE)==false)
				{
					lcd_cursor(1,0);
					lcd_puts((char*)functions_str[function][1]);
				}
				break;
			}

			default:
				display_print_value(0,FUNCTION_VALUE_OFFSET,value,true);
				if (shift==GREEN)
				{
					lcd_cursor(1,0);
					lcd_puts((char*)functions_str[function][1]);
				}
		}
	}
}

#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM

void display_page_ctrl(void)
{
	uint8_t instrument = Hardware_Get_Function();

	switch (display_encoder)
	{
		case CTRL_TEMPO:
			display_tempo();
			break;

		case CTRL_TUNE:
			display_tune();
			break;

		case CTRL_LFO_LEVEL:
			display_lfo_level();
			break;

		case CTRL_LFO_SPEED:
			display_lfo_speed();
			break;

		case CTRL_DELAY:
			display_delay(instrument);
			break;

		case CTRL_VOLUME:
			display_volume(instrument);
			break;
	}
}

#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON

/*void display_page_ctrl(void)
{
	uint8_t i, column, value;
	uint8_t dec;
	uint8_t shift = Hardware_Get_Ctrl_Shift();

	lcd_clear();

	for (i=0; i<6; i++)
	{
		column = 3*(encoder_to_ctrl[i+2]-2);
		value = Hardware_Get_Ctrl(shift,i);
		if ((i==CTRL_AMP || i==CTRL_FX) && shift!=0)
		{
			lcd_cursor(1,column-1);
			value >>= 3;
			dec = value/10;
			if (dec>0)
			{
				value -= 10*dec;
				lcd_putc(dec+'0');
			}
			else lcd_putc(' ');
			lcd_putc(value+'0');
		}
		else if (i==CTRL_FX && shift==0)
		{
			lcd_cursor(1,column);
			lcd_putc((value>>5)+'0');
		}
		else
		{
			lcd_bar_graph(column,value,255);
		}
	}
}*/

void display_page_ctrl(void)
{
	uint8_t shift = Hardware_Get_Ctrl_Shift();
	uint8_t value = Hardware_Get_Ctrl(shift,display_encoder);

	lcd_clear_to_eol(1,0);

	switch (display_encoder)
	{
		case CTRL_FILT:
			lcd_puts("Cutoff");
			break;

		case CTRL_Q:
			lcd_puts("Resonance");
			break;

		case CTRL_ENV:
			if (shift==0) lcd_puts("Filter ENV");
			else lcd_puts("Pitch ENV");
			break;

		case CTRL_LFO:
			if (shift==0) lcd_puts("Filter LFO");
			else lcd_puts("Pitch LFO");
			break;

		case CTRL_AMP:
			if (shift==0) lcd_puts("Amp LFO");
			else lcd_puts("PW LFO");
			break;

		case CTRL_FX:
			if (shift==0) lcd_puts("Distortion");
			else lcd_puts("Phase LFO");
			break;
	}

	display_print_value(1,FUNCTION_VALUE_OFFSET,value,true);
}

#endif /* PRODUCT */

#if PRODUCT==ATCYCLOTRON

void display_page_ctrl(void)
{
	//uint8_t function = Hardware_Get_Function();
	uint8_t shift = Hardware_Get_Ctrl_Shift();
	uint8_t value = Hardware_Get_Ctrl(shift,display_encoder);

	lcd_clear_to_eol(1,0);

	switch (display_encoder)
	{
		case CTRL_WAVE_CYCLE_LENGTH:
			lcd_puts("Cycle length");
			break;

		case CTRL_WAVE_CYCLE_LOOP:
			lcd_puts("Cycle loop");
			break;

		case CTRL_WAVE_CYCLE_ENV:
			if (shift==0) lcd_puts("Cycle env.");
			else lcd_puts("Pitch env.");
			break;

		case CTRL_WAVE_CYCLE_LFO:
			if (shift==0) lcd_puts("Cycle LFO");
			else lcd_puts("Pitch LFO");
			break;

		case CTRL_AMP:
			if (shift==0) lcd_puts("Amp. LFO");
			else lcd_puts("PW LFO");
			break;

		case CTRL_FX:
			if (shift==0) lcd_puts("Distortion");
			else lcd_puts("Phase LFO");
			break;
	}

	display_print_value(1,FUNCTION_VALUE_OFFSET,value,true);
}

#endif /* PRODUCT */

#if PRODUCT==DELAYERTRON
void display_page_ctrl(void)
{
	uint8_t shift = Hardware_Get_Ctrl_Shift();
	uint8_t value = Hardware_Get_Ctrl(shift,display_encoder);

	lcd_clear_to_eol(1,0);

	switch (display_encoder)
	{
		case CTRL_FILT:
			if (shift==0) lcd_puts("Cutoff");
			else lcd_puts("Filter LFO");
			break;

		case CTRL_Q:
			if (shift==0) lcd_puts("SR ENV");
			else lcd_puts("Pitch ENV");
			break;

		case CTRL_ENV:
			lcd_puts("Feedback");
			break;

		case CTRL_LFO:
			if (shift==0) lcd_puts("SR LFO");
			else lcd_puts("Pitch LFO");
			break;

		case CTRL_AMP:
			lcd_puts("Delay SR");
			break;

		case CTRL_FX:
			lcd_puts("Dry/wet mix");
			break;
	}

	display_print_value(1,FUNCTION_VALUE_OFFSET,value,true);
}

#endif /* PRODUCT */

boolean display_page_set(uint8_t page, uint8_t encoder)
{
	display_page_changed = display_page!=page? true : false;
	display_page = page;
	//display_encoder = encoder_to_ctrl[encoder];
	display_encoder = encoder;
	return display_page_changed;
}

/*boolean display_page_set(uint8_t page)
{
	display_page_changed = display_page!=page? true : false;
	display_page = page;
	return display_page_changed;
}*/


uint8_t display_page_get(void)
{
	return display_page;
}


void display_invalidate(void)
{
	display_page_changed = true;
}


/*void display_show_help(void)
{
	if (display_encoder<255)
	{
		lcd_cursor(0,0);
		lcd_puts((char*)encoder_help_str[display_encoder][0]);
		display_encoder = 255;
	}
}*/


void display_draw(boolean force_redraw)
{
	if (display_page_changed==true || force_redraw==true)
	{
		display_page_changed = false;
		switch (display_page)
		{
			case PAGE_SPLASH:
				display_page_splash();
				break;

			case PAGE_FUNCTION:
				display_page_function();
				break;

			case PAGE_CTRL:
#if PRODUCT==ATMEGADRUM
				display_page_function();
#else /* PRODUCT */
				//display_debug();
				display_page_ctrl();
#endif /* PRODUCT */
				break;
		}
	}
}
