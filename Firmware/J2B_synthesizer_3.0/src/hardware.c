#include "atmegatron.h"
#include "lcd.h"


//const uint16_t bg_select_store[16] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};              //values used to light single led of led array
//const uint16_t bg_fill_store[16] = {1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535};            //values used to fill leds between 0-x of led array
const uint16_t envADR_store[16] = {0,8,23,48,88,151,245,381,574,845,1217,1722,2399,3297,4476,6013};               //times used for attack, decay and release of envelopes
const uchar envS_store[16] = {0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,255};                          //levels used for sustain of envelope1 (unipolar for amplitude)
const signed char fenvS_store[16] = {0,8,17,25,34,42,51,59,68,76,85,93,102,110,119,127};                                 //levels used for sustain of envelope2 (bipolar for filter)
const uint16_t speed_store[16] = {32767,16383,8191,6143,5460,4095,3071,2730,2047,1535,1023,767,511,383,255,127}; //values used for speeds of LFO and arpeggiator
const uint16_t porta_store[16] = {0,5,14,27,46,77,123,195,306,477,740,1145,1768,2728,4207,6484};                  //values for portamento speeds

//lets and gets (not all letable or getable at pres)
#if PRODUCT==ATMEGADRUM

#define LOCKOUT 1000        //time between ctrl stop moving and locking it out
#define FLASHLONGTIME 300
#define FLASHSHORTTIME 150
#define MAXVAL 15
byte hard_func = 0;
byte hard_val = 0;
byte hard_ctrl[6] = {0,0,0,0,0,0};
boolean hard_shift = false;
unsigned long flashtick = 0;
boolean flashon = false;

#else /* PRODUCT */

#define HOLDLENGTH 2000     //time required to hold function button for hold functions (e.g. patch save)
#define LOCKOUT 3000        //time between ctrl stop moving and locking it out
byte hard_func = 255;         //current selected function. 255 to allow reset in Init_Hardware
byte hard_val[16];      //current select value for each function.
byte hard_ctrl[2][6];         //current value for each digital control [bank][control]
boolean hard_shift[16];       //current value of shift for each function (red/green mode)

#endif /* PRODUCT */

//long lastval;  //last value of value rotary encoder
byte maxval = 15;  //maximum value that is selectable on the value dial (normally 15 apart from user wave mode)
boolean testbutdown = false;  //test button (value encoder button)
byte hard_curctrlbank = 0;        //current value of ctrl bank (red/green mode of controls)
byte hard_curctrl = 1;            //current control being read.  Only 1 control is read in each pass.  read order:  0,1,0,2,0,3,0,4 etc, so cutoff is smooth as poss
byte hard_nextctrl = 1;           //sets next control to be read after cutoff.
byte lastfuncbut = 0;     //last value of function button input
byte lastvalbut = 0;      //last value of value button input
byte lastbank = 0;        //last value of ctrl bank button input
byte lastctrl[6];                 //last value read of each ctrl
// CPV - movingctrl and stopmovingtick don't seem to have a function anymore.
//boolean movingctrl[6];            //is a ctrl moving (i.e. is user turning it)
//unsigned long stopmovingtick[6];  //tick when ctrl will lock out (to stop jitter) assuming user has stopped moving it
unsigned long butstarttick = 0;   //tick when button was pressed
unsigned long ctrlstarttick = 0;   //tick when control was used
boolean finishedhold = false;     //used by function button for function that require 2 sec hold (e.g. patch save)
#if PRODUCT!=DELAYERTRON
boolean MIDIchannelmode = false;
#endif

void Hardware_Adjust_Value(byte func, byte val);
void Hardware_Adjust_Shift(byte func, boolean val);
boolean IsFuncFill(byte func);
boolean IsShiftHold(byte func);
int Hardware_Ctrl_Update(byte shift, byte index, int value);
void Hardware_reset_moving_ctrl(void);


// Here you can remap the rotary encoders in case you don't like
// the way they are set up.
//#define LEFT_HANDED
// 0 <-> 1
// 2 <-> 7
// 3 <-> 6
// 4 <-> 5
uint8_t encoder_to_ctrl[8] =
{
#if PRODUCT==ATMEGADRUM
#if defined LEFT_HANDED
	0, // function encoder
	1, // value encoder
	7, // tempo
	6, // tune
	5, // lfo speed
	4, // lfo level
	3, // delay
	2  // volume
#else
	0, // function encoder
	1, // value encoder
	2, // tempo
	3, // tune
	4, // lfo speed
	5, // lfo level
	6, // delay
	7  // volume
#endif // LEFT_HANDED
#endif /* PRODUCT */
#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
#if defined LEFT_HANDED
	0, // function encoder
	1, // value encoder
	2, // filter cut-off
	7, // filter resonance
	3, // filter envelope (red) / pitch envelope (green)
	4, // filter lfo (red) / pitch lfo (green)
	5, // amplitude lfo (red) / pulse width lfo (green)
	6  // distortion (red) / phase lfo (green)
#else
	0, // function encoder
	1, // value encoder
	7, // filter cut-off
	2, // filter resonance
	6, // filter envelope (red) / pitch envelope (green)
	5, // filter lfo (red) / pitch lfo (green)
	4, // amplitude lfo (red) / pulse width lfo (green)
	3  // distortion (red) / phase lfo (green)
#endif // LEFT_HANDED
#endif /* PRODUCT */
#if PRODUCT==DELAYERTRON
#if defined LEFT_HANDED
	0, // function encoder
	1, // value encoder
	3, // Cutoff (red) / Filter envelope (green)
	4, // Sample rate envelope (red) / Pitch envelope (green)
	7, // Feedback
	5, // Sample rate lfo (red) / pitch lfo (green)
	6, // Delay sample rate
	2  // Dry/wet mix
#else // LEFT_HANDED
	0, // function encoder
	1, // value encoder
	6, // Cutoff (red) / Filter envelope (green)
	5, // Sample rate envelope (red) / Pitch envelope (green)
	2, // Feedback
	4, // Sample rate lfo (red) / pitch lfo (green)
	3, // Delay sample rate
	7  // Dry/wet mix
#endif // LEFT_HANDED
#endif /* PRODUCT */
};

