The codes, scripts, and files presented are part of a bigger project and will not work by themselves. They are here only as part of a portfolio.

## fw_sha_update
This bash script updates the firmware with the latest commit short SHA signature. This allows the firmware to report the SHA of the running version. 

## Makefile
This is the makefile used to generate the programming file and program the system board.

## STM32H735ZGTX_FLASH.ld
This is an example of a linker file created by the STM32CubeIDE and modified by me to add the DMA buffer regions to support DMA data transmission between specific peripherals and memory.

## ad9528_stm32_hal (.c and .h)
These codes present a hardware abstraction layer of the PLL IC (AD9528).

## ad5293_stm32_hal (.c and .h)
These codes present a hardware abstraction for the Digital Potentiometer (AD5293).

## ltc2335_stm32_hal (.c and .h)
These codes present a hardware abstraction layer for the 16-bit 1 Msps ADC LTC2335.

## ltc266x_stm32_hal (.c and .h)
These codes present a hardware abstraction layer for the 16-channel (LTC2668) and 4-channel (LTC2664) DACs.
