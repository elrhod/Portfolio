This folder presents mostly C codes but also a Makefile and a bash script for Linux that are part of the C code generation. This demonstrates the ability to code in different languages that cooperate to generate the intended target binary.
The codes, scripts, and files presented are part of a bigger project and will not work by themselves. They are here only as part of a portfolio to demonstrate my coding experience. 

## fw_sha_update
This bash script updates the firmware with the latest commit short SHA signature. This allows the firmware to report the SHA of the running version. 

## Makefile
This is the makefile used to generate the programming file and program the system board.

## cmd_handle.c
This code snippet is part of a command handle function divided in three steps:
- Check if the input parameters are between accepted bounds;
- If the bounds checking is OK, call the specific handling function that will perform the action required by the user;
- Transmit the reply command to the user;

## STM32H735ZGTX_FLASH.ld
This is an example of a linker file created by the STM32CubeIDE and modified by me to add the DMA buffer regions to support DMA data transfer between specific peripherals and memory.

## ad9528_stm32_hal (.c and .h)
These codes present a hardware abstraction layer of the PLL IC (AD9528).

## ad5293_stm32_hal (.c and .h)
These codes present a hardware abstraction for the Digital Potentiometer (AD5293).

## ltc2335_stm32_hal (.c and .h)
These codes present a hardware abstraction layer for the 16-bit 1 Msps ADC LTC2335.

## ltc266x_stm32_hal (.c and .h)
These codes present a hardware abstraction layer for the 16-channel (LTC2668) and 4-channel (LTC2664) DACs.
