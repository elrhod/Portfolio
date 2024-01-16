## fw_sha_update
This bash script updates the firmware with the latest commit short SHA signature. This allows the firmware to report the SHA of the running version. 

## Makefile
This is the makefile used to generate the programming file and program the system board.

## STM32H735ZGTX_FLASH.ld
This is an example of a linker file created by the STM32CubeIDE and modified by me to add the DMA buffer regions to support DMA data transmission between specific peripherals and memory.
