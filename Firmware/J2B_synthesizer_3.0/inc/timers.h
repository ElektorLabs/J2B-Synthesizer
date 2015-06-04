/*
 * @brief Timer definitions
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

#ifndef __TIMERS_H__
#define __TIMERS_H__


// Timers you can use.
#define TIMER16_0  (0)
#define TIMER16_1  (1)
#define TIMER32_0  (2)
#define TIMER32_1  (3)

#define SAMPLE_RATE_TIMER  TIMER32_0
#define AUDIO_OUT_TIMER  TIMER32_1

#if SAMPLE_RATE_TIMER == AUDIO_OUT_TIMER
#pragma GCC error "Sample rate timer and audio out timer may not be the same."
#endif


#endif // __TIMERS_H__