#define ENCODER_FUNCTION  encoder_to_ctrl[0]
#define ENCODER_VALUE  encoder_to_ctrl[1]


//Hardware setup, only run once
void Init_Hardware(void)
{
	byte i;
	//int ctrlread;

#if PRODUCT==ATMEGADRUM
	hard_shift = false;
#else /* PRODUCT */
#if PRODUCT==ATMEGATRON
	byte newctrl;
#endif
	memset(hard_shift,0,sizeof(hard_shift)); // Set everything to RED.
#endif /* PRODUCT */

	Hardware_Let_Function(0);       //initialise function dial to 0

	//Hardware_LED_SetState(3,HIGH);  //initialise value encoder LED to red
	//Hardware_LED_SetState(2,LOW);
	Hardware_BicolorLED_SetState(2,2,1);

	Hardware_Refresh_ValBG();       //refresh value dial LED array

	Hardware_Let_CtrlBank(0);       //initialise ctrl bank to 0 (red mode)

#if PRODUCT==ATMEGATRON
	//READ FUNCTION ENCODER BUTTON to see if setting MIDI channel number
	newctrl = keyboard_get_pushbutton(ENCODER_FUNCTION,false);
	// If function button down, then MIDI channel mode.
	if (newctrl>0)
	{
		MIDIchannelmode = true;
		//initialise value encoder LED to yellow (woo)
		//Hardware_LED_SetState(3,HIGH);
		//Hardware_LED_SetState(2,HIGH);
		Hardware_BicolorLED_SetState(2,1,1);
		Hardware_Let_Value(FUNC_MEM,Memory_Channel_Read());
	}
#endif /* PRODUCT */

	for (i=0; i<=5; i++)
	{
		//ctrlread = analogRead(i);     //initialise lastctrl array.  use analogread, coz speed not important
		//newctrl = ctrlread >> 2;      //all ctrls are stored as byte (0-255) not ints
		//lastctrl[i] = newctrl;
		lastctrl[i] = 128; // TODO: should get these values from EEPROM.
	}

	hard_curctrl = 0;
}


