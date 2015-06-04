/*
 * @brief J2B Synthesizer
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

#include "chip.h"
#include "board.h"
#include "systick.h"
#include "pwm_timer.h"
#include "atmegatron.h"
#include "lcd.h"


#define J2B_PROCESSING  (1)


pwm_timer_t sample_rate_clock;
pwm_timer_t audio_out_clock;
#define AUDIO_OUT_CHANNEL_1  (0)
#define AUDIO_OUT_CHANNEL_2  (2)
#define AUDIO_OUT_FREQUENCY_REGISTER  (PWM_TIMER_FREQUENCY_REGISTER)
#define SAMPLE_RATE_OUT_CHANNEL  (3)
#define SAMPLE_RATE_FREQUENCY_REGISTER  (0)

volatile uint32_t pwm_update = 256;
volatile uint32_t pwm_update_ch2 = 256;
boolean mono = false;

// Audio out ISR
void TIMER32_1_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER32_1,AUDIO_OUT_FREQUENCY_REGISTER))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER32_1,AUDIO_OUT_FREQUENCY_REGISTER);

#if PRODUCT==DELAYERTRON
	{
		// This code is supposed to run at the PWM frequency (31.373 kHz or 16MHz/510 on Delayertron).
		uint16_t mult;
		uint16_t bs;
		uint16_t master_waveindex;
		int32_t master_out;

		mult = MIDI_mult[master_curnote];
		bs = MIDI_bs[master_curnote];
		master_waveindex = (master_index * mult >> bs) % WAVE_LEN;

		if (master_waveindex < master_lastwaveindex)
		{
			master_aenvlevel = Aenv_Get_Level();
			master_curnote = Pitch_Get_FreqCalc(); // get note
			master_index = 0;
		}
		master_lastwaveindex = master_waveindex;

#if J2B_PROCESSING==1
		master_out = wave_store[master_curbank][master_curtable][master_waveindex];
		// Multiply sample by current amp env level (0-255) and divide by 256
		master_out = master_out * master_aenvlevel >> 8;
		// 100% dry mix is actually set here
		master_dry = (sample_t) master_out;
		master_wet = delay_buffer[delay_readpos]; // CPV
		if (master_dm==8)
		{
			// 100% wet mix
			master_out = master_wet;
		}
		else if (master_wm<8)
		{
			// wet/dry mix
			master_out = (master_dry >> master_dm) + (master_wet >> master_wm);
		}

		// master_clipbs is Delay_(L/G)et_ClipMode, can be 0 or 1.
		master_out = master_out >> master_clipbs;
#elif J2B_PROCESSING==2
		master_dry = (master_aenvlevel*wave_store[master_curbank][master_curtable][master_waveindex]) >> (8-master_wm);
		master_wet = delay_buffer[delay_readpos] << master_dm;
		master_out = (master_dry+master_wet) >> (master_wm+master_dm+master_clipbs);
#endif

		if (Filt_Get_Type()>0 && Filt_Get_PostDelay()==false)
		{
			master_out = Filter_Process(master_out);
		}

		if (master_out>127)
		{
			pwm_update = 255;
		}
		else if (master_out<-128)
		{
			pwm_update = 0;
		}
		else
		{
			pwm_update = master_out + 128;
		}
		pwm_update_ch2 = pwm_update;

		master_index++;  //increment position in wavetable
	}
#endif /* PRODUCT */

		Chip_TIMER_SetMatch(LPC_TIMER32_1,AUDIO_OUT_CHANNEL_1,pwm_update);
		Chip_TIMER_SetMatch(LPC_TIMER32_1,AUDIO_OUT_CHANNEL_2,pwm_update_ch2);
	}
}


