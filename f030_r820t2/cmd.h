/*
 * cmd.h - Command parsing routines for STM32F030 breakout
 * 02-15-17 E. Brombaugh
 */
 
#ifndef __cmd__
#define __cmd__

#include "stm32f0xx.h"

extern void init_cmd(void);
extern void cmd_parse(char ch);

#endif
