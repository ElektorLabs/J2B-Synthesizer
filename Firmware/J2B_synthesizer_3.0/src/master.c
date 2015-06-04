#include "atmegatron.h"
#include "chip.h"  // For SystemCoreClock only.

//#define F_CPU  SystemCoreClock

#if PRODUCT==ATCYCLOTRON
#define MASTER_OCR1_MIN  (120)
#else /* PRODUCT */
#define MASTER_OCR1_MIN  (100)
#endif /* PRODUCT */

byte master_tickspassed = 0;
unsigned long master_tick = 0;  //current tick.  used for all timing of lfos, arp and envs
unsigned int master_ocr1 = 10227;  //interrupt frequency (sets pitch of synth)

#if PRODUCT==ATMEGATRON
	volatile uint16_t master_index = 0;  //position in master_output wavetable
	unsigned char master_output[WAVE_LEN];  //output wavetable
	//unsigned char master_output_ch2[WAVE_LEN];  //output wavetable
#endif /* PRODUCT */
#if PRODUCT==ATCYCLOTRON
	volatile unsigned char master_index = 0;               //position in master_output wavetable
	volatile unsigned char master_output[TOT_WAVES][WAVE_LEN];   //output wavetable
	//volatile unsigned char master_output_ch2[TOT_WAVES][WAVE_LEN];   //output wavetable
	volatile unsigned char master_cycleinc = WAVE_LEN-1;
	volatile unsigned char master_cyclecurinc = 0;
	volatile unsigned char master_cyclerep = 0;
	volatile unsigned char master_cyclecurrep = 0;
	volatile unsigned char master_curwave = 0;
#endif /* PRODUCT */
#if PRODUCT==ATMEGADRUM
	volatile uint16_t master_index[16];
	volatile int32_t master_sum;
	volatile unsigned char master_output;
	const int master_minocr1 = 900;
#endif /* PRODUCT */
#if PRODUCT==DELAYERTRON
	volatile byte master_curtable = 0;
	volatile byte master_curbank = 0;
	volatile byte master_curnote = 57;                       //current note pressed
	volatile unsigned int master_index = 0;               //position in master_output wavetable
	volatile byte master_lastwaveindex = 0;
	volatile int32_t master_dry = 0;
	volatile int32_t master_wet = 0; // CPV
	int master_aenvlevel = 0;
	volatile byte master_wm = 8;
	volatile byte master_dm = 0;
	volatile byte master_clipbs = 1;
#endif /* PRODUCT */

#if PRODUCT!=DELAYERTRON
unsigned long master_sf = 7040;  //sample frequency, ocr1 derived from this

// Set the sample frequency (pitch)
void Master_Let_SampleFreq(void)
{
	// Has the pitch actually changed?
	if (Pitch_Get_PitchChanged()==true)
	{
		// Calculate new sample frequency.
		master_sf = Pitch_Get_FreqCalc();
#if PRODUCT!=ATMEGADRUM
		master_sf *= WAVE_LEN;
#endif
#if PRODUCT==ATCYCLOTRON
		master_sf *= TOT_WAVES; //CPV
#endif
		// Convert sample frequency to interrupt ticks.
		master_ocr1 = Chip_Clock_GetSystemClockRate()/master_sf;
		//master_ocr1 = F_CPU/master_sf;  //convert to interrupt time
		if (master_ocr1>65535)
		{
			// Highest possible value for ocr1 (lowest freq)
			master_ocr1 = 65535;
		}

		if (master_ocr1<MASTER_OCR1_MIN)
		{
			// Lowest possible value for ocr1.  Any lower and code in interrupt doesn't have time to complete.
			// Worked out by trial and error.
			master_ocr1 = MASTER_OCR1_MIN;
		}
		//master_ocr1 >>= 2; // cpv
	}
}


unsigned long Master_Get_SampleFreq(void)
{
	return master_sf;
}
#endif /* PRODUCT */


#if PRODUCT==ATMEGADRUM
void Init_Master(void)
{
	byte i;
	for (i=0; i<16; i++)
	{
		master_index[i] = Wavestore_Get_Start(i+1)-1;
	}  
}
#endif