// Pitch ISR
void TIMER32_0_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER32_0,SAMPLE_RATE_FREQUENCY_REGISTER))
	{
		// Clear interrupt flag.
		Chip_TIMER_ClearMatch(LPC_TIMER32_0,SAMPLE_RATE_FREQUENCY_REGISTER);
		
#if PRODUCT==ATMEGATRON
		// If at end of wavetable, go back to start.
		if (master_index>WAVE_LEN-1)
		{
			master_index = 0;
		}

		// Set sample value.
		// We have 4 bits more resolution than on AVR so multiply by 4.
		// pwm_update is consumed in the Audio Out ISR.
		pwm_update = master_output[master_index] + 128;
		pwm_update_ch2 = pwm_update;

		/* CPV - for later
		if (mono==true) pwm_update_ch2 = master_output[master_index] + 128;
		else pwm_update_ch2 = master_output_ch2[master_index] + 128;*/

		//Chip_TIMER_SetMatch(audio_out_clock.timer,AUDIO_OUT_CHANNEL,master_output[master_index]+128);
		// Increment position in wavetable.
		master_index++;
#endif /* PRODUCT */
		
#if PRODUCT==ATCYCLOTRON
		//sets sample value to current index of current wave
		pwm_update = master_output[master_curwave][master_index] + 128;
		pwm_update_ch2 = pwm_update;

		/* CPV - for later
		 * if (mono==true) pwm_update_ch2 = master_output[master_curwave][master_index] + 128;
		else pwm_update_ch2 = master_output_ch2[master_curwave][master_index] + 128;*/

		master_index++;                                //increment index (position in wavetable)
		if (master_index > WAVE_LEN-1)
		{
			//if at end of wavetable, go back to start
			master_index = 0;
		}
		master_cyclecurinc++;                          //increment current wave increment
		if (master_cyclecurinc >= master_cycleinc)
		{
			//if current wave increment >= wave increment value    ***32 = full wave cycle playback
			master_cyclecurrep++;                          //increment current wave repeat
			if (master_cyclecurrep >= master_cyclerep)
			{
				//if current wave repeart >= wave repeat value       ***1 = single cycle playback
				master_curwave++;                              //increment current wave number
				if (master_curwave >= TOT_WAVES)
				{
					//if current wave number >= 4 then reset current wave
					master_curwave = 0;
				}
				master_cyclecurrep = 0;                        //reset current wave repeat
			}
			master_cyclecurinc = 0;                        //reset current wave increment
		}
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
		master_sum =
			(Wavestore_Get_Sample(master_index[0])>>Seq_Get_Amplitude(0)) +
			(Wavestore_Get_Sample(master_index[1])>>Seq_Get_Amplitude(1)) + 
			(Wavestore_Get_Sample(master_index[2])>>Seq_Get_Amplitude(2)) +
			(Wavestore_Get_Sample(master_index[3])>>Seq_Get_Amplitude(3)) + 
			(Wavestore_Get_Sample(master_index[4])>>Seq_Get_Amplitude(4)) + 
			(Wavestore_Get_Sample(master_index[5])>>Seq_Get_Amplitude(5)) + 
			(Wavestore_Get_Sample(master_index[6])>>Seq_Get_Amplitude(6)) + 
			(Wavestore_Get_Sample(master_index[7])>>Seq_Get_Amplitude(7)) + 
			(Wavestore_Get_Sample(master_index[8])>>Seq_Get_Amplitude(8)) + 
			(Wavestore_Get_Sample(master_index[9])>>Seq_Get_Amplitude(9)) + 
			(Wavestore_Get_Sample(master_index[10])>>Seq_Get_Amplitude(10)) + 
			(Wavestore_Get_Sample(master_index[11])>>Seq_Get_Amplitude(11)) + 
			(Wavestore_Get_Sample(master_index[12])>>Seq_Get_Amplitude(12)) + 
			(Wavestore_Get_Sample(master_index[13])>>Seq_Get_Amplitude(13)) +
			(Wavestore_Get_Sample(master_index[14])>>Seq_Get_Amplitude(14)) + 
			(Wavestore_Get_Sample(master_index[15])>>Seq_Get_Amplitude(15));

		// Scale back to compensate for addition.
		// Why isn't this done in the original Atmegadrum firmware??? (v1.2)
		// The sound quality is already crappy enough like this.
		master_sum >>= 3;

		// Let's do this the other way around for better sound quality.
		// Instead of shifting to the right and losing lots of bits, we now
		// shift to the left and divide afterwards.
		/*master_sum =
			(Wavestore_Get_Sample(master_index[0])<<(7-Seq_Get_Amplitude(0))) +
			(Wavestore_Get_Sample(master_index[1])<<(7-Seq_Get_Amplitude(1))) +
			(Wavestore_Get_Sample(master_index[2])<<(7-Seq_Get_Amplitude(2))) +
			(Wavestore_Get_Sample(master_index[3])<<(7-Seq_Get_Amplitude(3))) +
			(Wavestore_Get_Sample(master_index[4])<<(7-Seq_Get_Amplitude(4))) +
			(Wavestore_Get_Sample(master_index[5])<<(7-Seq_Get_Amplitude(5))) +
			(Wavestore_Get_Sample(master_index[6])<<(7-Seq_Get_Amplitude(6))) +
			(Wavestore_Get_Sample(master_index[7])<<(7-Seq_Get_Amplitude(7))) +
			(Wavestore_Get_Sample(master_index[8])<<(7-Seq_Get_Amplitude(8))) +
			(Wavestore_Get_Sample(master_index[9])<<(7-Seq_Get_Amplitude(9))) +
			(Wavestore_Get_Sample(master_index[10])<<(7-Seq_Get_Amplitude(10))) +
			(Wavestore_Get_Sample(master_index[11])<<(7-Seq_Get_Amplitude(11))) +
			(Wavestore_Get_Sample(master_index[12])<<(7-Seq_Get_Amplitude(12))) +
			(Wavestore_Get_Sample(master_index[13])<<(7-Seq_Get_Amplitude(13))) +
			(Wavestore_Get_Sample(master_index[14])<<(7-Seq_Get_Amplitude(14))) +
			(Wavestore_Get_Sample(master_index[15])<<(7-Seq_Get_Amplitude(15)));

		master_sum >>= 10;*/

		if (master_sum>127)
		{
			master_output=255;
		}
		else if (master_sum<-128)
		{
			master_output=0;
		}
		else
		{
			master_output = master_sum + 128;
		}
		pwm_update = master_output;
		pwm_update_ch2 = master_output;
		
		if (master_index[0]<Wavestore_Get_Start(1)) master_index[0]++; 
		if (master_index[1]<Wavestore_Get_Start(2)) master_index[1]++; 
		if (master_index[2]<Wavestore_Get_Start(3)) master_index[2]++; 
		if (master_index[3]<Wavestore_Get_Start(4)) master_index[3]++; 
		if (master_index[4]<Wavestore_Get_Start(5)) master_index[4]++; 
		if (master_index[5]<Wavestore_Get_Start(6)) master_index[5]++; 
		if (master_index[6]<Wavestore_Get_Start(7)) master_index[6]++; 
		if (master_index[7]<Wavestore_Get_Start(8)) master_index[7]++; 
		if (master_index[8]<Wavestore_Get_Start(9)) master_index[8]++; 
		if (master_index[9]<Wavestore_Get_Start(10)) master_index[9]++; 
		if (master_index[10]<Wavestore_Get_Start(11)) master_index[10]++; 
		if (master_index[11]<Wavestore_Get_Start(12)) master_index[11]++; 
		if (master_index[12]<Wavestore_Get_Start(13)) master_index[12]++; 
		if (master_index[13]<Wavestore_Get_Start(14)) master_index[13]++; 
		if (master_index[14]<Wavestore_Get_Start(15)) master_index[14]++; 
		if (master_index[15]<Wavestore_Get_Start(16)) master_index[15]++; 
#endif /* PRODUCT */

#if PRODUCT==DELAYERTRON
		{
			// This code is supposed to run at the sample rate frequency.
			sample_t rp, msb, lsb;
			int wp;

			if (master_wm==8)
			{
				// 100% wet.
				return;
			}
			if (Delay_Get_Feedback()==0)
			{
#if J2B_PROCESSING==1
				wp = master_dry;
#elif J2B_PROCESSING==2
				wp = master_dry >> master_wm;
#endif
				/*if (Filt_Get_Type()>0 && Filt_Get_PostDelay()==true)
				{
					wp = Filter_Process(wp);
				}
				delay_buffer[delay_writepos] = wp;*/
			}
			else
			{
				rp = delay_buffer[delay_readpos];
				msb = rp >> delay_fbmsb >> master_clipbs;
				if (msb==-1 && delay_fbmsb>0)
				{
					msb = 0;
				}
				lsb = rp >> delay_fblsb >> master_clipbs;
				if (lsb==-1 && delay_fblsb>0)
				{
					lsb = 0;
				}

#if J2B_PROCESSING==1
				wp = (master_dry >> master_clipbs) + msb + lsb;
#elif J2B_PROCESSING==2
				wp = (master_dry >> (master_wm+master_clipbs)) + msb + lsb;
#endif

				/*if (Filt_Get_Type()>0 && Filt_Get_PostDelay()==true)
				{
					wp = Filter_Process(wp);
				}
				if (wp<-128)
				{
					delay_buffer[delay_writepos] = -128;
				}
				else if (wp>127)
				{
					delay_buffer[delay_writepos] = 127;
				}
				else
				{
					delay_buffer[delay_writepos] = (sample_t)wp;
				}*/
			}

			if (Filt_Get_Type()>0 && Filt_Get_PostDelay()==true)
			{
				wp = Filter_Process(wp);
			}
			if (wp<-128) wp = -128;
			else if (wp>127) wp = 127;
			//else if (wp>-2 && wp<2) wp = 0; // CPV
			delay_buffer[delay_writepos] = (sample_t)wp;

			if (delay_writepos > DELAY_SIZE-2)
			{
				//if at end of wavetable, go back to start
				delay_writepos = 0;
			}
			else
			{
				delay_writepos++;
			}
			if (delay_readpos > DELAY_SIZE-2)
			{
				//if at end of wavetable, go back to start
				delay_readpos = 0;
			}
			else
			{
				delay_readpos++;
			}
		}
#endif /* PRODUCT */

		// Set pitch.
		Chip_TIMER_SetMatch(LPC_TIMER32_0,SAMPLE_RATE_FREQUENCY_REGISTER,master_ocr1);
		// Keep duty-cycle of 50%.
		Chip_TIMER_SetMatch(LPC_TIMER32_0,SAMPLE_RATE_OUT_CHANNEL,master_ocr1>>1);
	}
}




