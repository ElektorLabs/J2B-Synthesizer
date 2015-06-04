/*
 * @brief NXP LPCXpresso 1347 board file
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

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_NXP_LPCXPRESSO_1347 NXP LPC1347 Xpresso board support functions
 * @ingroup LPCOPEN_13XX_BOARD_LPCXPRESSO_1347
 * @{
 */

/** @defgroup BOARD_NXP_LPCXPRESSO_1347_OPTIONS BOARD: NXP LPC1347 Xpresso board build options
 * This board has options that configure its operation at build-time.<br>
 * @{
 */
 
 /** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
	out of the code at build time.
 */
//#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
	semihosting with this option.
 */
//#define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
	Board_UARTPutSTR functions.
 */
#define DEBUG_UART LPC_USART

/**
 * @}
 */

/* Board name */
#define BOARD_ELEKTOR_140182

#define OUTPUT  true
#define INPUT  false

// LED(s).
#define BOARD_LED1_GREEN_PORT  (0)
#define BOARD_LED1_GREEN_PIN  (18)
#define BOARD_LED1_RED_PORT  (0)
#define BOARD_LED1_RED_PIN  (9)
#define BOARD_LED2_GREEN_PORT  (0)
#define BOARD_LED2_GREEN_PIN  (10)
#define BOARD_LED2_RED_PORT  (0)
#define BOARD_LED2_RED_PIN  (8)
#define BOARD_LED3_PORT  (1)
#define BOARD_LED3_PIN  (31)
#define BOARD_LED1_GREEN  (0)
#define BOARD_LED1_RED  (1)
#define BOARD_LED2_GREEN  (2)
#define BOARD_LED2_RED  (3)
#define BOARD_LED3  (4)
#define BOARD_LED_OFF  (true)
#define BOARD_LED_ON  (false)
// The other LED function prototypes are in board_api.h
void Board_LED_Init(void);


// LCD (2x16) with I2C interface, based on ST7032i chip.
#define BOARD_LCD_LINES  (2)
#define BOARD_LCD_CHARS_PER_LINE  (16)
#define BOARD_LCD_RESET_PORT  (1)
#define BOARD_LCD_RESET_PIN  (15)
#define BOARD_LCD_RESET_ON  (0)
#define BOARD_LCD_RESET_OFF  (1)
#define BOARD_LCD_BACKLIGHT_PORT  (1)
#define BOARD_LCD_BACKLIGHT_PIN  (16)
#define BOARD_LCD_BACKLIGHT_ON  (0)
#define BOARD_LCD_BACKLIGHT_OFF  (1)
void Board_LCD_Init(void);
void Board_LCD_ResetAssert(void);
void Board_LCD_ResetDeassert(void);
void Board_LCD_SetBacklight(uint8_t Intensity);


// Keyboard.
#define BOARD_KEYBOARD_R0_PORT  (1)
#define BOARD_KEYBOARD_R0_PIN  (23)
#define BOARD_KEYBOARD_R1_PORT  (1)
#define BOARD_KEYBOARD_R1_PIN  (24)
#define BOARD_KEYBOARD_R2_PORT  (1)
#define BOARD_KEYBOARD_R2_PIN  (25)
#define BOARD_KEYBOARD_C0_PORT  (1)
#define BOARD_KEYBOARD_C0_PIN  (26)
#define BOARD_KEYBOARD_C1_PORT  (1)
#define BOARD_KEYBOARD_C1_PIN  (28)
#define BOARD_KEYBOARD_C2_PORT  (1)
#define BOARD_KEYBOARD_C2_PIN  (29)

#define BOARD_KEYBOARD_ISP_PORT  (0)
#define BOARD_KEYBOARD_ISP_PIN  (1)

#define BOARD_KEYBOARD_RESET_PORT  (0)
#define BOARD_KEYBOARD_RESET_PIN  (0)
// Uncomment if you don't need a reset pin in your system.
//#define RESET_IS_GPIO

// ---------------------------------------------------
//  Keys.
// ---------------------------------------------------
#define BOARD_KEYBOARD_S00  (0)  /* S1 push */
#define BOARD_KEYBOARD_S01  (1)  /* S2 push */
#define BOARD_KEYBOARD_S02  (2)  /* S3 push */
#define BOARD_KEYBOARD_S10  (3)  /* S4 push */
#define BOARD_KEYBOARD_S11  (4)  /* S5 push */
#define BOARD_KEYBOARD_S12  (5)  /* S6 push */
#define BOARD_KEYBOARD_S20  (6)  /* S7 push */
#define BOARD_KEYBOARD_S21  (7)  /* S8 push */
#define BOARD_KEYBOARD_S22  (8)  /* S11 (spare) */
#define BOARD_KEYBOARD_ISP  (9)  /* S9 */
#if defined RESET_IS_GPIO
#define BOARD_KEYBOARD_RESET  (10)  /* S10 */
#endif
// Other non-matrix keys go here.

