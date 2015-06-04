/*
 * @brief LPCXpresso 1347 board file
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
#include "retarget.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#if defined (BOARD_ELEKTOR_140182)
	#define BUTTONS_BUTTON1_GPIO_PORT_NUM           BOARD_KEYBOARD_ISP_PORT
	#define BUTTONS_BUTTON1_GPIO_BIT_NUM            BOARD_KEYBOARD_ISP_PIN
#else
	#define BUTTONS_BUTTON1_GPIO_PORT_NUM           0
	#define BUTTONS_BUTTON1_GPIO_BIT_NUM            16
#endif


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize pin muxing for UART interface */
void Board_UART_Init(void)
{
	/* Pin Muxing has already been done during SystemInit */
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_ENABLE)
	/* Wait for space in FIFO */
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_ENABLE)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_ENABLE)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_ENABLE)
	Board_UART_Init();

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, 115200);
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);
#endif
}

/* Initializes board LED(s) */
void Board_LED_Init(void)
{
	// Set PIO0_8, _9, _10 & _18 as output.
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LED2_RED_PORT,BOARD_LED2_RED_PIN,OUTPUT);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LED1_RED_PORT,BOARD_LED1_RED_PIN,OUTPUT);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LED2_GREEN_PORT,BOARD_LED2_GREEN_PIN,OUTPUT);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LED1_GREEN_PORT,BOARD_LED1_GREEN_PIN,OUTPUT);
	// Set PIO1_31 as output.
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LED3_PORT,BOARD_LED3_PIN,OUTPUT);
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber==BOARD_LED1_GREEN)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LED1_GREEN_PORT,BOARD_LED1_GREEN_PIN,On);
	}
	else if (LEDNumber==BOARD_LED1_RED)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LED1_RED_PORT,BOARD_LED1_RED_PIN,On);
	}
	else if (LEDNumber==BOARD_LED2_GREEN)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LED2_GREEN_PORT,BOARD_LED2_GREEN_PIN,On);
	}
	else if (LEDNumber==BOARD_LED2_RED)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LED2_RED_PORT,BOARD_LED2_RED_PIN,On);
	}
	else if (LEDNumber==BOARD_LED3)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LED3_PORT,BOARD_LED3_PIN,!On); // LED3 is inverted.
	}
}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
	bool state = false;

	if (LEDNumber==BOARD_LED1_GREEN)
	{
		state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BOARD_LED1_GREEN_PORT,BOARD_LED1_GREEN_PIN);
	}
	else if (LEDNumber==BOARD_LED1_RED)
	{
		state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BOARD_LED1_RED_PORT,BOARD_LED1_RED_PIN);
	}
	else if (LEDNumber==BOARD_LED2_GREEN)
	{
		state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BOARD_LED2_GREEN_PORT,BOARD_LED2_GREEN_PIN);
	}
	else if (LEDNumber==BOARD_LED2_RED)
	{
		state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BOARD_LED2_RED_PORT,BOARD_LED2_RED_PIN);
	}
	else if (LEDNumber==BOARD_LED3)
	{
		state = !Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BOARD_LED3_PORT,BOARD_LED3_PIN);  // LED3 is inverted.
	}

	return state;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	Board_LED_Set(LEDNumber,!Board_LED_Test(LEDNumber));
}


void Board_LCD_Init(void)
{
	// Setup backlight control.
	Chip_IOCON_PinMuxSet(LPC_IOCON,BOARD_LCD_BACKLIGHT_PORT,BOARD_LCD_BACKLIGHT_PIN,IOCON_DIGMODE_EN);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LCD_BACKLIGHT_PORT,BOARD_LCD_BACKLIGHT_PIN,OUTPUT);

	// Setup reset control.
	Chip_IOCON_PinMuxSet(LPC_IOCON,BOARD_LCD_RESET_PORT,BOARD_LCD_RESET_PIN,IOCON_DIGMODE_EN);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,BOARD_LCD_RESET_PORT,BOARD_LCD_RESET_PIN,OUTPUT);
}


void Board_LCD_ResetAssert(void)
{
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LCD_RESET_PORT,BOARD_LCD_RESET_PIN,false);
}


void Board_LCD_ResetDeassert(void)
{
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LCD_RESET_PORT,BOARD_LCD_RESET_PIN,true);
}


void Board_LCD_SetBacklight(uint8_t Intensity)
{
	if (Intensity!=0) Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LCD_BACKLIGHT_PORT,BOARD_LCD_BACKLIGHT_PIN,BOARD_LCD_BACKLIGHT_ON);
	else Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BOARD_LCD_BACKLIGHT_PORT,BOARD_LCD_BACKLIGHT_PIN,BOARD_LCD_BACKLIGHT_OFF);
}


/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initialize GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* Initialize LEDs */
	Board_LED_Init();
	Board_LED_Set(BOARD_LED1_GREEN,BOARD_LED_OFF);
	Board_LED_Set(BOARD_LED1_RED,BOARD_LED_OFF);
	Board_LED_Set(BOARD_LED2_GREEN,BOARD_LED_OFF);
	Board_LED_Set(BOARD_LED2_RED,BOARD_LED_OFF);
	Board_LED_Set(BOARD_LED3,BOARD_LED_ON); // LED3 is inverted compared to LED1 & LED2.

	// Initialize LCD.
	Board_LCD_Init();
	// Put it in reset.
	Board_LCD_ResetAssert();
	// Backlight off.
	Board_LCD_SetBacklight(false);
}

/* Initialize pin muxing for SSP interface */
void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP0) {
	}
	else {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 23, (IOCON_FUNC2 | IOCON_MODE_PULLUP)); /* PIO1_23 connected to SSEL1 */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 20, (IOCON_FUNC2 | IOCON_MODE_PULLUP)); /* PIO1_20 connected to SCK1 */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, (IOCON_FUNC2 | IOCON_MODE_PULLUP)); /* PIO1_21 connected to MISO1 */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 22, (IOCON_FUNC2 | IOCON_MODE_PULLUP)); /* PIO1_22 connected to MOSI1 */
	}
}

/* Configure pin for ADC channel 0 */
void Board_ADC_Init(void)
{
	/* Muxing already setup as part of SystemInit for AD0 */
}

/* Initialize buttons on the board */
void Board_Buttons_Init(void)
{
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM, false);
}

/* Get button status */
uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;
	if (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM) == 0x00) {
		ret |= BUTTONS_BUTTON1;
	}
	return ret;
}