void setup(void)
{
	// The hardware setup has been removed from setup and is done before.
	// Here we now only initialize synthesizer stuff.

#if PRODUCT==ATMEGATRON ||PRODUCT==ATCYCLOTRON
	Init_Waves();  // Initialize wavetables.
	//Init_Hardware();  // Initialize input controls.
	LFO_CalcVal();  // Initialize LFO.
	AEnv_CalcVal();  // Initialize amplitude envelope.
	FEnv_CalcVal();  // Initialize filter/pitch envelope.
#endif /* PRODUCT */

#if PRODUCT==ATMEGATRON
	Filt_CalcVals();  // Initialize filter.
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
	Init_Master();
	//Init_Hardware();
#endif /* PRODUCT */

	Init_Hardware();

	if (POLL_MIDI==1)
	{
		// Open MIDI port (standard serial port)
		MIDI_Init();
	}
	else
	{
		// Open the port for debugging.
		Serial.begin(9600);
	}

#if PRODUCT==ATMEGATRON ||PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
	Memory_Vanilla();  //load the Vanilla patch
#endif /* PRODUCT */

	Hardware_LED_StartFlash(0,5);    //flash to say "I'm ready!"
}


void loop_priority_low(void)
{
	static uint32_t t_prev = 0;
	uint32_t t;

	if (POLL_MIDI==1)
	{
		// Poll the MIDI input.
		MIDI_Poll();
	}
	if (POLL_HARD==1)
	{
		t = systick_counter;
		if (t>=t_prev+10)
		{
			// Limit hardware polling to 100 Hz.
			t_prev = t;
			// Poll the hardware (potentiometers, rotary encoders and buttons).
			Hardware_Poll();
		}
	}

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
	#ifdef POLL_TEST
		{
			if (master_tick>=Testmode_Get_NextTick())
			{
				// If test mode is being used, trigger next test note if it's time to.
				Testmode_ProcessNext();
			}
		}
	#endif
#endif

#if PRODUCT==ATMEGATRON ||PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
	// Poll the arpeggiator and see if it's time for a note (if it's on).
	Arp_Poll();
#endif

	display_draw(false);
	keyboard_scan_matrix();
}


