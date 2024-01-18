/*
 * ltc2335_stm32_hal.c
 *
 *  Created on: Mar 18, 2021
 *      Author: Eduardo
 */



#include "ltc2335_stm32_hal.h"

void hal_ltc2335_spi_sequencer_program(SPI_HandleTypeDef *hspi, ADC_LTC2335SequencerCommandType *sequence_data, uint16_t size){
	uint8_t transmit[size];
	int sequence_cnt=0;

	for(int i=0; i<size; i++) {
		if(sequence_data[i].has_command){  // only includes in the sequence the commands that have a "has_command" flag set
			transmit[sequence_cnt] = sequence_data[i].channel | sequence_data[i].softspam_code | ADC_CONTROL_WORD_VALIDATION;
			sequence_cnt++;
		}
	}

	for (size_t i = 0; i < sequence_cnt; i++) {
		hal_ltc2335_spi_transmit(hspi, &transmit[i], 1, LTC2335_TIMEOUT);	
	}


}

void hal_ltc2335_spi_transmit(SPI_HandleTypeDef *hspi, uint8_t *transmit_data, uint16_t size, uint32_t timeout){
	HAL_StatusTypeDef ret;

	// Enable pll chip select pin
//    HAL_GPIO_WritePin(SPI_LTC2335_CS_GPIO_Port, SPI_LTC2335_CS_Pin, CS_ENABLE);
    // transmit data
    ret = HAL_SPI_Transmit(hspi, transmit_data, size, timeout);
    // Disable pll chip select pin
//    HAL_GPIO_WritePin(SPI_LTC2335_CS_GPIO_Port, SPI_LTC2335_CS_Pin, CS_DISABLE);
}
