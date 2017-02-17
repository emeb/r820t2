README.TXT

Building
--------

Just run
	make
	
Flashing
--------

1) Ensure openocd isn't running as a server already.

2) Make sure that your STM32F0 Discovery board is connected via USB and the
ST-LINK jumpers are set correctly (this example is for the on-board processor,
so 1-2 and 3-4 should be shorted)

3) run
	make flash

4) Done!

Debugging
---------

1) in a separate window start up openocd as a GDB server
	openocd -f openocd.cfg

2) run gdb with your favorite UI
	ddd --debugger arm-none-eabi-gdb main.elf

3) connect to the server within gdb
	target extended-remote localhost:3333

4) Start debugging!

LICENSE
-------

The r820t2 driver code (r820t2.[ch]) herein originated with the Airspy product
which derived from the Linux driver. Consequently my revisions are subject to
the LICENSE file from those projects.

The CMSIS and StdPeriph directories are from ARM ans ST Microelectronics and
are suject to the licenses therein.

