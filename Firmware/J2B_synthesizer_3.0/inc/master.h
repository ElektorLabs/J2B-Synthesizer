#ifndef __MASTER_H__
#define __MASTER_H__


#define MASTER_BEATTICK  (500) //length of beat in ticks. 500 = 120bpm i.e. 1 beat = 500ms (this is overridden when MIDI clock received)


extern byte master_tickspassed;
extern unsigned long master_tick;  //current tick.  used for all timing of lfos, arp and envs
extern unsigned int master_ocr1;  //interrupt frequency (sets pitch of synth)

#if PRODUCT!=DELAYERTRON
extern unsigned long master_sf; // = 7040;   //sample frequency, ocr1 derived from this
void Master_Let_SampleFreq(void);
unsigned long Master_Get_SampleFreq(void);
#endif

#if PRODUCT==ATMEGATRON
	extern volatile uint16_t master_index;  //position in master_output wavetable
	extern unsigned char master_output[WAVE_LEN];   //output wavetable
	//extern unsigned char master_output_ch2[WAVE_LEN];   //output wavetable
#endif /* PRODUCT */

#if PRODUCT==ATCYCLOTRON
	extern volatile unsigned char master_index;               //position in master_output wavetable
	extern volatile unsigned char master_output[TOT_WAVES][WAVE_LEN];   //output wavetable
	//extern volatile unsigned char master_output_ch2[TOT_WAVES][WAVE_LEN];   //output wavetable
	extern volatile unsigned char master_cycleinc;
	extern volatile unsigned char master_cyclecurinc;
	extern volatile unsigned char master_cyclerep;
	extern volatile unsigned char master_cyclecurrep;
	extern volatile unsigned char master_curwave;
#endif /* PRODUCT */

#if PRODUCT==ATMEGADRUM
	#define MASTER_SAMPS 16
	extern volatile uint16_t master_index[16];
	extern volatile signed int master_sum;
	extern volatile unsigned char master_output;
	extern const int master_minocr1;
	void Init_Master(void);
#endif /* PRODUCT */

#if PRODUCT==DELAYERTRON
	extern volatile byte master_curtable;
	extern volatile byte master_curbank;
	extern volatile byte master_curnote; //current note pressed
	extern volatile unsigned int master_index; //position in master_output wavetable
	extern volatile byte master_lastwaveindex;
	extern volatile int32_t master_dry;
	extern volatile int32_t master_wet; // CPV
	extern int master_aenvlevel;
	extern volatile byte master_wm;
	extern volatile byte master_dm;
	extern volatile byte master_clipbs;
#endif /* PRODUCT */


#endif // __MASTER_H__