// --------------------------------------------------------------
//  Rotary encoders.
// --------------------------------------------------------------
#define BOARD_KEYBOARD_RE0  /* S1 */
#define BOARD_KEYBOARD_RE1  /* S2 */
#define BOARD_KEYBOARD_RE2  /* S3 */
#define BOARD_KEYBOARD_RE3  /* S4 */
#define BOARD_KEYBOARD_RE4  /* S5 */
#define BOARD_KEYBOARD_RE5  /* S6 */
#define BOARD_KEYBOARD_RE6  /* S7 */
#define BOARD_KEYBOARD_RE7  /* S8 */

#if defined(BOARD_KEYBOARD_RE0)  /* S1 */
#define BOARD_KEYBOARD_RE0_A_PORT  (1)
#define BOARD_KEYBOARD_RE0_A_PIN  (19)
#define BOARD_KEYBOARD_RE0_B_PORT  (1)
#define BOARD_KEYBOARD_RE0_B_PIN  (20)
#endif

#if defined(BOARD_KEYBOARD_RE1)  /* S2 */
#define BOARD_KEYBOARD_RE1_A_PORT  (1)
#define BOARD_KEYBOARD_RE1_A_PIN  (21)
#define BOARD_KEYBOARD_RE1_B_PORT  (1)
#define BOARD_KEYBOARD_RE1_B_PIN  (22)
#endif

#if defined(BOARD_KEYBOARD_RE2)  /* S3 */
#define BOARD_KEYBOARD_RE2_A_PORT  (0)
#define BOARD_KEYBOARD_RE2_A_PIN  (2)
#define BOARD_KEYBOARD_RE2_B_PORT  (0)
#define BOARD_KEYBOARD_RE2_B_PIN  (11)
#endif

#if defined(BOARD_KEYBOARD_RE3)  /* S4 */
#define BOARD_KEYBOARD_RE3_A_PORT  (0)
#define BOARD_KEYBOARD_RE3_A_PIN  (7)
#define BOARD_KEYBOARD_RE3_B_PORT  (0)
#define BOARD_KEYBOARD_RE3_B_PIN  (12)
#endif

#if defined(BOARD_KEYBOARD_RE4)  /* S5 */
#define BOARD_KEYBOARD_RE4_A_PORT  (0)
#define BOARD_KEYBOARD_RE4_A_PIN  (17)
#define BOARD_KEYBOARD_RE4_B_PORT  (0)
#define BOARD_KEYBOARD_RE4_B_PIN  (14)
#endif

#if defined(BOARD_KEYBOARD_RE5)  /* S6 */
#define BOARD_KEYBOARD_RE5_A_PORT  (0)
#define BOARD_KEYBOARD_RE5_A_PIN  (19)
#define BOARD_KEYBOARD_RE5_B_PORT  (0)
#define BOARD_KEYBOARD_RE5_B_PIN  (16)
#endif

#if defined(BOARD_KEYBOARD_RE6)  /* S7 */
#define BOARD_KEYBOARD_RE6_A_PORT  (0)
#define BOARD_KEYBOARD_RE6_A_PIN  (20)
#define BOARD_KEYBOARD_RE6_B_PORT  (0)
#define BOARD_KEYBOARD_RE6_B_PIN  (22)
#endif

#if defined(BOARD_KEYBOARD_RE7)  /* S8 */
#define BOARD_KEYBOARD_RE7_A_PORT  (0)
#define BOARD_KEYBOARD_RE7_A_PIN  (21)
#define BOARD_KEYBOARD_RE7_B_PORT  (0)
#define BOARD_KEYBOARD_RE7_B_PIN  (23)
#endif

#define BOARD_KEYBOARD_REPEAT_DELAY  (500) /* [ms] */
#define BOARD_KEYBOARD_REPEAT_RATE  (100) /* [ms] */
#define BOARD_KEYBOARD_DOUBLE_CLICK_MIN (100)  /* [ms] */
#define BOARD_KEYBOARD_DOUBLE_CLICK_MAX (BOARD_KEYBOARD_DOUBLE_CLICK_MIN+300)  /* [ms] */


/**
 * Button defines
 */
#define BUTTONS_BUTTON1     0x01
#define NO_BUTTON_PRESSED   0x00


/**
 * @brief	Initialize buttons on the board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Get button status
 * @return	status of button
 */
uint32_t Buttons_GetStatus(void);

/**
 * @brief	Sets up board specific ADC interface
 * @return	Nothing
 */
void Board_ADC_Init(void);

/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