void loop_priority_high(void)
{
	// Update master_tick.
	//master_tick = systick_counter;
	master_tickspassed = systick_counter - master_tick;
	master_tick += master_tickspassed;

#if PRODUCT==ATMEGADRUM
	Seq_Process();
#endif

	LFO_CalcVal();  //calculate the current output of the LFO
	
#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON
	AEnv_CalcVal();  //calculate the current output of the amplitude envelope
	FEnv_CalcVal();  //calculate the current output of the filter/pitch envelope
#endif
#if PRODUCT==ATMEGATRON
	Filt_CalcVals();  //calculate the variables used by the filter
#endif
#if PRODUCT==ATCYCLOTRON
	Cycle_CalcVals();
#endif
#if PRODUCT==DELAYERTRON
	Filt_CalcFc();  //calculate the variables used by the filter
	Delay_CalcSF();
#endif

	Pitch_Process();  //calculate the current pitch

#if PRODUCT!=DELAYERTRON
	Master_Let_SampleFreq();  //set interrupt freq based on above calculation
#endif
	
#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON
	// If a key is down (actually is amplitude envelope idle):
	if (Aenv_Get_State()>0)
	{
		// Process the wavetable (filter/envelope/fx etc...).
		Wave_Process();
		Flange_Process_Channel2();
	}
	// Otherwise clear the wavetable.  This is to stop.
	else
	{
		Wave_Clear();
		Flange_ClearBuffer();
	}
#endif
}


