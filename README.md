# CUSF Avionics 2014

## Firmware Compilation

Designed to be compiled with the GNU ARM Toolchain - https://launchpad.net/gcc-arm-embedded

This project uses the Fatfs module and so when setting up your local copy you will need to extract the fatfs zip under /ChibiOS-RT/ext and place its contents (doc and src) in a folder called fatfs in the ext folder.

You will also need to uncomment

    #define STM32F407xx
	
on line 76 of ChibiOS-RT/os/ext/CMSIS/ST/stm32f4xx.h

The project should then simply compile with

    make
	