//*************POLL HARDWARE***********************
void Hardware_Poll(void)
{
	int newval;
	int newfunc;
	byte newbut, newfuncbut, newvalbut;
	unsigned long butcurtick;
	int temp;

	// Read Function encoder (the left encoder)
	newfunc = keyboard_get_rotary_encoder_value(ENCODER_FUNCTION);
	if (newfunc!=0)
	{
		// Grab display focus if needed.
		// If the page must change first to get focus, the rotary encoder
		// value will be read again the next time the hardware is polled.
		if (display_page_set(PAGE_FUNCTION,7)==0)
		{
			// Page is already being displayed, use the encoder value.
			newfunc += hard_func;
			if (newfunc>15) newfunc = 0;
			else if (newfunc<0) newfunc = 15;
			Hardware_Let_Function(newfunc);
		}
		// Eat rotary encoder value.
		keyboard_set_rotary_encoder_value(ENCODER_FUNCTION,0);
	}

	// Read Value encoder (the right encoder)
	newval = keyboard_get_rotary_encoder_value(ENCODER_VALUE);
	if (newval!=0)
	{
		// Grab display focus if needed.
		if (display_page_set(PAGE_FUNCTION,6)==0)
		{
			// Has encoder moved at all?
			if (newval!=0)
			{
#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
				if (testbutdown==true)
				{
					// If value encoder button is down, change test note pitch.
					//if (newval>lastval)
					if (newval>0)
					{
						MIDI_TestButtonInc();
					}
					//if (newval<lastval)
					if (newval<0)
					{
						MIDI_TestButtonDec();
					}
					//lastval = newval;
				}
				else
#endif /* PRODUCT */
				{
					// Allow roll-over of all functions (forget about "fill" functions).
#if PRODUCT==ATMEGADRUM
					newval += hard_val;
					if (newval>maxval) newval = 0;
					else if (newval<0) newval = maxval;
					Hardware_Let_Value(newval);
#else /* PRODUCT */
					newval += hard_val[hard_func];
					if (newval>maxval) newval = 0;
					else if (newval<0) newval = maxval;
					Hardware_Let_Value(hard_func,newval);
#endif /* PRODUCT */
				}
			}
		}
		// Eat rotary encoder value.
		keyboard_set_rotary_encoder_value(ENCODER_VALUE,0);
	}

	// Read Control Bank button (red/green mode)
	newbut = keyboard_get_pushbutton(BOARD_KEYBOARD_ISP,false);
#if PRODUCT==ATMEGADRUM
	if (lastbank==0 && newbut>0)
	{
		// Button pressed, toggle ctrl bank.
		if (hard_curctrlbank==0)
		{
			Hardware_Let_CtrlBank(1);
		}
		else
		{
			Hardware_Let_CtrlBank(0);
		}
		butstarttick = master_tick;
		finishedhold = false;
	}
#else /* PRODUCT */
	// Has a button just been pressed - do nothing on release
	if (lastbank!=newbut && newbut>0)
	{
		// Toggle ctrl bank.
		if (hard_curctrlbank==0)
		{
			Hardware_Let_CtrlBank(1);
		}
		else
		{
			Hardware_Let_CtrlBank(0);
		}
		// CPV - removed movingctrl stuff.
		// Set all ctrls to not moving, just in case last thing you did was move ctrl in other bank and not locked out.
		//Hardware_reset_moving_ctrl();
		//for (i=0; i<6; i++)
		//{
		//	movingctrl[i] = false;
		//	stopmovingtick[i] = master_tick + LOCKOUT;
		//}
		display_invalidate();
	}
	// Store last state of button.
	lastbank = newbut;
#endif /* PRODUCT */

	// Read Function encoder button (left button)
	newfuncbut = keyboard_get_pushbutton(ENCODER_FUNCTION,false);

#if PRODUCT==ATMEGADRUM
	if (lastfuncbut==0 && newfuncbut>0)
	{
		// Button down.
		butstarttick = master_tick;
		Hard_Let_Shift(true);
		finishedhold = false;
	}
	else if (lastfuncbut>0 && newfuncbut==0)
	{
		// Button up.
		butcurtick = master_tick - butstarttick;
		// Press for less than 2 sec to load sequencer pattern.
		if (butcurtick<2000)
		{
			// Do load here.
			lcd_cursor(1,0);
			lcd_puts("Load pattern ");
			display_print_value(1,FUNCTION_VALUE_OFFSET,hard_func+1,true);

			if (Seq_Get_Playing()==true)
			{
				Memory_Let_LoadOnBeat(hard_func);
				//Hardware_LED_StartFlash(0,253);
			}
			else
			{
				SysTick_Delay(100); // Give the user time to read the message.
				Memory_Load(hard_func);
				display_invalidate();
			}
		}
		Hard_Let_Shift(false);
	}

#else /* PRODUCT */

	// Are we on a function that requires function button to be held (e.g. save patch)?
	if (IsShiftHold(hard_func)==true)
	{
		//********SAVE AND LOAD IS DONE HERE**********
		//button pressed down, go green and wait
		if (lastfuncbut==0 && newfuncbut>0)
		{
			butstarttick = master_tick;
			Hard_Let_Shift(hard_func, true);
			finishedhold = false;
		}
		//is button still held
		else if (lastfuncbut>0 && newfuncbut>0)
		{
			butcurtick = master_tick - butstarttick;
			// Has button been held down for longer than hold length?
			if (butcurtick>=HOLDLENGTH && finishedhold==false)
			{
				switch (hard_func)
				{
#if PRODUCT!=DELAYERTRON
					case FUNC_WAVE:
						// then do userwave write
						Memory_UserWave_Write(hard_val[FUNC_WAVE]);
						break;

					case FUNC_AENVA:
						// then write patch to sysex
						MIDI_SYSEX_write_patch();
						break;

					case FUNC_AENVR:
						// then start sysex mem dump
						Memory_SYSEX_write_mem();
						break;
#endif
					case FUNC_MEM:
#if PRODUCT==ATMEGATRON
						if (MIDIchannelmode==true)
						{
							MIDI_Set_Channel(hard_val[hard_func]);
						}
						else
#endif /* PRODUCT */
						{
							lcd_cursor(1,0);
							lcd_puts("Saving...");
							SysTick_Delay(100);
							// then save patch
							Memory_Save(hard_val[hard_func]);
						}
						break;
				}
				// completed operation (to prevent refires)
				finishedhold = true;
				Hard_Let_Shift(hard_func, false);
				// make encoder flash
				Hardware_LED_StartFlash(0,7);
			}
		}
		// Has button been released...
		else if (newfuncbut==0 && Hard_Get_Shift(hard_func)==true)
		{
			butcurtick = master_tick - butstarttick;
			// and it's been held less time than hold length...
			if (butcurtick < HOLDLENGTH)
			{
				switch (hard_func)
				{
#if PRODUCT!=DELAYERTRON
					case FUNC_WAVE:
						// ...then toggle user wave mode
						Memory_UserWave_Read(hard_val[FUNC_WAVE]);
						break;
#endif
					case FUNC_MEM:
						lcd_cursor(1,0);
						// then load patch
						if (Memory_Load(hard_val[hard_func])==true)
						{
							lcd_puts("Loading...");
						}
						else
						{
							lcd_puts("Empty...");
						}
						SysTick_Delay(100);

						// cpv Hardware_reset_moving_ctrl();  // Stop knob jitter from overriding loaded patch.
						break;
				}
			}
			// Set shift back to false, LED red (red mode).
			Hard_Let_Shift(hard_func, false);
		}
	}
	else
	{
		// Button pressed, but not hold function.
		if (lastfuncbut!=newfuncbut && newfuncbut>0)
		{
			// Grab focus.
			display_page_set(PAGE_FUNCTION,7);
			// ...then toggle shift  (red/green mode)
			Hard_Let_Shift(hard_func, !Hard_Get_Shift(hard_func));
		}
	}
	//store last button state
	lastfuncbut = newfuncbut;
#endif /* PRODUCT */

	// Read Value encoder button (the button on the right)
	newvalbut = keyboard_get_pushbutton(ENCODER_VALUE,false);
#if PRODUCT==ATMEGADRUM
	if (lastvalbut==0 && newvalbut>0)
	{
		// Button pressed.
		Seq_Let_Pattern(hard_func,hard_val,!Seq_Get_Pattern(hard_func,hard_val));
		Hardware_Refresh_valLED();
	}
#else /* PRODUCT */
	//has button been pressed down
	if (lastvalbut==0 && newvalbut>0)
	{
		//midi test note on
		MIDI_TestButtonDown();
		testbutdown = true;
		//reset encoder pulse, because different ranges for midi test note mode.
		keyboard_set_rotary_encoder_value(ENCODER_VALUE,0);
		//knob goes green -> cpv red
		//Hardware_BicolorLED_SetState(1,2,1);
		//Hardware_LED_SetState(2,HIGH);
		//Hardware_LED_SetState(3,LOW);
	}
	//has button been released
	if (newvalbut==0 && lastvalbut>0)
	{
		//midi test note off
		MIDI_TestButtonUp();
		testbutdown = false;
		//reset encoder pulse again, to stop possible value dial jump
		keyboard_set_rotary_encoder_value(ENCODER_VALUE,0);
		//knob back to red -> cpv back to off
		//Hardware_BicolorLED_SetState(1,2,0);
	}
	//store last button state
	lastvalbut = newvalbut;
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
	if (lastbank==newbut && lastfuncbut==newfuncbut && lastvalbut==newvalbut)
	{
		// No change in button states.
		// Red-green button aka ANAL BANK
		if (lastbank==true)
		{
			// Press for at least 2 sec to clear sequencer.
			butcurtick = master_tick - butstarttick;
			if (butcurtick>=2000 && finishedhold==false)
			{
				finishedhold = true;
				Hardware_LED_StartFlash(2,7);
				Hardware_Let_CtrlBank(0);
				Seq_Clear();
				Hardware_Refresh_valLED();
				Hardware_Refresh_ValBG();
				lcd_cursor(1,0);
				lcd_puts("Clearing...");
				SysTick_Delay(200);
				display_invalidate();
			}
		}
		// Function encoder button
		if (lastfuncbut==true)
		{
			// Press for at least 2 sec to save sequencer pattern.
			butcurtick = master_tick - butstarttick;
			if (butcurtick>=2000 && finishedhold==false)
			{
				//DO SAVE HERE
				finishedhold = true;
				Memory_Save(hard_func);
				lcd_cursor(1,0);
				lcd_puts("Save pattern ");
				display_print_value(1,FUNCTION_VALUE_OFFSET,hard_func+1,true);
				SysTick_Delay(200);
				display_invalidate();
				//Hardware_LED_StartFlash(0,7);
			}
		}
	}
	// Store last button states.
	lastbank = newbut;
	lastfuncbut = newfuncbut;
	lastvalbut = newvalbut;
#endif /* PRODUCT */

	// Read Control knobs.

#if PRODUCT==ATMEGADRUM
	temp = 0;
	Hardware_Ctrl(temp,CTRL_TEMPO,60,255,true); // Tempo.
	Hardware_Ctrl(temp,CTRL_TUNE,0,255,true); // Tune.
	Hardware_Ctrl(temp,CTRL_LFO_LEVEL,0,255,true); // LFO level.
	Hardware_Ctrl(temp,CTRL_LFO_SPEED,0,15,false); // LFO speed.
	Hardware_Ctrl(temp,CTRL_DELAY,0,7,false); // Delay.
	Hardware_Ctrl(temp,CTRL_VOLUME,0,7,false); // Volume.
	if (master_tick-ctrlstarttick>=1000 && display_page_get()!=PAGE_FUNCTION)
	{
		// Fall back to function display if no control touched for too long.
		display_page_set(PAGE_FUNCTION,6);
	}
#endif /* PRODUCT */
#if PRODUCT==ATMEGATRON
	temp = Hardware_Get_Ctrl_Shift();
	Hardware_Ctrl(temp,CTRL_FILT,0,255,true);
	Hardware_Ctrl(temp,CTRL_Q,0,255,true);
	Hardware_Ctrl(temp,CTRL_ENV,0,255,true);
	Hardware_Ctrl(temp,CTRL_LFO,0,255,true);
	if (temp!=0)
	{
		Hardware_Ctrl(temp,CTRL_AMP,0,31,true); // Pulse width modulation
		Hardware_Ctrl(temp,CTRL_FX,0,31,true);  // Phaser
	}
	else
	{
		Hardware_Ctrl(temp,CTRL_AMP,0,255,true); // Amplitude modulation
		Hardware_Ctrl(temp,CTRL_FX,0,7,false); // Distortion
	}
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
	temp = Hardware_Get_Ctrl_Shift();
	Hardware_Ctrl(temp,CTRL_WAVE_CYCLE_LENGTH,0,31,false);
	Hardware_Ctrl(temp,CTRL_WAVE_CYCLE_LOOP,0,31,false);
	Hardware_Ctrl(temp,CTRL_WAVE_CYCLE_ENV,0,255,true);
	Hardware_Ctrl(temp,CTRL_WAVE_CYCLE_LFO,0,255,true);
	if (temp!=0)
	{
		Hardware_Ctrl(temp,CTRL_AMP,0,31,true); // Pulse width modulation
		Hardware_Ctrl(temp,CTRL_FX,0,31,true);  // Phaser
	}
	else
	{
		Hardware_Ctrl(temp,CTRL_AMP,0,255,true); // Amplitude modulation
		Hardware_Ctrl(temp,CTRL_FX,0,7,false); // Distortion
	}
#endif /* PRODUCT */
#if PRODUCT==DELAYERTRON
	temp = Hardware_Get_Ctrl_Shift();
	Hardware_Ctrl(temp,CTRL_FILT,0,255,true);
	Hardware_Ctrl(temp,CTRL_SAMPLE_RATE_ENV,0,255,true);
	Hardware_Ctrl(temp,CTRL_FEEDBACK,0,15,false);
	Hardware_Ctrl(temp,CTRL_SAMPLE_RATE_LFO,0,255,true);
	Hardware_Ctrl(temp,CTRL_DELAY_SAMPLE_RATE,0,255,true);
	Hardware_Ctrl(temp,CTRL_MIX,0,15,false);
#endif /* PRODUCT */

#define CTRL_FILT  (0)  // Atmegatron & Delayertron
#define CTRL_SAMPLE_RATE_ENV  (1)  // Delayertron
#define CTRL_FEEDBACK  (2)  // Delayertron
#define CTRL_SAMPLE_RATE_LFO  (3)  // Delayertron
#define CTRL_DELAY_SAMPLE_RATE  (4)  // Delayertron
#define CTRL_MIX  (5)  // Delayertron

	//update LEDs that may be flashing
	//Hardware_LED_RefreshFlash();
}


