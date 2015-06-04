/**
* @file
* Implementation of a portable LCD driver.
*
* You have to provide two functions to interface this driver
* to the actual hardware:
*
* void lcd_4bit_write_port(uint8_t value);
*
* uint8_t lcd_4bit_read_port(void);
*
* void lcd_backlight(uint8_t on);
*
* Author: Clemens Valens
* License: GNU General Public License
*
* History
* 2011.04.11  ver 1.00    Preliminary version, first release
*/

#include "atmegatron.h"


const uint8_t custom_characters[8][8] =
{
	/*{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
	{ 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f }*/
#if PRODUCT==ATMEGADRUM
	{ 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00 }, // 0, single dot
	{ 0x00, 0x0e, 0x11, 0x15, 0x11, 0x0e, 0x00, 0x00 }, // 1, small circle with dot
	{ 0x15, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x15, 0x00 }, // 2, large star
	{ 0x0e, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0e, 0x00 }, // 3, filled 'O'
	{ 0x00, 0x0e, 0x1f, 0x1f, 0x1f, 0x0e, 0x00, 0x00 }, // 4, filled small circle
	{ 0x0e, 0x11, 0x0e, 0x0a, 0x0e, 0x11, 0x0e, 0x00 }, // 5, filled 'O' with inverted small circle
	{ 0x00, 0x00, 0x0a, 0x04, 0x0a, 0x00, 0x00, 0x00 }, // 6, small x
	{ 0x0e, 0x1f, 0x15, 0x1b, 0x15, 0x1f, 0x0e, 0x00 }, // 7, filled 'O' with inverted small x
#else /* PRODUCT */
	{ 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 },
	{ 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1f, 0x04 },
	{ 0x04, 0x04, 0x04, 0x04, 0x04, 0x1f, 0x04, 0x04 },
	{ 0x04, 0x04, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x04 },
	{ 0x04, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x04, 0x04 },
	{ 0x04, 0x04, 0x04, 0x0e, 0x0e, 0x1f, 0x1f, 0x04 },
	{ 0x04, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 },
	{ 0x1f, 0x1f, 0x0e, 0x0e, 0x04, 0x04, 0x04, 0x04 }
#endif /* PRODUCT */
};


// State machine handler for I2C0 and I2C1.
static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id))
	{
		Chip_I2C_MasterStateHandler(id);
	}
	else
	{
		Chip_I2C_SlaveStateHandler(id);
	}
}


void I2C_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}


void st7032i_write_byte(uint8_t cmd_or_data, uint8_t value)
{
	// buffer must be static because the I2C engine does not copy the data,
	// but uses a pointer to buffer.
	static uint8_t buffer[2];
	buffer[0] = cmd_or_data;
	buffer[1] = value;
	Chip_I2C_MasterSend(I2C0,LCD_I2C_ADDRESS,buffer,2);
}


void st7032i_write_data(uint8_t value)
{
	// Bit 6 (the RS bit) must be set (1) in order to write the Data Register (DR).
	// With bit 7 cleared (the Co bit) only one control byte can be sent.
	st7032i_write_byte(0x40,value);
}


void st7032i_write_command(uint8_t cmd, uint8_t value)
{
	// Bit 6 (the RS bit) must be cleared (0) in order to write the Instruction Register (IR).
	// With bit 7 cleared (the Co bit) only one control byte can be sent.
	st7032i_write_byte(0x00,cmd|value);
}


uint8_t lcd_putc(char ch)
{
	st7032i_write_data(ch);
	return 1;
}


uint8_t lcd_puts(char *p_str)
{
	uint8_t result = 0;

	while (*p_str!=0)
	{
		st7032i_write_data(*p_str++);
		result += 1;
	}

	return result;
}


void lcd_clear(void)
{
	st7032i_write_command(ST7032_CMD_CLEAR,0);
	SysTick_Delay(2);
}


void lcd_cursor_move(uint8_t line, uint8_t column)
{
	st7032i_write_command(ST7032_CMD_DDRAM_ADDRESS,(line==0?0:0x40)+column);
	lcd_display_cursor_blink(1,0,0);
}


void lcd_cursor(uint8_t line, uint8_t column)
{
	st7032i_write_command(ST7032_CMD_DDRAM_ADDRESS,(line==0?0:0x40)+column);
#if PRODUCT==ATMEGADRUM
	lcd_display_cursor_blink(1,1,0);
#else /* PRODUCT */
	lcd_display_cursor_blink(1,0,0);
#endif /* PRODUCT */
}


void lcd_display_cursor_blink(uint8_t display, uint8_t cursor, uint8_t blink)
{
	uint8_t cmd = 0;
	if (display!=0) cmd |= ST7032_DISPLAY_ON;
	// Beware of auto-increment. If you don't see the cursor, maybe it fell off the screen?
	if (cursor!=0) cmd |= ST7032_CURSOR_ON;
	if (blink!=0) cmd |= ST7032_BLINK_ON;
	st7032i_write_command(ST7032_CMD_ON_OFF,cmd);
}


