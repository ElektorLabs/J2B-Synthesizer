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

#ifndef __PWM_TIMER_H__
#define __PWM_TIMER_H__


#include "timers.h"

/*
// Choose the MR register that will determine the frequency.
#define PWM_FREQUENCY_REGISTER  (3)
// Choose the MR register that will determine the duty-cycle.
#define PWM_DUTY_CYCLE_REGISTER  (0)

#if PWM_FREQUENCY_REGISTER == PWM_DUTY_CYCLE_REGISTER
#pragma GCC error "Frequency register and duty-cycle register may not be the same."
#endif

#define PWM_MAT_OUTPUT  PWM_DUTY_CYCLE_REGISTER

#if PWM_TIMER_NR == TIMER16_0
	#define PWM_TIMER  LPC_TIMER16_0
	#define PWM_TIMER_IRQ  (TIMER_16_0_IRQn)
	#if PWM_MAT_OUTPUT == 0
		#define PWM_MAT_PORT  (0)
		#define PWM_MAT_PIN  (8)
		#define PWM_IOCON  IOCON_PIO0_8
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#elif PWM_MAT_OUTPUT == 1
		#define PWM_MAT_PORT  (0)
		#define PWM_MAT_PIN  (9)
		#define PWM_IOCON  IOCON_PIO0_9
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#elif PWM_MAT_OUTPUT == 2
		#define PWM_MAT_PORT  (0)
		#define PWM_MAT_PIN  (10)
		#define PWM_IOCON  IOCON_PIO0_10
		#define PWM_IOCON_FUNC  IOCON_FUNC3
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#elif PWM_TIMER_NR == TIMER16_1
	#define PWM_TIMER  LPC_TIMER16_1
	#define PWM_TIMER_IRQ  (TIMER_16_1_IRQn)
	#if PWM_MAT_OUTPUT == 0
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (9)
		#define PWM_IOCON  IOCON_PIO1_9
		#define PWM_IOCON_FUNC  IOCON_FUNC1
	#elif PWM_MAT_OUTPUT == 1
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (10)
		#define PWM_IOCON  IOCON_PIO1_10
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#elif PWM_TIMER_NR == TIMER32_0
	#define PWM_TIMER  LPC_TIMER32_0
	#define PWM_TIMER_IRQ  (TIMER_32_0_IRQn)
	#if PWM_MAT_OUTPUT == 0
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (6)
		#define PWM_IOCON  IOCON_PIO1_6
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#elif PWM_MAT_OUTPUT == 1
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (7)
		#define PWM_IOCON  IOCON_PIO1_7
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#elif PWM_MAT_OUTPUT == 2
		#define PWM_MAT_PORT  (0)
		#define PWM_MAT_PIN  (1)
		#define PWM_IOCON  IOCON_PIO0_1
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#elif PWM_MAT_OUTPUT == 3
		#define PWM_MAT_PORT  (0)
		#define PWM_MAT_PIN  (11)
		#define PWM_IOCON  IOCON_PIO0_11
		#define PWM_IOCON_FUNC  IOCON_FUNC3
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#elif PWM_TIMER_NR == TIMER32_1
	#define PWM_TIMER  LPC_TIMER32_1
	#define PWM_TIMER_IRQ  (TIMER_32_1_IRQn)
	#if PWM_MAT_OUTPUT == 0
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (1)
		#define PWM_IOCON  IOCON_PIO1_1
		#define PWM_IOCON_FUNC  IOCON_FUNC3
	#elif PWM_MAT_OUTPUT == 1
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (2)
		#define PWM_IOCON  IOCON_PIO1_2
		#define PWM_IOCON_FUNC  IOCON_FUNC3
	#elif PWM_MAT_OUTPUT == 2
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (3)
		#define PWM_IOCON  IOCON_PIO1_3
		#define PWM_IOCON_FUNC  IOCON_FUNC3
	#elif PWM_MAT_OUTPUT == 3
		#define PWM_MAT_PORT  (1)
		#define PWM_MAT_PIN  (4)
		#define PWM_IOCON  IOCON_PIO1_4
		#define PWM_IOCON_FUNC  IOCON_FUNC2
	#else
		// Does not exist.
		#pragma GCC error "Match output pin MATx does not exist."
	#endif
#else
	// Does not exist.
	#pragma GCC error "Selected PWM timer does not exist."
#endif
*/

typedef struct
{
	uint8_t port;
	uint8_t pin;
	uint32_t function;
	uint32_t duty_cycle;
}
pwm_timer_channel_t;

typedef struct
{
	LPC_TIMER_T *timer;
	LPC1347_IRQn_Type irq;
	uint8_t frequency_register;
	pwm_timer_channel_t channel[4];
}
pwm_timer_t;

// The datasheet recommends to use MR3 as the PWM frequency register.
#define PWM_TIMER_FREQUENCY_REGISTER  (3)
#define PWM_TIMER_DEFAULT_DUTY_CYCLE  (50)

#define PWM_TIMERS  (4)
extern const pwm_timer_t pwm_timers[PWM_TIMERS];


void PwmTimer_Init(const pwm_timer_t *p_timer, uint32_t frequency);
void PwmTimer_InitChannel(const pwm_timer_t *p_timer, uint8_t channel, uint32_t duty_cycle);
void PwmTimer_SetFrequency(const pwm_timer_t *p_timer, uint32_t frequency);
void PwmTimer_SetDutyCycle(const pwm_timer_t *p_timer, uint8_t channel, uint32_t duty_cycle);
void PwmTimer_Start(const pwm_timer_t *p_timer);
void PwmTimer_Stop(const pwm_timer_t *p_timer);
void PwmTimer_EnableInterrupt(const pwm_timer_t *p_timer, uint8_t channel);


#endif // __PWM_TIMER_H__