boolean Hardware_Ctrl(byte shift, uint8_t ctrl, int ctrl_min, int ctrl_max, boolean accelerate)
{
	int temp;
	boolean changed = false;
	uint8_t encoder = encoder_to_ctrl[ctrl+2];
	if (keyboard_get_pushbutton(encoder,false)>0)
	{
		// Reset rotary encoder.
		Hardware_Let_Ctrl(shift,ctrl,ctrl_min);
		changed = true;
	}
	else
	{
		temp = keyboard_get_rotary_encoder_value(encoder);
		if (accelerate==true)
		{
			temp *= keyboard_get_rotary_encoder_accelerator(encoder);
		}
		if (temp!=0)
		{
			temp += Hardware_Get_Ctrl(shift,ctrl);
			{
				if (temp>ctrl_max) temp = ctrl_max;
				else if (temp<ctrl_min) temp = ctrl_min;
				Hardware_Let_Ctrl(shift,ctrl,temp);
			}
			keyboard_set_rotary_encoder_value(encoder,0);
			changed = true;
		}
	}

	if (changed==true)
	{
		ctrlstarttick = master_tick;
		display_page_set(PAGE_CTRL,ctrl);
		display_invalidate();
	}

	return changed;
}

#if PRODUCT==ATMEGADRUM

