/*
 * cmd.c - Command parsing routines for STM32F303 breakout SPI to ice5 FPGA
 * 05-11-16 E. Brombaugh
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmd.h"
#include "usart.h"
#include "printf.h"
#include "systick.h"
#include "r820t2.h"

#define MAX_ARGS 4

/* locals we use here */
char cmd_buffer[80];
char *cmd_wptr;
const char *cmd_commands[] = 
{
	"help",
	"read",
	"write",
    "dump",
    "freq",
    "lna_gain",
    "mixer_gain",
    "vga_gain",
    "lna_agc_ena",
    "mixer_agc_ena",
    "bandwidth",
    "init",
    ""
};

/* reset buffer & display the prompt */
void cmd_prompt(void)
{
	/* reset input buffer */
	cmd_wptr = &cmd_buffer[0];

	/* prompt user */
	printf("\rCommand>");
}

/* process command line after <cr> */
void cmd_proc(void)
{
	char *token, *argv[MAX_ARGS];
	int argc, cmd, reg;
	unsigned long data, i;

	/* parse out three tokens: cmd arg arg */
	argc = 0;
	token = strtok(cmd_buffer, " ");
	while(token != NULL && argc < MAX_ARGS)
	{
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}

	/* figure out which command it is */
	if(argc > 0)
	{
		cmd = 0;
		while(cmd_commands[cmd] != '\0')
		{
			if(strcmp(argv[0], cmd_commands[cmd])==0)
				break;
			cmd++;
		}
	
		/* Can we handle this? */
		if(cmd_commands[cmd] != '\0')
		{
			printf("\r\n");

			/* Handle commands */
			switch(cmd)
			{
				case 0:		/* Help */
					printf("help - this message\r\n");
					printf("read <addr> - ead reg\r\n");
					printf("write <addr> <data> - write reg, data\r\n");
					printf("dump - dump all regs\r\n");
					printf("freq <frequency> - Set freq in Hz\r\n");
					printf("lna_gain <gain> - Set gain of LNA [0 - 15]\r\n");
					printf("mixer_gain <gain> - Set gain Mixer [0 - 15]\r\n");
					printf("vga_gain <gain> - Set gain VGA [0 - 15]\r\n");
					printf("lna_agc_ena <state> - Enable LNA AGC [0 / 1]\r\n");
					printf("mixer_agc_ena <state> - Enable Mixer AGC [0 / 1]\r\n");
					printf("bandwidth <bw> - Set IF bandwidth [0 - 15]\r\n");
                    printf("init - run initialization\n\r");
					break;
	
				case 1: 	/* read */
					if(argc < 2)
						printf("read - missing arg(s)\r\n");
					else
					{
						reg = (int)strtoul(argv[1], NULL, 0) & 0x3f;
						data = R820T2_i2c_read_reg_uncached(reg);
						printf("read: 0x%02X = 0x%02lX\r\n", reg, data);
					}
					break;
	
				case 2: 	/* write */
					if(argc < 3)
						printf("write - missing arg(s)\r\n");
					else
					{
						reg = (int)strtoul(argv[1], NULL, 0) & 0x3f;
						data = strtoul(argv[2], NULL, 0);
						R820T2_i2c_write_reg(reg, data);
						printf("write: 0x%02X 0x%02lX\r\n", reg, data);
					}
					break;
		
				case 3: 	/* dump */
                    printf("dump:\n\r");
                    for(i=0;i<0x20;i++)
                    {
                        reg = R820T2_i2c_read_reg_uncached(i);
                        printf("%3d : 0x%02X\n\r", i, reg&0xff);
                        systick_delayms(5);
                    }
                    break;
                
                case 4:     /* freq */
					if(argc < 2)
						printf("freq - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0);
						R820T2_set_freq(data);;
						printf("freq:  %ld\r\n", data);
					}
                    break;
                    
                case 5:     /* lna gain */
					if(argc < 2)
						printf("lna_gain - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0) & 0x0f;
						R820T2_set_lna_gain(data);;
						printf("lna_gain:  %ld\r\n", data);
					}
                    break;
                
                case 6:     /* mixer gain */
					if(argc < 2)
						printf("mixer_gain - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0) & 0x0f;
						R820T2_set_mixer_gain(data);;
						printf("mixer_gain:  %ld\r\n", data);
					}
                    break;
                
                case 7:     /* vga gain */
					if(argc < 2)
						printf("vga_gain - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0) & 0x0f;
						R820T2_set_vga_gain(data);;
						printf("vga_gain:  %ld\r\n", data);
					}
                    break;
                
                case 8:     /* lna agc enable */
					if(argc < 2)
						printf("lna_agc_ena - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0) & 0x01;
						R820T2_set_lna_agc(data);;
						printf("lna_agc_ena:  %ld\r\n", data);
					}
                    break;
                
                case 9:     /* mixer agc enable */
					if(argc < 2)
						printf("lna_agc_ena - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0) & 0x01;
						R820T2_set_mixer_agc(data);;
						printf("mixer_agc_ena:  %ld\r\n", data);
					}
                    break;
                
                case 10:     /* IF Bandwidth */
					if(argc < 2)
						printf("bandwidth - missing arg(s)\r\n");
					else
					{
						data = (int)strtoul(argv[1], NULL, 0) & 0x0f;
						R820T2_set_if_bandwidth(data);;
						printf("bandwidth:  %ld\r\n", data);
					}
                    break;
                
				case 11: 	/* init */
                    printf("init\n\r");
                    R820T2_init();
                    break;
                
				default:	/* shouldn't get here */
					break;
			}
		}
		else
			printf("Unknown command\r\n");
	}
}
void R820T2_set_if_bandwidth(uint8_t bw);
	
void init_cmd(void)
{
	/* prompt */
	cmd_prompt();
}

void cmd_parse(char ch)
{
	/* accumulate chars until cr, handle backspace */
	if(ch == '\b')
	{
		/* check for buffer underflow */
		if(cmd_wptr - &cmd_buffer[0] > 0)
		{
			printf("\b \b");		/* Erase & backspace */
			cmd_wptr--;		/* remove previous char */
		}
	}
	else if(ch == '\r')
	{
		*cmd_wptr = '\0';	/* null terminate, no inc */
		cmd_proc();
		cmd_prompt();
	}
	else
	{
		/* check for buffer full (leave room for null) */
		if(cmd_wptr - &cmd_buffer[0] < 254)
		{
			*cmd_wptr++ = ch;	/* store to buffer */
			usart_putc(NULL, ch);   /* echo */
		}
	}
}