/*void loop(void)
{
	if (POLL_MIDI==1)
	{
		// Poll the MIDI input.
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0,10,true);
		MIDI_Poll();
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0,10,false);
	}
	if (POLL_HARD==1)
	{
		// Poll the hardware (potentiometers, rotary encoders and buttons).
		Hardware_Poll();
	}

	// Update master_tick.
	master_tick = systick_counter;

	if (POLL_TEST==1)
	{
		if (master_tick>=Testmode_Get_NextTick())
		{
			// If test mode is being used, trigger next test note if it's time to.
			Testmode_ProcessNext();
		}
	}
	Arp_Poll();                                       //poll the arpeggiator and see if it's time for a note (if it's on)
	LFO_CalcVal();                                    //calculate the current output of the LFO
	AEnv_CalcVal();                                   //calculate the current output of the amplitude envelope
	FEnv_CalcVal();                                   //calculate the current output of the filter/pitch envelope
	Filt_CalcVals();                                  //calculate the variables used by the filter
	Pitch_Process();                                  //calculate the current pitch
	Master_Let_SampleFreq();                          //set interrupt freq based on above calculation
	// If a key is down (actually is amplitude envelope idle):
	if (Aenv_Get_State()>0)
	{
		// Process the wavetable (filter/envelope/fx etc...).
		Wave_Process();
	}
	// Otherwise clear the wavetable.  This is to stop.
	else
	{
		Wave_Clear();
		Flange_ClearBuffer();
	}
}*/