/*int Hardware_Ctrl_Update(byte shift, byte index, int value)
{
	if (value<0)
	{
		//value = 255;
		value = 0;
	}
	else if (value>255)
	{
		//value = 0;
		value = 255;
	}
	Hardware_Let_Ctrl(index,value);
	display_invalidate();
	return value;
}*/


void Hardware_Let_Ctrl(byte shift, byte index, byte value)
{
	hard_ctrl[index] = value;
	switch (index)
	{
		case CTRL_TEMPO:      //Filter Cutoff
			//Seq_Let_Tempo(map(value,0,255,60,255));
			Seq_Let_Tempo(value);
			break;

		case CTRL_TUNE:
			if (value==0)
			{
				Pitch_Let_CurFreq(8000);
			}
			else
			{
				Pitch_Let_CurFreq(map(value,0,255,master_minocr1,16000));
			}
			break;

		case CTRL_LFO_LEVEL:
			Pitch_Let_LFOAmt(value);
			break;

		case CTRL_LFO_SPEED:
			//LFO_Let_Speed(speed_store[value>>4]); // Select LFO speed.
			LFO_Let_Speed(speed_store[value]); // Select LFO speed.
			break;

		case CTRL_DELAY:
			//Seq_Let_DelayBeats(hard_func,value>>5);
			Seq_Let_DelayBeats(hard_func,value);
			break;

		case CTRL_VOLUME:
			//Seq_Let_Volume(hard_func,7-(value>>5));
			Seq_Let_Volume(hard_func,7-value);
			break;
	}
}

byte Hardware_Get_Ctrl(byte shift, byte index)
{
	return hard_ctrl[index];
}

#else /* PRODUCT */

int Hardware_Ctrl_Update(byte shift, byte index, int value)
{
	if (value<0)
	{
		//value = 255;
		value = 0;
	}
	else if (value>255)
	{
		//value = 0;
		value = 255;
	}
	Hardware_Let_Ctrl(shift,index,value);
	display_invalidate();
	return value;
}


void Hardware_Let_Ctrl(byte shift, byte index, byte value)
{
  switch (index)
  {
    case CTRL_FILT:                      //Filter Cutoff frequency (same in red and green mode)
#if PRODUCT==DELAYERTRON
		hard_ctrl[shift][index] = value;
		if (shift==0)
		{
			Filt_Let_Fc(value);
		}
		else
		{
			Filt_Let_LFOAmt(value);
		}
#else
      hard_ctrl[0][index] = value;
      hard_ctrl[1][index] = value;
#endif
#if PRODUCT==ATMEGATRON
      Filt_Let_Fc(value);
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
      Cycle_Let_Inc(31 - (value/* >> 3*/));
#endif /* PRODUCT */
      break;

    case CTRL_Q: //Filter resonance (same in red and green mode)
#if PRODUCT==DELAYERTRON
		hard_ctrl[shift][index] = value;
		if (shift==0)
		{
			Delay_Let_FenvAmt(value); //Filter envelope amount
		}
		else
		{
			Pitch_Let_FenvAmt(value); //Pitch envelope amount
		}
#else
		hard_ctrl[0][index] = value;
		hard_ctrl[1][index] = value;
#endif
#if PRODUCT==ATMEGATRON
      Filt_Let_Q(value);
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
      Cycle_Let_Rep(value/* >> 3*/);
#endif /* PRODUCT */
      break;

    case CTRL_ENV:
#if PRODUCT==DELAYERTRON
		hard_ctrl[0][index] = value;
		hard_ctrl[1][index] = value;
		Delay_Let_Feedback(value/*>>4*/);
#else
      hard_ctrl[shift][index] = value;
      if (shift==0)
      {
#if PRODUCT==ATMEGATRON
          Filt_Let_FenvAmt(value);        //Filter envelope amount
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
          Cycle_Let_FenvAmt(value);        //Filter envelope amount
#endif /* PRODUCT */
      }
      else
      {
        Pitch_Let_FenvAmt(value);       //Pitch envelope amount
      }
#endif
      break;

    case CTRL_LFO:
      hard_ctrl[shift][index] = value;
      if (shift==0)
      {
#if PRODUCT==ATMEGATRON
          Filt_Let_LFOAmt(value); //Filter LFO amount
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
          Cycle_Let_LFOAmt(value); //Wave LFO amount
#endif /* PRODUCT */
#if PRODUCT==DELAYERTRON
          Delay_Let_LFOAmt(value); //Delay LFO amount
#endif
      }
      else
      {
        Pitch_Let_LFOAmt(value);        //Pitch LFO amount
      }
      break;

    case CTRL_AMP:
#if PRODUCT==DELAYERTRON
		hard_ctrl[0][index] = value;
		hard_ctrl[1][index] = value;
		Delay_Let_SF(value);
#else
      hard_ctrl[shift][index] = value;
      if (shift==0)
      {
        Amplitude_Let_LFOAmt(value);    //Amplitude LFO amount
      }
      else
      {
        PWM_Let_LFOAmt(value/*>>3*/);       //Pulse width modulation amount
      }
#endif
      break;

    case CTRL_FX:
#if PRODUCT==DELAYERTRON
		hard_ctrl[0][index] = value;
		hard_ctrl[1][index] = value;
		Delay_Let_DryMix(15-(value/*>>4*/));
		Delay_Let_WetMix(value/*>>4*/);
#else
      hard_ctrl[shift][index] = value;
      if (shift==0)
      {
        Distortion_Let_Amt(value/*>>5*/);  //Distortion (takes values 0-7)
      }
      else
      {
        Flange_Let_LFOAmt(value/*>>3*/);   //Phaser (takes values 0-31)
      }
#endif
      break;
  }
}


