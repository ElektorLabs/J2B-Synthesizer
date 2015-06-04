/*
 * @brief Sample rate related timer stuff.
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

#include "board.h"
#include "sample_rate.h"

/*
#if SAMPLE_RATE_TIMER_NR == TIMER16_0
void TIMER16_0_IRQHandler(void)
#elif SAMPLE_RATE_TIMER_NR == TIMER16_1
void TIMER16_1_IRQHandler(void)
#elif SAMPLE_RATE_TIMER_NR == TIMER32_0
void TIMER32_0_IRQHandler(void)
#elif SAMPLE_RATE_TIMER_NR == TIMER32_1
void TIMER32_1_IRQHandler(void)
#else
	// Does not exist.
	#pragma GCC error "Selected SAMPLE_RATE timer does not exist."
void SampleRate_DummyIRQHandler(void)
#endif
{
	if (Chip_TIMER_MatchPending(SAMPLE_RATE_TIMER,SAMPLE_RATE_FREQUENCY_REGISTER))
	{
		Chip_TIMER_ClearMatch(SAMPLE_RATE_TIMER,SAMPLE_RATE_FREQUENCY_REGISTER);
		Board_LED_Toggle(0);
	}
}


void SampleRate_Init(uint32_t default_rate, uint32_t samples_per_period)
{
	// Setup pin multiplexer.
	Chip_IOCON_PinMuxSet(LPC_IOCON,SAMPLE_RATE_IOCON,IOCON_DIGMODE_EN|IOCON_MODE_INACT|SAMPLE_RATE_IOCON_FUNC);
	// Make the pin an output.
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,SAMPLE_RATE_MAT_PORT,SAMPLE_RATE_MAT_PIN,true);

	Chip_TIMER_Init(SAMPLE_RATE_TIMER);
	Chip_TIMER_Reset(SAMPLE_RATE_TIMER);
	Chip_TIMER_MatchEnableInt(SAMPLE_RATE_TIMER,SAMPLE_RATE_FREQUENCY_REGISTER);
	Chip_TIMER_SetMatch(SAMPLE_RATE_TIMER,SAMPLE_RATE_FREQUENCY_REGISTER,Chip_Clock_GetSystemClockRate()/(default_rate*samples_per_period));
	Chip_TIMER_ResetOnMatchEnable(SAMPLE_RATE_TIMER,SAMPLE_RATE_FREQUENCY_REGISTER);
	Chip_TIMER_Enable(SAMPLE_RATE_TIMER);

	// Enable timer interrupt.
	NVIC_ClearPendingIRQ(SAMPLE_RATE_TIMER_IRQ);
	NVIC_EnableIRQ(SAMPLE_RATE_TIMER_IRQ);
}
*/
