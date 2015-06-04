/**
* @file
* Definitions for an I2C LCD driver based on the ST7032i chip from Sitronix.
*
* Author: Clemens Valens
* License: GNU General Public License
*
* History
* 2014.09.08  ver 1.00    Preliminary version, first release
*/

#ifndef __LCD_H__
#define __LCD_H__


#include "board.h"


#define LCD_I2C_ADDRESS  (0x3e)

// The speed of the display depends on the frequency of its internal oscillator.
#define ST7032_FOSC  (192000) /* [Hz], can be tweaked if extended mode is available. */
#define ST7032_RESPONSE_TIME_SLOW  (410*1000000/ST7032_FOSC) /* [us] */
#define ST7032_RESPONSE_TIME_NORMAL  (10*1000000/ST7032_FOSC) /* [us] */

#define ST7032_RESET_RECOVERY_TIME  (40) /* [ms] */

// ST7032 commands
// Clear Display
#define ST7032_CMD_CLEAR  (0x01)

// Return Home
#define ST7032_CMD_HOME  (0x02)

// Entry Mode Set
#define ST7032_CMD_ENTRY_MODE  (0x04)
#define ST7032_SHIFT_CURSOR  (0x00) /* default */
#define ST7032_SHIFT_DISPLAY  (0x01)
#define ST7032_DECREMENT  (0x00)
#define ST7032_INCREMENT  (0x02) /* default */

// Display ON/OFF
#define ST7032_CMD_ON_OFF  (0x08)
#define ST7032_BLINK_OFF  (0x00) /* default */
#define ST7032_BLINK_ON  (0x01)
#define ST7032_CURSOR_OFF  (0x00) /* default */
#define ST7032_CURSOR_ON  (0x02)
#define ST7032_DISPLAY_OFF  (0x00) /* default */
#define ST7032_DISPLAY_ON  (0x04)

// Cursor or Display Shift
#define ST7032_CMD_SHIFT  (0x10)
#define ST7032_SHIFT_LEFT  (0x00)
#define ST7032_SHIFT_RIGHT  (0x04)
#define ST7032_SHIFT_CURSOR  (0x00)
#define ST7032_SHIFT_SCREEN  (0x08)

// Function Set
#define ST7032_CMD_FUNCTION  (0x20)
#define ST7032_FUNC_NORMAL  (0x00) /* default */
#define ST7032_FUNC_EXTENDED  (0x01)
#define ST7032_FUNC_HEIGHT_NORMAL  (0x00) /* default */
#define ST7032_FUNC_HEIGHT_DOUBLE  (0x04)
#define ST7032_FUNC_1_LINE  (0x00) /* default */
#define ST7032_FUNC_2_LINES  (0x08)
#define ST7032_FUNC_4_BIT  (0x00)
#define ST7032_FUNC_8_BIT  (0x10) /* default */

#define ST7032_FUNC_CONFIG_NORMAL  (ST7032_FUNC_NORMAL | ST7032_FUNC_HEIGHT_NORMAL | ST7032_FUNC_2_LINES | ST7032_FUNC_8_BIT)
#define ST7032_FUNC_CONFIG_EXTENDED  (ST7032_FUNC_EXTENDED | ST7032_FUNC_HEIGHT_NORMAL | ST7032_FUNC_2_LINES | ST7032_FUNC_8_BIT)
#define ST7032_FUNC_CONFIG_EXTENDED_DOUBLE  (ST7032_FUNC_EXTENDED | ST7032_FUNC_HEIGHT_DOUBLE | ST7032_FUNC_2_LINES | ST7032_FUNC_8_BIT)

// Set CGRAM Address (normal function, ST7032_FUNC_NORMAL)
#define ST7032_CMD_CGRAM_ADDRESS  (0x40)
#define ST7032_CGRAM_ADDRESS_MASK  (0x3f)

// Set DDRAM Address
#define ST7032_CMD_DDRAM_ADDRESS  (0x80)
#define ST7032_DDRAM_ADDRESS_MASK  (0x7f)

// Bias selection/Internal OSC frequency adjust (extended function)
#define ST7032_CMD_BIAS_OSC  (0x10)
#define ST7032_BIAS_020  (0x00) /* default */
#define ST7032_BIAS_025  (0x08)
#define ST7032_OSC_MASK  (0x07) /* defaults to 0x04 */

// Set Icon RAM Address (extended function)
#define ST7032_CMD_ICON_RAM_ADDRESS  (0x40)
#define ST7032_ICON_ADDRESS_MASK  (0x0f)

// Power/ICON control/Contrast set (high byte) (extended function)
#define ST7032_CMD_POWER_ICON_CONTRAST  (0x50)
#define ST7032_BOOSTER_OFF  (0x00) /* default */
#define ST7032_BOOSTER_ON  (0x04)
#define ST7032_ICON_OFF  (0x00) /* default */
#define ST7032_ICON_ON  (0x08)
#define ST7032_CONTRAST_HI_MASK  (0x03) /* defaults to 0x02 */

// Follower control (extended function)
#define ST7032_CMD_FOLLOWER  (0x60)
#define ST7032_FOLLOWER_OFF  (0x00) /* default */
#define ST7032_FOLLOWER_ON  (0x08)
#define ST7032_AMPLIFIER_RATIO_MASK  (0x07) /* defaults to 0x02 */

// Contrast set (low byte) (extended function)
#define ST7032_CMD_CONTRAST_LO  (0x70)
#define ST7032_CONTRAST_LO_MASK  (0x0f) /* defaults to 0x00 */


uint8_t lcd_putc(char ch);
uint8_t lcd_puts(char *p_str);
void lcd_clear(void);
void lcd_cursor_move(uint8_t line, uint8_t column);
void lcd_cursor(uint8_t line, uint8_t column);
void lcd_display_cursor_blink(uint8_t display, uint8_t cursor, uint8_t blink);
void lcd_clear_to_eol(int line, int column);
void lcd_contrast(uint8_t contrast);
void lcd_font(uint8_t font);
void lcd_set_custom_character(uint8_t index, const uint8_t *p_data);
void lcd_bar_graph(uint8_t position, uint8_t value, uint8_t value_max);
void lcd_init(void);


#endif // __LCD_H__