byte Hardware_Get_Ctrl(byte shift, byte index)
{
	return hard_ctrl[shift][index];
}


byte Hardware_Get_Ctrl_Shift(void)
{
	return hard_curctrlbank;
}

#endif /* PRODUCT */


//**************FUNCTION KNOB*******************
//set current function
void Hardware_Let_Function(byte newfunc)
{
	if (newfunc!=hard_func)
	{
		//if new value is different to current
		hard_func = newfunc;
		display_invalidate();
		//Hardware_Refresh_FuncBG();                                  //refresh function dial LED array
		//Hardware_Refresh_ValBG();                                   //refresh value dial LED array (to show val of new func)
//		Hardware_Refresh_ShiftLED(hard_func);                       //refresh shift (red/green) mode (to show shift mode of new func)
#if PRODUCT==ATMEGADRUM
		// CPV - removed movingctrl stuff.
		//Hardware_reset_moving_ctrl();  // Stops analogue knobs from affecting different instruments.
#else /* PRODUCT */
		if (hard_shift[FUNC_AENVS]==true && hard_func==FUNC_WAVE)
		{
			//set max value of value dial (normally 15)
			maxval = 5;
		}
		else
		{
			maxval = 15;
		}
#endif /* PRODUCT */
	}
}


byte Hardware_Get_Function()
{
	//get current function
	return hard_func;
}


void Hardware_Refresh_FuncBG(void)
{
	//user_interface_print_function(hard_func,hard_shift[hard_func]);
	display_invalidate();
}


//*************VALUE KNOB***********************
//set new value of function
#if PRODUCT==ATMEGADRUM

void Hardware_Let_Value(byte newval)
{
	if (newval!=hard_val)
	{   //don't allow floating positions, what to do??
		hard_val = newval;
		Hardware_Refresh_ValBG();
		Hardware_Refresh_valLED();
	}
}

byte Hardware_Get_Value(void)
{
	return hard_val;
}


void Hardware_Refresh_valLED(void)
{
	display_invalidate();
}


void Hard_Let_Shift(boolean newshift)
{
	hard_shift = newshift;
	Hardware_Refresh_ShiftLED();
}


boolean Hard_Get_Shift(void)
{
	return hard_shift;
}


void Hardware_Refresh_ShiftLED(void)
{
	// TODO: implement this, but what exactly should it do?

	if (hard_shift==true)
	{
		//hard_bg_led.digitalWrite(1,LOW);
		//hard_bg_led.digitalWrite(0,HIGH);
	}
	else
	{
		//hard_bg_led.digitalWrite(1,HIGH);
		//hard_bg_led.digitalWrite(0,LOW);
	}
}

#else /* PRODUCT */

void Hardware_Let_Value(byte func, byte newval)
{
	// Is new value different to current value?
	if (newval!=hard_val[func])
	{
		hard_val[func] = newval;
		Hardware_Adjust_Value(func, hard_val[func]);    //the meat of how the new value effects the sound
	    if (func==hard_func)
	    {
	    	Hardware_Refresh_ValBG();                     //refresh the value dial LED array
	    }
	}
}


