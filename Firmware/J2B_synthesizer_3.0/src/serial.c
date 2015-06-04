/*
 * @brief Serial port driver
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
#include "atmegatron.h"


size_t serial_write(uint8_t value);
int serial_read(void);
void serial_flush(void);
int serial_available(void);
void serial_begin(uint32_t baudrate);


const serial_t Serial =
{
	serial_write,
	serial_read,
	serial_flush,
	serial_available,
	serial_begin
};


// Transmit and receive ring buffers.
STATIC RINGBUFF_T txring;
STATIC RINGBUFF_T rxring;

// Transmit and receive ring buffer sizes.
#define UART_TX_RBUFFER_SIZE  (64)
#define UART_RX_RBUFFER_SIZE  (64)

// Transmit and receive buffers.
static uint8_t rxbuff[UART_RX_RBUFFER_SIZE];
static uint8_t txbuff[UART_TX_RBUFFER_SIZE];


static void Init_UART_PinMux(void)
{
	// Pin muxing for serial port is already set by board layer library.
	// (See board_sysinit.c files in  J2B_board/src/)
}


void UART_IRQHandler(void)
{
	// Want to handle any errors? Do it here.

	// Use default ring buffer handler.
	Chip_UART_IRQRBHandler(LPC_USART,&rxring,&txring);
}


size_t serial_write(uint8_t value)
{
	Chip_UART_SendRB(LPC_USART,&txring,(const uint8_t *)&value,1);
	return 1;
}


int serial_read(void)
{
	uint8_t value;
	Chip_UART_ReadRB(LPC_USART,&rxring,&value,1);
	return (int)value;
}


void serial_flush(void)
{
	RingBuffer_Flush(&txring);
}


int serial_available(void)
{
	return RingBuffer_GetCount(&rxring);
}


void serial_begin(uint32_t baudrate)
{
	Init_UART_PinMux();

	// Setup UART.
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART,baudrate);
	Chip_UART_ConfigData(LPC_USART,(UART_LCR_WLEN8|UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART,(UART_FCR_FIFO_EN|UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);

	// Before using the ring buffers, initialize them using the ring buffer init function.
	RingBuffer_Init(&rxring,rxbuff,1,UART_RX_RBUFFER_SIZE);
	RingBuffer_Init(&txring,txbuff,1,UART_TX_RBUFFER_SIZE);

	// Enable receive data and line status interrupt, the transmit interrupt is handled by the driver.
	Chip_UART_IntEnable(LPC_USART,(UART_IER_RBRINT|UART_IER_RLSINT));

	// preemption = 1, sub-priority = 1.
	// 0 is highest priority, 32 is lowest.
	// All interrupt priorities default to 0 at power up.
	NVIC_SetPriority(UART0_IRQn,2);
	NVIC_EnableIRQ(UART0_IRQn);
}
