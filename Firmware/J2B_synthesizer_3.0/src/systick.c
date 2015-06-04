/*
 * @brief Systick related functions.
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
#include "systick.h"
#include "keyboard.h"

void loop_priority_high(void);

volatile uint32_t systick_counter = 0;
static volatile uint16_t systick_rollover_counter = 0;
static volatile uint32_t systick_delay_counter = 0;
static volatile uint16_t systick_seconds_counter = 0;
volatile uint32_t seconds_counter = 0;
static volatile uint16_t systick_led1_red_counter = 0;
static volatile uint16_t systick_led1_green_counter = 0;
static volatile uint16_t systick_led3_counter = 0;

#define LED_BLINK_RATE  (SYSTICK_RATE_HZ/4)
volatile uint32_t systick_led_counter = LED_BLINK_RATE;

void SysTick_Handler(void)
{
	//Board_LED_Set(BOARD_LED1_GREEN,true);
	if (systick_counter==0xffffffff) systick_rollover_counter += 1;
	systick_counter += 1;

	if (systick_delay_counter!=0) systick_delay_counter -= 1;

	systick_seconds_counter += 1;
	if (systick_seconds_counter>=SYSTICK_RATE_HZ)
	{
		systick_seconds_counter = 0;
		SysTick_Seconds();
	}

	systick_led_counter -= 1;
	if (systick_led_counter==0)
	{
		systick_led_counter = LED_BLINK_RATE;
		//Board_LED_Toggle(BOARD_LED1_RED);
	}

//	loop_priority_high();
	keyboard_scan_encoders();
//	Board_LED_Set(BOARD_LED1_GREEN,false);

	if (systick_led1_green_counter>0)
	{
		systick_led1_green_counter -= 1;
		if (systick_led1_green_counter==0)
		{
			Board_LED_Set(BOARD_LED1_GREEN,BOARD_LED_OFF);
		}
	}

	if (systick_led1_red_counter>0)
	{
		systick_led1_red_counter -= 1;
		if (systick_led1_red_counter==0)
		{
			Board_LED_Set(BOARD_LED1_RED,BOARD_LED_OFF);
		}
	}

	if (systick_led3_counter>0)
	{
		systick_led3_counter -= 1;
		if (systick_led3_counter==0)
		{
			Board_LED_Set(BOARD_LED3,0);
		}
	}
}


void SysTick_Seconds(void)
{
	seconds_counter += 1;
	//Board_LED_Toggle(BOARD_LED0);
}


void SysTick_Delay(uint32_t ticks)
{
	// Blocking delay.
	if (ticks!=0)
	{
		systick_delay_counter = ticks;
		while (systick_delay_counter>0);
	}
}


void SysTick_LED_Flash(uint8_t led, uint32_t ticks)
{
	if (ticks>0)
	{
		if (led==BOARD_LED1_GREEN)
		{
			systick_led1_green_counter = ticks;
			Board_LED_Set(BOARD_LED1_GREEN,BOARD_LED_ON);
		}
		else if (led==BOARD_LED1_RED)
		{
			systick_led1_red_counter = ticks;
			Board_LED_Set(BOARD_LED1_RED,BOARD_LED_ON);
		}
		else if (led==BOARD_LED3)
		{
			systick_led3_counter = ticks;
			Board_LED_Set(BOARD_LED3,1);
		}
	}
}
