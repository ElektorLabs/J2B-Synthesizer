/*
 * @brief LPCXpresso 1347 Sysinit file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "string.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* Pin muxing table, only items that need changing from their default pin
   state are in this table. */
STATIC const PINMUX_GRP_T pinmuxing[] = {
#if defined (BOARD_ELEKTOR_140182)
	{ 0,  3, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7) }, /* PIO0_3 used for USB_VBUS */
	{ 0,  4, (IOCON_FUNC1 | IOCON_SFI2C_EN) }, /* PIO0_4 used for SCL */
	{ 0,  5, (IOCON_FUNC1 | IOCON_SFI2C_EN) }, /* PIO0_5 used for SDA */
	{ 0,  6, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7) }, /* PIO0_6 used for USB_CONNECT */
	{ 0, 11, (IOCON_FUNC1 | IOCON_DIGMODE_EN) }, /* PIO0_11 used for GPIO */
	{ 0, 12, (IOCON_FUNC1 | IOCON_DIGMODE_EN) }, /* PIO0_12 used for GPIO */
	{ 0, 13, (IOCON_FUNC3 | IOCON_DIGMODE_EN) }, /* PIO0_13 used for CT32B1_MAT0 */
	{ 0, 14, (IOCON_FUNC1 | IOCON_DIGMODE_EN) }, /* PIO0_14 used for GPIO */
	{ 0, 10, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7) }, /* PIO0_10 used for GPIO */
	{ 0, 15, (IOCON_FUNC3 | IOCON_DIGMODE_EN) }, /* PIO0_15 used for CT32B1_MAT2 */
	{ 1, 13, (IOCON_FUNC3 | IOCON_RESERVED_BIT_7) }, /* PIO1_13 used for TXD */
	{ 1, 14, (IOCON_FUNC3 | IOCON_RESERVED_BIT_7) }, /* PIO1_14 used for RXD */
	{ 1, 27, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7) }, /* PIO1_27 used for CT32B0_MAT3 */
#else
	{0,  1,  (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_1 used for CLKOUT */
	{0,  2,  (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_PULLUP)},	/* PIO0_2 used for SSEL */
	{0,  3,  (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_3 used for USB_VBUS */
	{0,  4,  (IOCON_FUNC1 | IOCON_FASTI2C_EN)},							/* PIO0_4 used for SCL */
	{0,  5,  (IOCON_FUNC1 | IOCON_FASTI2C_EN)},							/* PIO0_5 used for SDA */
	{0,  6,  (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_6 used for USB_CONNECT */
	{0,  8,  (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_8 used for MISO0 */
	{0,  9,  (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_9 used for MOSI0 */
	{0,  11, (IOCON_FUNC2 | IOCON_ADMODE_EN		 | IOCON_FILT_DIS)},	/* PIO0_11 used for AD0 */
	{0,  18, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_18 used for RXD */
	{0,  19, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO0_19 used for TXD */
	{1,  29, (IOCON_FUNC1 | IOCON_RESERVED_BIT_7 | IOCON_MODE_INACT)},	/* PIO1_29 used for SCK0 */
#endif
};

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	/* Enable IOCON clock */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}

/* Set up and initialize clocking prior to call to main */
void Board_SetupClocking(void)
{
	Chip_SetupXtalClocking();
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	/* Booting from FLASH, so remap vector table to FLASH */
	Chip_SYSCTL_Map(REMAP_USER_FLASH_MODE);

	/* Setup system clocking and muxing */
	Board_SetupMuxing();
	Board_SetupClocking();
}