void Hardware_Adjust_Value(byte func, byte val)
{               //the meat of how a new value effects the sound
  switch (func) {
#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
      case FUNC_WAVE:
#if PRODUCT!=DELAYERTRON
        if (hard_shift[FUNC_AENVS]==false)
#endif
        {
        	//if not user wave mode
          Wave_Let_Table(val);                                 //select waveform
        }
        break;
      case FUNC_FILT:
        Filt_Let_Type(val);                                    //set filter type
        break;
      case FUNC_FENVA:
        Fenv_Let_A(envADR_store[val]);       //set filter/pitch envelope attack time  (stored in PROGMEM)
        break;
      case FUNC_FENVDR:
        Fenv_Let_DR(envADR_store[val]);      //set filter/pitch envelope decay time
        break;
      case FUNC_FENVS:
        Fenv_Let_S(fenvS_store[val]);   //set filter/pitch envelope sustain level
        break;
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
      case FUNC_WAVE:
      case FUNC_FILT:
      case FUNC_FENVA:
      case FUNC_FENVDR:
        if (hard_shift[FUNC_AENVS]==false)
        {                    //if not user wave mode
          Wave_Let_Table(func,val);                                 //select waveform
        }
        break;
      case FUNC_FENVS:
        Fenv_Let_Preset(val);   //set filter/pitch envelope sustain level
        break;
#endif /* PRODUCT */
      case FUNC_AENVA:
        Aenv_Let_A(envADR_store[val]);       //set amplitude envelope attack time
        break;
      case FUNC_AENVD:
        Aenv_Let_D(envADR_store[val]);       //set amplitude envelope decay time
        break;
      case FUNC_AENVS:
        Aenv_Let_S(envS_store[val]);         //set amplitude envelope sustain level
        break;
      case FUNC_AENVR:
        Aenv_Let_R(envADR_store[val]);       //set amplitude envelope release time
        break;
      case FUNC_LFOTYPE:
        LFO_Let_Type(val);                                     //select lfo waveform
        break;
      case FUNC_LFOSPEED:
        LFO_Let_Speed(speed_store[val]);//select lfo speed  (stored in PROGMEM)
        break;
      case FUNC_ARPTYPE:                                       //select arpeggiator pattern
        Arp_Let_Type(val);
        if (val==0)
        {
        	// Make sure that both LEDs are on when the arpeggiator is switched off.
			if (Hardware_Get_Ctrl_Shift()==GREEN)
			{
				Hardware_BicolorLED_SetState(1,1,2);
				Hardware_BicolorLED_SetState(2,1,2);
			}
			else
			{
				Hardware_BicolorLED_SetState(1,2,1);
				Hardware_BicolorLED_SetState(2,2,1);
			}
        }
        break;
      case FUNC_ARPSPEED:
        Arp_Let_Speed(speed_store[val]);            //set arpeggiator speed when free running
        MIDI_Let_ClockArpSpeed(speed_store[val]);   //set arpeggiator speed when MIDI clock synced
        break;
      case FUNC_PORTA:
        Pitch_Let_Porta(porta_store[val]);          //set portamento time
        break;
      case FUNC_BITCRUSH:
#if PRODUCT==DELAYERTRON
    	  Delay_Let_Position(val);
#else
    	  BitCrush_Let_Type(val);                                //set wave crusher setting
#endif
        break;
      case FUNC_MEM:                                           //this is a hold function, so do nothing (patch save)
        break;
  }
}


byte Hardware_Get_Value(byte func)
{
	return hard_val[func];
}


void Hard_Let_Shift(byte func, boolean newshift)
{
	if (newshift!=hard_shift[func])
	{
		switch (func)
		{
#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON

			// Tie filter/pitch envelope shifts together (envelope invert mode).
			case FUNC_FENVA:
			case FUNC_FENVDR:
			case FUNC_FENVS:
				hard_shift[FUNC_FENVA] = newshift;
				hard_shift[FUNC_FENVDR] = newshift;
				hard_shift[FUNC_FENVS] = newshift;
				break;
#endif /* PRODUCT */

#if PRODUCT==DELAYERTRON
			case FUNC_AENVA:		//**GET rid of this when you put sysex back in
			case FUNC_AENVD:
			case FUNC_AENVS:
			case FUNC_AENVR:
				newshift = false;
				break;
#endif /* PRODUCT */

			// Tie LFO shifts together (LFO invert mode).
			case FUNC_LFOTYPE:
			case FUNC_LFOSPEED:
				hard_shift[FUNC_LFOTYPE] = newshift;
				hard_shift[FUNC_LFOSPEED] = newshift;
				break;

			// Tie arpeggiator shifts together (arpeggiator ping pong mode).
			case FUNC_ARPTYPE:
			case FUNC_ARPSPEED:
				hard_shift[FUNC_ARPTYPE] = newshift;
				hard_shift[FUNC_ARPSPEED] = newshift;
				break;

			default:
				hard_shift[func] = newshift;
		}
		// Meat of how shift affects sound.
		Hardware_Adjust_Shift(func, newshift);
		if (func==hard_func)
		{
		//	Hardware_Refresh_ShiftLED(func);          //refresh function knob LED colour
			Hardware_Refresh_FuncBG();
		}
		// Update display.
		display_invalidate();
	}
}


boolean Hard_Get_Shift(byte func)
{
	return hard_shift[func];
}


