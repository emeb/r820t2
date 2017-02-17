/*
 * systick.c - systick routines
 */

#include "systick.h"

static __IO uint32_t TimingDelay, Ticks;

/*
 * init the system tick infrastructure
 */
void systick_init(void)
{
    Ticks = 0;
    
	/* start the 1ms system tick */
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
		/* Capture error */ 
		while (1);
	}
}

/*
 * blocking delay for number of milliseconds
 */
void systick_delayms(uint32_t ms)
{
	TimingDelay = ms;

	while(TimingDelay != 0);
}

/*
 * Get tick count for nonblocking delays
 */
uint32_t systick_gettick(void)
{
    return Ticks;
}

/*
 * IRQ handler
 */
void SysTick_Handler(void)
{
	/* update ms delay timer */
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
    
    Ticks++;
}

