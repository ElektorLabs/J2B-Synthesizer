#ifndef __SAMPLE_RATE_H__
#define __SAMPLE_RATE_H__


#include "timers.h"

#define SAMPLE_RATE_FREQUENCY_REGISTER  (3)  /* Choose the MR register that will determine the frequency. */
#define SAMPLE_RATE_MAT_OUTPUT  SAMPLE_RATE_FREQUENCY_REGISTER

/*
#if SAMPLE_RATE_TIMER_NR == TIMER16_0
	#define SAMPLE_RATE_TIMER  LPC_TIMER16_0
	#define SAMPLE_RATE_TIMER_IRQ  (TIMER_16_0_IRQn)
	#if SAMPLE_RATE_MAT_OUTPUT == 0
		#define SAMPLE_RATE_MAT_PORT  (0)
		#define SAMPLE_RATE_MAT_PIN  (8)
		#define SAMPLE_RATE_IOCON  IOCON_PIO0_8
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#elif SAMPLE_RATE_MAT_OUTPUT == 1
		#define SAMPLE_RATE_MAT_PORT  (0)
		#define SAMPLE_RATE_MAT_PIN  (9)
		#define SAMPLE_RATE_IOCON  IOCON_PIO0_9
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#elif SAMPLE_RATE_MAT_OUTPUT == 2
		#define SAMPLE_RATE_MAT_PORT  (0)
		#define SAMPLE_RATE_MAT_PIN  (10)
		#define SAMPLE_RATE_IOCON  IOCON_PIO0_10
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC3
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#elif SAMPLE_RATE_TIMER_NR == TIMER16_1
	#define SAMPLE_RATE_TIMER  LPC_TIMER16_1
	#define SAMPLE_RATE_TIMER_IRQ  (TIMER_16_1_IRQn)
	#if SAMPLE_RATE_MAT_OUTPUT == 0
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (9)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_9
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC1
	#elif SAMPLE_RATE_MAT_OUTPUT == 1
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (10)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_10
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#elif SAMPLE_RATE_TIMER_NR == TIMER32_0
	#define SAMPLE_RATE_TIMER  LPC_TIMER32_0
	#define SAMPLE_RATE_TIMER_IRQ  (TIMER_32_0_IRQn)
	#if SAMPLE_RATE_MAT_OUTPUT == 0
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (6)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_6
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#elif SAMPLE_RATE_MAT_OUTPUT == 1
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (7)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_7
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#elif SAMPLE_RATE_MAT_OUTPUT == 2
		#define SAMPLE_RATE_MAT_PORT  (0)
		#define SAMPLE_RATE_MAT_PIN  (1)
		#define SAMPLE_RATE_IOCON  IOCON_PIO0_1
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#elif SAMPLE_RATE_MAT_OUTPUT == 3
		#define SAMPLE_RATE_MAT_PORT  (0)
		#define SAMPLE_RATE_MAT_PIN  (11)
		#define SAMPLE_RATE_IOCON  IOCON_PIO0_11
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC3
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#elif SAMPLE_RATE_TIMER_NR == TIMER32_1
	#define SAMPLE_RATE_TIMER  LPC_TIMER32_1
	#define SAMPLE_RATE_TIMER_IRQ  (TIMER_32_1_IRQn)
	#if SAMPLE_RATE_MAT_OUTPUT == 0
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (1)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_1
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC3
	#elif SAMPLE_RATE_MAT_OUTPUT == 1
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (2)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_2
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC3
	#elif SAMPLE_RATE_MAT_OUTPUT == 2
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (3)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_3
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC3
	#elif SAMPLE_RATE_MAT_OUTPUT == 3
		#define SAMPLE_RATE_MAT_PORT  (1)
		#define SAMPLE_RATE_MAT_PIN  (4)
		#define SAMPLE_RATE_IOCON  IOCON_PIO1_4
		#define SAMPLE_RATE_IOCON_FUNC  IOCON_FUNC2
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#else
	// Does not exist.
	#pragma GCC error "Selected SAMPLE_RATE timer does not exist."
#endif
*/

void SampleRate_Init(uint32_t default_rate, uint32_t samples_per_period);


#endif // __SAMPLE_RATE_H__
