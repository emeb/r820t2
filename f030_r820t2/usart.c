/*
 * usart.c - serial i/o routines for STM32F030F4
 * 09-04-16 E. Brombaugh
 */

#include <stdio.h>
#include "usart.h"

/* uncomment this to enable interrupt-based USART processing */
#define USART_IRQ

/* uncomment this for RX */
#define USART_RX

#ifdef USART_IRQ
#ifdef USART_RX
/* rx buffer */
#define RX_BUFLEN 256
uint8_t RX_buffer[RX_BUFLEN];
uint8_t *RX_wptr, *RX_rptr;
#endif
/* tx buffer */
#define TX_BUFLEN 400
uint8_t TX_buffer[TX_BUFLEN];
uint8_t *TX_wptr, *TX_rptr;
#endif

/* USART1 setup */
void usart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef USART_IRQ
#ifdef USART_RX
	/* init RX buffer write/read pointers*/
	RX_wptr = &RX_buffer[0];
	RX_rptr = &RX_buffer[0];
#endif
	/* init TX buffer */
	TX_wptr = &TX_buffer[0];
	TX_rptr = &TX_buffer[0];
#endif
	
	/* Setup USART */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Connect PA2 to USART1_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef USART_RX
	/* Connect PA3 to USART1_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

	/* Configure USART Rx as alternate function w/ pullup */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

	/* USART configuration */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* USART = 115k-8-N-1 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
#ifdef USART_IRQ
#ifdef USART_RX
	/* Enable RX interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}

int usart_getc(void)
{
#ifdef USART_RX
#ifndef USART_IRQ
	/* Non-interrupt version */
	if(USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == SET)
		return USART_ReceiveData(USART6);
	else
		return EOF;
#else
	/* interrupt version */
	int retval;
	
	/* check if there's data in the buffer */
	if(RX_rptr != RX_wptr)
	{
		/* get the data */
		retval = *RX_rptr++;
		
		/* wrap the pointer */
		if((RX_rptr - &RX_buffer[0])>=RX_BUFLEN)
			RX_rptr = &RX_buffer[0];
	}
	else
		retval = EOF;

	return retval;
#endif
#else
    return 0;
#endif
}

/*
 * output for tiny printf
 */
void usart_putc(void* p, char c)
{
#ifndef USART_IRQ
	/* Non-interrupt version */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{}
	USART_SendData(USART1, (uint8_t) c);
#else
	/* interrupt version */
	/* check if there's room in the buffer */
	if((TX_wptr != TX_rptr-1) &&
	   (TX_wptr - TX_rptr != (TX_BUFLEN-1)))
	{
		/* Yes - Queue the new char & enable IRQ */
		*TX_wptr++ = c;
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

		/* Wrap pointer */
		if((TX_wptr - &TX_buffer[0])>=TX_BUFLEN)
			TX_wptr = &TX_buffer[0];
	}
#endif
}

#ifdef USART_IRQ
/*
 * USART IRQ handler - used only for Rx for now
 */
void USART1_IRQHandler(void)
{
	/* receive */
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* get the character */
		uint8_t rxchar = USART_ReceiveData(USART1);

#ifdef USART_RX
		/* check if there's room in the buffer */
		if((RX_wptr != RX_rptr-1) &&
           (RX_wptr - RX_rptr != (RX_BUFLEN-1)))
		{
			/* Yes - Queue the new char */
			*RX_wptr++ = rxchar;
	
			/* Wrap pointer */
			if((RX_wptr - &RX_buffer[0])>=RX_BUFLEN)
				RX_wptr = &RX_buffer[0];
		}
#endif
	}
	
	/* transmit */
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		/* check if there's data in the buffer */
		if(TX_rptr != TX_wptr)
		{
			/* get data out of the buffer */
			uint8_t c = *TX_rptr++;
			
			/* wrap the pointer */
			if((TX_rptr - &TX_buffer[0])>=TX_BUFLEN)
				TX_rptr = &TX_buffer[0];

			/* send the data */
			USART_SendData(USART1, c);
		}
		else
		{
			/* No TX data ready so disable the interrupt */
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}
}
#endif

