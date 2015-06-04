#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__


#include "board.h"


typedef enum
{
	key_released = 0,
	key_pressed
}
keyboard_key_press_t;


typedef struct
{
	int8_t debounce;
	keyboard_key_press_t value;
	bool changed;
}
keyboard_key_data_t;


typedef struct
{
	uint8_t port;
	uint8_t pin;
	uint8_t function;
	keyboard_key_data_t data;
}
keyboard_key_t;


typedef struct
{
	uint8_t col_port;
	uint8_t col_pin;
	uint8_t row_port;
	uint8_t row_pin;
	keyboard_key_data_t data;
}
keyboard_matrix_key_t;


typedef struct
{
	int8_t value;
	int8_t state;
	uint8_t accelerator;
	uint32_t timestamp;
	int8_t state_sub;
}
keyboard_rotary_encoder_data_t;


#define KEY_DEBOUNCE_MIN  (0)
#define KEY_DEBOUNCE_MAX  (3)
#define KEY_PERIOD_MIN  (10)
#define KEY_PERIOD_MAX  (500)


void keyboard_init(void);
int8_t keyboard_key_is_debounced(int8_t debounce);
uint8_t keyboard_matrix_key_read(keyboard_matrix_key_t *p_key);
uint8_t keyboard_key_read(keyboard_key_t *p_key);
uint8_t keyboard_key_debounce(int8_t *p_debounce, uint8_t state);
uint32_t keyboard_scan_matrix(void);
uint32_t keyboard_scan_encoders(void);
void keyboard_encoder_speed(keyboard_rotary_encoder_data_t *p_encoder_data);
int8_t keyboard_encoder_tick(keyboard_key_t *p_key_a, keyboard_key_t *p_key_b, keyboard_rotary_encoder_data_t *p_encoder_data);
bool keyboard_get_pushbutton_changed(uint8_t key_nr);
keyboard_key_press_t keyboard_get_pushbutton(uint8_t key_nr, bool use);
void keyboard_set_pushbutton(uint8_t key_nr, keyboard_key_press_t value);
int8_t keyboard_get_rotary_encoder_value(uint8_t encoder_nr);
void keyboard_set_rotary_encoder_value(uint8_t encoder_nr, int8_t value);
uint8_t keyboard_get_rotary_encoder_accelerator(uint8_t encoder_nr);


#endif // __KEYBOARD_H__
