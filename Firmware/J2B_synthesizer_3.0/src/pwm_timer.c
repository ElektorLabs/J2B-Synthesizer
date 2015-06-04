/*
 * @brief PWM timer driver
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
#include "pwm_timer.h"


#define PWM_TIMER_DUTY_CYCLE_MAX  (1000)

const pwm_timer_t pwm_timers[PWM_TIMERS] =
{
	{ // 0
		LPC_TIMER16_0,
		TIMER_16_0_IRQn,
		PWM_TIMER_FREQUENCY_REGISTER,
		{
			{ 0, 8, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 9, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 10, IOCON_FUNC3, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ -1, -1, -1, -1 },
		},
	},
	{ // 1
		LPC_TIMER16_1,
		TIMER_16_1_IRQn,
		PWM_TIMER_FREQUENCY_REGISTER,
		{
			{ 0, 21, IOCON_FUNC1, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 22, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ -1, -1, -1, -1 },
			{ -1, -1, -1, -1 },
		},
	},
	{ // 2
		LPC_TIMER32_0,
		TIMER_32_0_IRQn,
		PWM_TIMER_FREQUENCY_REGISTER,
		{
			{ 0, 18, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 19, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0,  1, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 1, 27, IOCON_FUNC1, PWM_TIMER_DEFAULT_DUTY_CYCLE },
		},
	},
	{ // 3
		LPC_TIMER32_1,
		TIMER_32_1_IRQn,
		PWM_TIMER_FREQUENCY_REGISTER,
		{
			{ 0, 13, IOCON_FUNC3, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 14, IOCON_FUNC3, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 15, IOCON_FUNC3, PWM_TIMER_DEFAULT_DUTY_CYCLE },
			{ 0, 16, IOCON_FUNC2, PWM_TIMER_DEFAULT_DUTY_CYCLE },
		},
	}
};


void PwmTimer_Init(const pwm_timer_t *p_timer, uint32_t frequency)
{
	// Setup timer.
	Chip_TIMER_Init(p_timer->timer);
	Chip_TIMER_Reset(p_timer->timer);
	// Register the MRx register that will determine the frequency.
	Chip_TIMER_ResetOnMatchEnable(p_timer->timer,p_timer->frequency_register);
	// Load the frequency register.
	PwmTimer_SetFrequency(p_timer,frequency);
	//PwmTimer_Start(p_timer->timer);
}


void PwmTimer_InitChannel(const pwm_timer_t *p_timer, uint8_t channel, uint32_t duty_cycle)
{
	// Setup pin multiplexer.
	Chip_IOCON_PinMuxSet(LPC_IOCON,p_timer->channel[channel].port,p_timer->channel[channel].pin,IOCON_DIGMODE_EN|IOCON_MODE_INACT|p_timer->channel[channel].function);
	// Make the pin an output.
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,p_timer->channel[channel].port,p_timer->channel[channel].pin,OUTPUT);

	// Activate PWM mode on the duty-cycle register.
	// CPV 09042014: for some reason the LPCOpen library doesn't know about the PWMC register,
	//               so I added it myself.
	p_timer->timer->PWMC |= 1 << channel;
	PwmTimer_SetDutyCycle(p_timer,channel,duty_cycle);
}


void PwmTimer_SetFrequency(const pwm_timer_t *p_timer, uint32_t frequency)
{
	Chip_TIMER_SetMatch(p_timer->timer,p_timer->frequency_register,Chip_Clock_GetSystemClockRate()/frequency);
}


void PwmTimer_SetDutyCycle(const pwm_timer_t *p_timer, uint8_t channel, uint32_t duty_cycle)
{
	if (duty_cycle>PWM_TIMER_DUTY_CYCLE_MAX) duty_cycle = PWM_TIMER_DUTY_CYCLE_MAX;
	Chip_TIMER_SetMatch(p_timer->timer,channel,duty_cycle*p_timer->timer->MR[p_timer->frequency_register]/PWM_TIMER_DUTY_CYCLE_MAX);
}


void PwmTimer_Start(const pwm_timer_t *p_timer)
{
	Chip_TIMER_Enable(p_timer->timer);
}


void PwmTimer_Stop(const pwm_timer_t *p_timer)
{
	Chip_TIMER_Disable(p_timer->timer);
}


void PwmTimer_EnableInterrupt(const pwm_timer_t *p_timer, uint8_t channel)
{
	// Select the match register that will generate interrupts.
	Chip_TIMER_MatchEnableInt(p_timer->timer,channel);
	// Enable timer interrupt.
	NVIC_ClearPendingIRQ(p_timer->irq);
	NVIC_EnableIRQ(p_timer->irq);
}

