/*
 * usart.h - serial i/o routines for STM32F030F4
 * 05-30-15 E. Brombaugh
 */

#ifndef __usart__
#define __usart__

#include "stm32f0xx.h"

void usart_init(void);
int usart_getc(void);
void usart_putc(void* p, char c);

#endif
