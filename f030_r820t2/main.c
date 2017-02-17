/*
 * STM32F030 driver for R820T2
 * E. Brombaugh 02-12-2017
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f0xx.h"
#include "led.h"
#include "systick.h"
#include "usart.h"
#include "printf.h"
#include "r820t2.h"
#include "cmd.h"

int main(void)
{
    uint8_t regs[32], reg;
    int rxchar;
	uint32_t i, tick;
    
	/* initialize the hardware */
	led_init();
	systick_init();
	usart_init();
	init_printf(0,usart_putc);
	
	/* Startup message */
	printf("\r\nSTM32F030 R820T2 driver.\n\r");
	
	/* init R820T2 */
	R820T2_init();
	printf("R820T2 initialized.\n\r");

    /* Loop forever */
	init_cmd();
    tick = systick_gettick() + 100;
	while(1)
	{
		/* UART command processing */
		if((rxchar = usart_getc())!= EOF)
		{
			/* Parse commands */
			cmd_parse(rxchar);
		}
                
		/* nonblocking wait to flash light */
		if(tick < systick_gettick())
		{
            led_toggle(LED1);
            tick = systick_gettick() + 100;
        }
	}
}
