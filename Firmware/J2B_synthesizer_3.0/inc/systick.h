#ifndef __SYSTICK_H__
#define __SYSTICK_H__


#define SYSTICK_RATE_HZ  (1000)	// Ticks per second.

void SysTick_Seconds(void);
void SysTick_Delay(uint32_t ticks);
void SysTick_LED_Flash(uint8_t led, uint32_t ticks);

extern volatile uint32_t seconds_counter;
extern volatile uint32_t systick_counter;


#endif // __SYSTICK_H__