void lcd_clear_to_eol(int line, int column)
{
	uint8_t i;
	lcd_cursor_move(line,column);
	for (i=column; i<BOARD_LCD_CHARS_PER_LINE; i++)
	{
		lcd_putc(' ');
	}
	lcd_cursor_move(line,column);
}


void lcd_contrast(uint8_t contrast)
{
	// Set contrast low bits.
	st7032i_write_command(ST7032_CMD_CONTRAST_LO,contrast&ST7032_CONTRAST_LO_MASK);
	// Set contrast high bits.
	// When LCD is powered from 3V the booster must be on.
	st7032i_write_command(ST7032_CMD_POWER_ICON_CONTRAST,ST7032_BOOSTER_ON|((contrast>>4)&ST7032_CONTRAST_HI_MASK));
}


void lcd_font(uint8_t font)
{
	st7032i_write_command(ST7032_CMD_FUNCTION,ST7032_FUNC_CONFIG_EXTENDED|(font&ST7032_FUNC_HEIGHT_DOUBLE));
}


void lcd_set_custom_character(uint8_t index, const uint8_t *p_data)
{
	int i;
	// Up to 8 custom characters may be defined.
	// Select function table 0.
	st7032i_write_command(ST7032_CMD_FUNCTION,ST7032_FUNC_CONFIG_NORMAL);
	// Set CGRAM address.
	st7032i_write_command(ST7032_CMD_CGRAM_ADDRESS,(8*index)&0x3f);
	// 8 bytes per character.
	for (i=0; i<7; i++)
	{
		st7032i_write_data(p_data[i]&0x1f);
	}
}


void lcd_bar_graph(uint8_t position, uint8_t value, uint8_t value_max)
{
	//st7032i_write_command(ST7032_CMD_ON_OFF,ST7032_DISPLAY_ON|ST7032_CURSOR_OFF|ST7032_BLINK_OFF);
	uint16_t v = value;
	v = 14*v/value_max; // map [1,255] to 14 possibilities, 0 is possibility 15.
	lcd_cursor(1,position);
	if (v<=6)
	{
		if (value==0)
		{
			v = 5; //'0';
		}
		else
		{
			v += 1;
			if (v==5) v = 0x11;
			else if (v==7) v = 'T';
		}
		lcd_putc(v);
		lcd_cursor(0,position);
		lcd_putc(0);
	}
	else
	{
		lcd_putc(0);
		lcd_cursor(0,position);
		v -= 6;
		if (v==5) v = 0x11;
		else if (v==7) v = 'T';
		else if (v>7) v = 7; //'M';
		lcd_putc(v);
	}
}


void lcd_init(void)
{
	int i;

	Board_LCD_SetBacklight(true);
	// Liberate LCD.
	SysTick_Delay(10);
	Board_LCD_ResetDeassert();
	SysTick_Delay(ST7032_RESET_RECOVERY_TIME);

	// Setup I2C port.
	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0,400000); // ST7032 can handle 400 kHz.
	Chip_I2C_SetMasterEventHandler(I2C0,Chip_I2C_EventHandler);
	// Enable I2C interrupts.
	NVIC_EnableIRQ(I2C0_IRQn);
	// Low interrupt priority.
	NVIC_SetPriority(I2C0_IRQn,7);

	// Initialize LCD module.
	// Set custom characters.
	for (i=0; i<8; i++)
	{
		lcd_set_custom_character(i,(const uint8_t *)&custom_characters[i]);
	}
	// Set our default config and enable extended functions.
	st7032i_write_command(ST7032_CMD_FUNCTION,ST7032_FUNC_CONFIG_EXTENDED);
	// Set internal oscillator.
	st7032i_write_command(ST7032_CMD_BIAS_OSC,ST7032_BIAS_020|4);
	// Set contrast. 4 is a good value for 3V displays.
	lcd_contrast(4);
	// Follower on, amplifier max. (Why? From the Midas datasheet)
	st7032i_write_command(ST7032_CMD_FOLLOWER,ST7032_FOLLOWER_ON|7);
	// A 200 ms delay is recommended for the power to stabilize.
	SysTick_Delay(200);
	// Display on.
	//st7032i_write_command(ST7032_CMD_ON_OFF,ST7032_DISPLAY_ON|ST7032_CURSOR_OFF|ST7032_BLINK_OFF);
	lcd_display_cursor_blink(1,0,0);
	// Clear display.
	lcd_clear();

	// Check custom characters.
	/*lcd_display_cursor_blink(1,1,0);
	for (i=0; i<8; i++)
	{
		lcd_putc((char)i);
	}
	SysTick_Delay(5000);*/
}