// Meat of how shift affects sound.
void Hardware_Adjust_Shift(byte func, boolean val)
{
  //byte i;
  switch (func)
  {
      case FUNC_WAVE:
#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
#if PRODUCT!=DELAYERTRON
        if (hard_shift[FUNC_AENVS]==false)
#endif
        {
          if (val==true)
          {
            Wave_Let_Bank(1);                          //set waveform bank
          }
          else
          {
            Wave_Let_Bank(0);
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
      case FUNC_FILT:
      case FUNC_FENVA:
      case FUNC_FENVDR:
        if (hard_shift[FUNC_AENVS]==false)
        {
          if (val==true)
          {
            Wave_Let_Bank(func,1);                          //set waveform bank
          }
          else
          {
            Wave_Let_Bank(func,0);
#endif /* PRODUCT */
          }
        }
        break;

#if PRODUCT==ATMEGATRON || PRODUCT==DELAYERTRON
      case FUNC_FILT:
#if PRODUCT==DELAYERTRON
          Filt_Let_PostDelay(val);
#else
    	  Filt_Let_GainAdj(val);                          //set filter normalise mode
#endif
        break;

      case FUNC_FENVA:
      case FUNC_FENVDR:
#endif /* PRODUCT */
      case FUNC_FENVS:
#if PRODUCT==DELAYERTRON
    	  Fenv_Let_Invert(val);            //set filter/pitch envelope invert mode
#else
        Fenv_Let_Invert(!Fenv_Get_Invert());            //set filter/pitch envelope invert mode
#endif
        break;

      case FUNC_AENVA:                                  //hold function, so do nothing here  (SEE Hardware_Poll)
        break;

#if PRODUCT!=DELAYERTRON
      case FUNC_AENVD:
        MIDI_Let_SYSEXRead(val);                        //set sysex read mode
        break;

      case FUNC_AENVS:
        Wave_Let_UserMode(val);                         //set user wave mode
        if (val==true && hard_val[FUNC_WAVE]>5)
        {
          Hardware_Let_Value(FUNC_WAVE,5);
        }
        break;
#endif

      case FUNC_AENVR:                                  //hold function, so do nothing here  (SEE Hardware_Poll)
        break;

      case FUNC_LFOTYPE:
      case FUNC_LFOSPEED:
        LFO_Let_Invert(val);                            //set LFO invert mode
        break;

      case FUNC_ARPTYPE:
      case FUNC_ARPSPEED:
        Arp_Let_PingPong(val);                          //set arpeggiator ping pong mode
        break;

      case FUNC_PORTA:
        Pitch_Let_PropPorta(val);                       //set proportional portamento mode
        break;

      case FUNC_BITCRUSH:
#if PRODUCT==DELAYERTRON
    	  Delay_Let_ClipMode(val);
#else
    	  BitCrush_Let_PreFilt(val);                      //set wave crusher pre-filter mode
#endif

        // CPV: no break???

      case FUNC_MEM:                                    //hold function, so do nothing here  (SEE Hardware_Poll)
        break;
  }
}

boolean IsShiftHold(byte func)
{
	//returns true for functions where encoder button should be held down
	switch (func)
	{
#if PRODUCT!=DELAYERTRON
		case FUNC_WAVE:
			return hard_shift[FUNC_AENVS];

		case FUNC_AENVA:
		case FUNC_AENVR:
#endif
		case FUNC_MEM:
			return true;
	}
	return false;
}

#endif /* PRODUCT */


void Hardware_Refresh_ValBG(void)
{
	//user_interface_print_value(0,13,hard_val[hard_func],true);
	display_invalidate();
}



//*************CTRL BANK BUTTON***********************
//set ctrl bank (red/green mode)
void Hardware_Let_CtrlBank(byte value)
{
	hard_curctrlbank = value;
	//Hardware_Refresh_CtrlBankLED();              //refresh LED color
	Hardware_BicolorLED_SetState(1,hard_curctrlbank!=0?1:0,hard_curctrlbank==0?1:0);
	Hardware_BicolorLED_SetState(2,hard_curctrlbank!=0?1:0,hard_curctrlbank==0?1:0);
	/*if (hard_curctrlbank==0)
	{
		Hardware_BicolorLED_SetState(false,true);
		//Hardware_LED_SetState(2,false);
		//Hardware_LED_SetState(3,true);
	}
	else
	{
		Hardware_BicolorLED_SetState(true,false);
		//Hardware_LED_SetState(2,true);
		//Hardware_LED_SetState(3,false);
	}*/

#if PRODUCT==ATMEGADRUM
	Seq_Let_Playing((boolean)value,false);
#endif /* PRODUCT */
}


void Hardware_LED_SetState(byte LEDnum, byte LEDstate)
{
	if (LEDnum==BOARD_LED2_GREEN)
	{
//		Board_LED_Set(BOARD_LED2_GREEN,LEDstate==HIGH?BOARD_LED_ON:BOARD_LED_OFF);
	}
	else if (LEDnum==BOARD_LED2_RED)
	{
//		Board_LED_Set(BOARD_LED2_RED,LEDstate==HIGH?BOARD_LED_ON:BOARD_LED_OFF);
	}
	else if (LEDnum==BOARD_LED3)
	{
		// MIDI LED.
		Board_LED_Set(BOARD_LED3,LEDstate);
	}
}


void Hardware_BicolorLED_SetState(uint8_t led, uint8_t green, uint8_t red)
{
	if (led==1)
	{
		if (green<2) Board_LED_Set(BOARD_LED1_GREEN,green==1?BOARD_LED_ON:BOARD_LED_OFF);
		if (red<2) Board_LED_Set(BOARD_LED1_RED,red==1?BOARD_LED_ON:BOARD_LED_OFF);
	}
	else if (led==2)
	{
		if (green<2) Board_LED_Set(BOARD_LED2_GREEN,green==1?BOARD_LED_ON:BOARD_LED_OFF);
		if (red<2) Board_LED_Set(BOARD_LED2_RED,red==1?BOARD_LED_ON:BOARD_LED_OFF);
	}
}


void Hardware_LED_StartFlash(byte LEDnum, byte FlashTimes)
{
	//SysTick_LED_Flash(LEDnum==0?BOARD_LED1_GREEN:BOARD_LED1_RED,10);
	if (LEDnum==4)
	{
		SysTick_LED_Flash(BOARD_LED3,10);
	}
}


void Hardware_LED_CancelFlash(byte LEDnum)
{
	if (LEDnum==4)
	{
		SysTick_LED_Flash(BOARD_LED3,0);
	}
}

/*boolean IsFuncFill(byte func)
{
  //returns true for functions where value dial fills up
  switch (func) {
      case FUNC_WAVE:
      case FUNC_FILT:
#if PRODUCT==ATCYCLOTRON
      case FUNC_FENVA:
      case FUNC_FENVDR:
      case FUNC_FENVS:
#endif
      case FUNC_LFOTYPE:
      case FUNC_LFOSPEED:
      case FUNC_ARPTYPE:
      case FUNC_ARPSPEED:
      case FUNC_BITCRUSH:
      case FUNC_MEM:
        return false;
        break;
      default:
        return true;
  }
}*/


/*void Hardware_reset_moving_ctrl(void)
{
  // Stops analogue knobs from affecting different insts.
  uint8_t i;
  for (i=0; i<6; i++)
  {
    movingctrl[i] = false;
  }
}*/

#if PRODUCT==ATCYCLOTRON

unsigned int Hardware_Get_envADR(byte index)
{                //get the current value of a function
	return envADR_store[index];
}

char Hardware_Get_envS(byte index)
{                //get the current value of a function
  return fenvS_store[index];
}

#endif /* PRODUCT */
