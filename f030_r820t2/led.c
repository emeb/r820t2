/*
 * led.c - f030 breakout LED setup
 */

#include "led.h"

/*
 * Initialize the breakout board LED
 */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* turn on clock for LED GPIO */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	/* Enable PB1 for output */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

/*
 * Turn on LED
 */
void led_on(uint32_t LED)
{
	GPIOB->ODR |= LED;
}

/*
 * Turn off LED
 */
void led_off(uint32_t LED)
{
	GPIOB->ODR &= ~LED;
}

/*
 * Toggle LED
 */
void led_toggle(uint32_t LED)
{
	GPIOB->ODR ^= LED;
}

