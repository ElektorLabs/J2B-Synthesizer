#include "atmegatron.h"


keyboard_key_t g_encoder[] =
{
	// Function bit 7 is either reserved 1 (IOCON_RESERVED_BIT_7) or "Digital functional mode" (IOCON_DIGMODE_EN),
	// meaning that it has to be set in all cases.
	// No pull-ups or -downs on rotary encoder pins. All inputs are inverted to read as 1 on contact closed.
	// Rotary encoders are handled as two non-matrix keys.

	#if defined BOARD_KEYBOARD_RE0 /* S1 */
		{ BOARD_KEYBOARD_RE0_A_PORT, BOARD_KEYBOARD_RE0_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE0_B_PORT, BOARD_KEYBOARD_RE0_B_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE1 /* S2 */
		{ BOARD_KEYBOARD_RE1_A_PORT, BOARD_KEYBOARD_RE1_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE1_B_PORT, BOARD_KEYBOARD_RE1_B_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE2 /* S3 */
		{ BOARD_KEYBOARD_RE2_A_PORT, BOARD_KEYBOARD_RE2_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE2_B_PORT, BOARD_KEYBOARD_RE2_B_PIN, IOCON_FUNC1|IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE3 /* S4 */
		{ BOARD_KEYBOARD_RE3_A_PORT, BOARD_KEYBOARD_RE3_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE3_B_PORT, BOARD_KEYBOARD_RE3_B_PIN, IOCON_FUNC1|IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE4 /* S5 */
		{ BOARD_KEYBOARD_RE4_A_PORT, BOARD_KEYBOARD_RE4_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE4_B_PORT, BOARD_KEYBOARD_RE4_B_PIN, IOCON_FUNC1|IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE5 /* S6 */
		{ BOARD_KEYBOARD_RE5_A_PORT, BOARD_KEYBOARD_RE5_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE5_B_PORT, BOARD_KEYBOARD_RE5_B_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE6 /* S7 */
		{ BOARD_KEYBOARD_RE6_A_PORT, BOARD_KEYBOARD_RE6_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE6_B_PORT, BOARD_KEYBOARD_RE6_B_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif

	#if defined BOARD_KEYBOARD_RE7 /* S8 */
		{ BOARD_KEYBOARD_RE7_A_PORT, BOARD_KEYBOARD_RE7_A_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
		{ BOARD_KEYBOARD_RE7_B_PORT, BOARD_KEYBOARD_RE7_B_PIN, IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif
};


keyboard_key_t g_key[] =
{
	// Function bit 7 is either reserved 1 (IOCON_RESERVED_BIT_7) or "Digital functional mode" (IOCON_DIGMODE_EN),
	// meaning that it has to be set in all cases.
	// Pull-ups needed. All inputs are inverted to read as 1 on contact closed.

	// S9 (ISP)
	{ BOARD_KEYBOARD_ISP_PORT, BOARD_KEYBOARD_ISP_PIN, IOCON_MODE_PULLUP|IOCON_DIGMODE_EN|IOCON_INV_EN },

	#if defined RESET_IS_GPIO
		// S10 (reset) could go here...
		{ BOARD_KEYBOARD_RESET_PORT, BOARD_KEYBOARD_RESET_PIN, IOCON_FUNC1|IOCON_MODE_PULLUP|IOCON_DIGMODE_EN|IOCON_INV_EN },
	#endif
};


keyboard_matrix_key_t g_matrix_key[] =
{
#ifdef BOARD_KEYBOARD_S00  /* S1 */
	{ BOARD_KEYBOARD_C0_PORT, BOARD_KEYBOARD_C0_PIN, BOARD_KEYBOARD_R0_PORT, BOARD_KEYBOARD_R0_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S01  /* S2 */
	{ BOARD_KEYBOARD_C1_PORT, BOARD_KEYBOARD_C1_PIN, BOARD_KEYBOARD_R0_PORT, BOARD_KEYBOARD_R0_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S02  /* S3 */
	{ BOARD_KEYBOARD_C2_PORT, BOARD_KEYBOARD_C2_PIN, BOARD_KEYBOARD_R0_PORT, BOARD_KEYBOARD_R0_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S10  /* S4 */
	{ BOARD_KEYBOARD_C0_PORT, BOARD_KEYBOARD_C0_PIN, BOARD_KEYBOARD_R1_PORT, BOARD_KEYBOARD_R1_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S11  /* S5 */
	{ BOARD_KEYBOARD_C1_PORT, BOARD_KEYBOARD_C1_PIN, BOARD_KEYBOARD_R1_PORT, BOARD_KEYBOARD_R1_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S12  /* S6 */
	{ BOARD_KEYBOARD_C2_PORT, BOARD_KEYBOARD_C2_PIN, BOARD_KEYBOARD_R1_PORT, BOARD_KEYBOARD_R1_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S20  /* S7 */
	{ BOARD_KEYBOARD_C0_PORT, BOARD_KEYBOARD_C0_PIN, BOARD_KEYBOARD_R2_PORT, BOARD_KEYBOARD_R2_PIN, { 0, 0 }},
#endif

#ifdef BOARD_KEYBOARD_S21  /* S8 */
	{ BOARD_KEYBOARD_C1_PORT, BOARD_KEYBOARD_C1_PIN, BOARD_KEYBOARD_R2_PORT, BOARD_KEYBOARD_R2_PIN, { 0, 0 }},
#endif

	// FYI: S9 is ISP, S10 is reset button.

#ifdef BOARD_KEYBOARD_S22  /* S11 */
	{ BOARD_KEYBOARD_C2_PORT, BOARD_KEYBOARD_C2_PIN, BOARD_KEYBOARD_R2_PORT, BOARD_KEYBOARD_R2_PIN, { 0, 0 }},
#endif
};


// Since all data is zero, a memset would have been quicker to type.
keyboard_rotary_encoder_data_t g_encoder_data[] =
{
#ifdef BOARD_KEYBOARD_RE0  /* S1 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE1  /* S2 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE2  /* S3 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE3  /* S4 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE4  /* S5 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE5  /* S6 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE6  /* S7 */
	{
		0, 0, 0, 0, 0
	},
#endif

#ifdef BOARD_KEYBOARD_RE7  /* S8 */
	{
		0, 0, 0, 0, 0
	},
#endif
};


#define MATRIX_KEY_COUNT  (sizeof(g_matrix_key)/sizeof(g_matrix_key[0]))
#define ENCODER_PIN_COUNT  (sizeof(g_encoder)/sizeof(g_encoder[0]))
#define ENCODER_COUNT  (sizeof(g_encoder_data)/sizeof(g_encoder_data[0]))
#define KEYS_OTHER_COUNT  (sizeof(g_key)/sizeof(g_key[0]))


void keyboard_init(void)
{
	int i;
	keyboard_matrix_key_t *p_matrix_key;
	keyboard_key_t *p_encoder;
	keyboard_key_t *p_keys_other;

	// Initialize key matrix.
	p_matrix_key = g_matrix_key;
	for (i=0; i<MATRIX_KEY_COUNT; i++)
	{
		// Function bit 7 is either reserved 1 (IOCON_RESERVED_BIT_7) or "Digital functional mode" (IOCON_DIGMODE_EN),
		// meaning that it has to be set in all cases.
		// Pull-ups needed on row pins. All inputs are inverted to read as 1 on contact closed.
		Chip_IOCON_PinMuxSet(LPC_IOCON,p_matrix_key->row_port,p_matrix_key->row_pin,IOCON_MODE_PULLUP|IOCON_DIGMODE_EN|IOCON_INV_EN);
		Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,p_matrix_key->col_port,p_matrix_key->col_pin,OUTPUT);
		Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,p_matrix_key->row_port,p_matrix_key->row_pin,INPUT);
		p_matrix_key->data.debounce = 0;
		p_matrix_key->data.value = key_released;
		p_matrix_key->data.changed = false;
		p_matrix_key++;
	}

	// Initialize rotary encoders.
	p_encoder = g_encoder;
	// A rotary encoder has two pins which is why we use ENCODER_PIN_COUNT.
	for (i=0; i<ENCODER_PIN_COUNT; i++)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON,p_encoder->port,p_encoder->pin,p_encoder->function);
		Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,p_encoder->port,p_encoder->pin,INPUT);
		p_encoder->data.debounce = 0;
		p_encoder->data.value = key_released;
		p_encoder++;
	}

	// Initialize remaining keys like S9 & S10.
	p_keys_other = g_key;
	for (i=0; i<KEYS_OTHER_COUNT; i++)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON,p_keys_other->port,p_keys_other->pin,p_keys_other->function);
		Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,p_keys_other->port,p_keys_other->pin,INPUT);
		p_keys_other->data.debounce = 0;
		p_keys_other->data.value = key_released;
		p_keys_other++;
	}
}


int8_t keyboard_key_is_debounced(int8_t debounce)
{
	int8_t result = 0;
	if (debounce>=KEY_DEBOUNCE_MAX)
	{
		// Key debounced high.
		result = 1;
	}
	else if (debounce<=KEY_DEBOUNCE_MIN)
	{
		// Key debounced low.
		result = -1;
	}
	return result;
}


uint8_t keyboard_matrix_key_read(keyboard_matrix_key_t *p_key)
{
	uint8_t value;

	// Scan a key. Pressed keys read as high (inverted input).
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,p_key->col_port,p_key->col_pin,0);
	value = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,p_key->row_port,p_key->row_pin);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,p_key->col_port,p_key->col_pin,1);
	return value;
}


inline uint8_t keyboard_key_read(keyboard_key_t *p_key)
{
	// Pressed keys read as high (inverted input).
	return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,p_key->port,p_key->pin);
}


uint8_t keyboard_key_debounce(int8_t *p_debounce, uint8_t state)
{
	uint8_t result = 0;

	// Debounce.
	if (state!=0)
	{
		if ((*p_debounce)<KEY_DEBOUNCE_MAX) (*p_debounce)++;
		if ((*p_debounce)>=KEY_DEBOUNCE_MAX) result = 1;
	}
	else
	{
		if ((*p_debounce)>KEY_DEBOUNCE_MIN) (*p_debounce)--;
		if ((*p_debounce)<=KEY_DEBOUNCE_MIN) result = 1;
	}

	return result;
}


uint32_t keyboard_scan_matrix(void)
{
	int i;
	uint8_t state;

	// Scan key matrix.
	for (i=0; i<MATRIX_KEY_COUNT; i++)
	{
		state = keyboard_matrix_key_read(&g_matrix_key[i]);
		if (keyboard_key_debounce(&g_matrix_key[i].data.debounce,state)!=0)
		{
			// Update pushbutton value.
			if (g_matrix_key[i].data.debounce>=KEY_DEBOUNCE_MAX)
			{
				if (g_matrix_key[i].data.value!=key_pressed)
				{
					g_matrix_key[i].data.changed = true;
				}
				g_matrix_key[i].data.value = key_pressed;
			}
			else
			{
				if (g_matrix_key[i].data.value!=key_released)
				{
					g_matrix_key[i].data.changed = true;
				}
				g_matrix_key[i].data.value = key_released;
			}
		}
	}

	// Scan other keys (S9, S10) too.
	for (i=0; i<KEYS_OTHER_COUNT; i++)
	{
		state = keyboard_key_read(&g_key[i]);
		if (keyboard_key_debounce(&g_key[i].data.debounce,state)!=0)
		{
			// Update pushbutton value.
			if (g_key[i].data.debounce>=KEY_DEBOUNCE_MAX)
			{
				if (g_key[i].data.value!=key_pressed)
				{
					g_key[i].data.changed = true;
				}
				g_key[i].data.value = key_pressed;
			}
			else
			{
				if (g_key[i].data.value!=key_released)
				{
					g_key[i].data.changed = true;
				}
				g_key[i].data.value = key_released;
			}
		}
	}

	return 0;
}


uint32_t keyboard_scan_encoders(void)
{
	int i, j, inc;
	int8_t state_new;
	uint8_t a, b;

	j = 0;
	for (i=0; i<ENCODER_COUNT; i++)
	{
		a = keyboard_key_read(&g_encoder[j]);
		j += 1;
		b = keyboard_key_read(&g_encoder[j]);
		j += 1;
		state_new = (a<<1) | b;
		if (state_new!=g_encoder_data[i].state)
		{
			inc = a ^ (g_encoder_data[i].state&0x01);
			if (inc==0) inc = -1;
			g_encoder_data[i].state = state_new;

			g_encoder_data[i].state_sub += inc;
			if ((g_encoder_data[i].state_sub<=-4) || (g_encoder_data[i].state_sub>=4))
			{
				g_encoder_data[i].state_sub = 0;
				g_encoder_data[i].value -= inc;
				keyboard_encoder_speed(&g_encoder_data[i]);
			}
		}
	}

	return 0;
}


void keyboard_encoder_speed(keyboard_rotary_encoder_data_t *p_encoder_data)
{
	uint32_t timestamp = systick_counter;
	uint32_t delta = timestamp - p_encoder_data->timestamp;
	//delta = 0x1ffff >> delta;
	if (delta<KEY_PERIOD_MIN) delta = KEY_PERIOD_MIN;
	else if (delta>KEY_PERIOD_MAX) delta = KEY_PERIOD_MAX;
	p_encoder_data->accelerator = (uint8_t)(KEY_PERIOD_MAX/delta);
	if (p_encoder_data->accelerator==0) p_encoder_data->accelerator = 1;
	p_encoder_data->timestamp = timestamp;
}


bool keyboard_get_pushbutton_changed(uint8_t key_nr)
{
	if (key_nr>=BOARD_KEYBOARD_ISP)
	{
		// "Other" pushbuttons start at S9.
		return g_key[key_nr-BOARD_KEYBOARD_ISP].data.changed;
	}
	else
	{
		return g_matrix_key[key_nr].data.changed;
	}
}


keyboard_key_press_t keyboard_get_pushbutton(uint8_t key_nr, bool use)
{
	if (key_nr>=BOARD_KEYBOARD_ISP)
	{
		// "Other" pushbuttons start at S9.
		if (use==true) g_key[key_nr-BOARD_KEYBOARD_ISP].data.changed = false;
		return g_key[key_nr-BOARD_KEYBOARD_ISP].data.value;
	}
	else
	{
		if (use==true) g_matrix_key[key_nr].data.changed = false;
		return g_matrix_key[key_nr].data.value;
	}
}


void keyboard_set_pushbutton(uint8_t key_nr, keyboard_key_press_t value)
{
	if (key_nr>=BOARD_KEYBOARD_ISP)
	{
		// "Other" pushbuttons start at S9.
		g_key[key_nr-BOARD_KEYBOARD_ISP].data.value = value;
		g_key[key_nr-BOARD_KEYBOARD_ISP].data.changed = false;
	}
	else
	{
		g_matrix_key[key_nr].data.value = value;
		g_matrix_key[key_nr].data.changed = false;
	}
}


int8_t keyboard_get_rotary_encoder_value(uint8_t encoder_nr)
{
	return g_encoder_data[encoder_nr].value;
}


void keyboard_set_rotary_encoder_value(uint8_t encoder_nr, int8_t value)
{
	g_encoder_data[encoder_nr].value = value;
}


uint8_t keyboard_get_rotary_encoder_accelerator(uint8_t encoder_nr)
{
	return g_encoder_data[encoder_nr].accelerator;
}

