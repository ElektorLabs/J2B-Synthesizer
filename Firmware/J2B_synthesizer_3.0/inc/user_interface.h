/*
 * @brief User interface
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

#ifndef __DISPLAY_H__
#define __DISPLAY_H__


#define PAGE_SPLASH  (0)
#define PAGE_FUNCTION  (1)
#define PAGE_CTRL  (2)
#define FUNCTION_VALUE_OFFSET  (13)


void display_init(void);
uint8_t display_print_value(uint8_t line, uint8_t position, int value, boolean ignore_sign);
uint8_t display_print_value_f(uint8_t line, uint8_t position, float value, uint8_t decimals);
void display_page_splash(void);
void display_page_function(void);
void display_page_ctrl(void);
boolean display_page_set(uint8_t page, uint8_t encoder);
uint8_t display_page_get(void);
void display_draw(boolean force_redraw);
void display_invalidate(void);


#endif // __DISPLAY_H__