int main(void)
{
	// Generic initialization.
	// Before we get here the startup routine has already called SystemInit,
	// which resides in the file sysinit.c
	SystemCoreClockUpdate();
	Board_Init();
	SysTick_Config(SystemCoreClock/SYSTICK_RATE_HZ);

	// Setup LCD, pushbuttons and rotary encoders.
	keyboard_init();
	display_init();

	// Setup PWM timer for audio output.
	// Use default frequency register (MR3).
	// Audio timer is LPC_TIMER32_1, i.e. CT32B1
	// MAT10 (PIO0_13) is PWM1.
	// MAT11 (PIO0_14) is shared with S5B.
	// MAT12 (PIO0_15) is shared with green LED1.
	// MAT13 (PIO0_16) is shared with S6B.
	audio_out_clock = pwm_timers[AUDIO_OUT_TIMER];
	audio_out_clock.frequency_register = AUDIO_OUT_FREQUENCY_REGISTER;
#if PRODUCT==DELAYERTRON
	// Atdelayertron needs much lower PWM frequency because it does a lot
	// of stuff in the PWM ISR.
	PwmTimer_Init(&audio_out_clock,281250/4);
#else
	PwmTimer_Init(&audio_out_clock,281250/2);
#endif
	// Setup channel 0 for PWM output on pin 34 (PIO0_13).
	PwmTimer_InitChannel(&audio_out_clock,AUDIO_OUT_CHANNEL_1,500); // Duty-cycle x10 for more precision.
	PwmTimer_InitChannel(&audio_out_clock,AUDIO_OUT_CHANNEL_2,500); // Duty-cycle x10 for more precision.
	PwmTimer_Start(&audio_out_clock);
	PwmTimer_EnableInterrupt(&audio_out_clock,AUDIO_OUT_FREQUENCY_REGISTER);
	NVIC_SetPriority(TIMER_32_1_IRQn,0);

	// Setup PWM timer for pitch/sample rate.
	// Sample rate timer is LPC_TIMER32_0, i.e. CT32B0
	// MAT00 (PIO0_18) is PWM2.
	// MAT01 (PIO0_19) is shared with S6A.
	// MAT02 (PIO0_1) is shared with the ISP enable pin / S9.
	// MAT03 (PIO1_27) is FX32.
	sample_rate_clock = pwm_timers[SAMPLE_RATE_TIMER];
	sample_rate_clock.frequency_register = SAMPLE_RATE_FREQUENCY_REGISTER;
	// This frequency setting has no effect because master_ocr1 controls the frequency.
	PwmTimer_Init(&sample_rate_clock,440*32);
	// Setup channel 0 for PWM output on pin 12 (PIO1_27).
	PwmTimer_InitChannel(&sample_rate_clock,SAMPLE_RATE_OUT_CHANNEL,500); // Duty-cycle x10 for more precision.
	PwmTimer_Start(&sample_rate_clock);
	PwmTimer_EnableInterrupt(&sample_rate_clock,SAMPLE_RATE_FREQUENCY_REGISTER);
	NVIC_SetPriority(TIMER_32_0_IRQn,1);

	// Initialize random number generator (seems to be missing in original code).
	randomSeed(0); // TODO: find a better seed.

	// Call the Arduino setup function.
	setup();

	// LEDs toggle in interrupt handlers.
	//uint32_t seconds_counter_prev = seconds_counter;

	while (1)
	{
		// Use pushbutton S11 ("Spare") to quickly enter ISP mode.
		/*if (keyboard_get_pushbutton(BOARD_KEYBOARD_S22)==key_pressed)
		{
			__disable_irq();
			// How can we invoke USB ISP mode from software?
			// This call invokes serial ISP.
			Chip_FLASH_ReInvokeISP();
		}*/

		if (keyboard_get_pushbutton_changed(BOARD_KEYBOARD_S22)==true)
		{
			if (keyboard_get_pushbutton(BOARD_KEYBOARD_S22,true)==key_pressed)
			{
				mono = !mono;
			}
		}

		loop_priority_low();
		// The idea was to call loop_priority_high from the systick ISR to
		// fix the sample calculation rate to a stable known value. This
		// was OK on the LPC1343, but execution is too long on the LPC1347?
		// The assembly code is identical for both cases.
		loop_priority_high();

		// Do we need this?
		__WFI();
	}

	return 0;
}

/**
 * @}
 */
