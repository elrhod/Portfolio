/*
 * ad5293_stm32_hal.c
 *
 *  Created on: Mar 11, 2021
 *      Author: Eduardo
 */

#include <ad5293_stm32_hal.h>

void hal_ad5293_spi_transmit(SPI_HandleTypeDef *hspi1, DIG_POT_ad5293_cmd_t *transmit_data, uint16_t size, uint32_t timeout, uint8_t dig_pot_num){
	DIG_POT_ad5293_cmd_t transmit;

	//Invert order of bytes to send
	transmit.asBytes[0] = transmit_data->asBytes[1];
	transmit.asBytes[1] = transmit_data->asBytes[0];

	// Enable  chip select pin
	switch(dig_pot_num) {
		case 0:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G0_GPIO_Port, CFG_SPI_CS_SS_G0_Pin, GPIO_PIN_RESET);
		break;

		case 1:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G1_GPIO_Port, CFG_SPI_CS_SS_G1_Pin, GPIO_PIN_RESET);
		break;

		case 2:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G2_GPIO_Port, CFG_SPI_CS_SS_G2_Pin, GPIO_PIN_RESET);
		break;

		case 3:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G3_GPIO_Port, CFG_SPI_CS_SS_G3_Pin, GPIO_PIN_RESET);
		break;
	}
  // transmit data
  HAL_SPI_Transmit(hspi1, transmit.asBytes, size, timeout);
  // Disable  chip select pin
	switch(dig_pot_num) {
		case 0:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G0_GPIO_Port, CFG_SPI_CS_SS_G0_Pin, GPIO_PIN_SET);
		break;

		case 1:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G1_GPIO_Port, CFG_SPI_CS_SS_G1_Pin, GPIO_PIN_SET);
		break;

		case 2:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G2_GPIO_Port, CFG_SPI_CS_SS_G2_Pin, GPIO_PIN_SET);
		break;

		case 3:
		HAL_GPIO_WritePin(CFG_SPI_CS_SS_G3_GPIO_Port, CFG_SPI_CS_SS_G3_Pin, GPIO_PIN_SET);
		break;
	}
}

void hal_ad5293_spi_update_dig_pot(SPI_HandleTypeDef *hspi1, uint16_t value, uint8_t dig_pot_num){
	DIG_POT_ad5293_cmd_t transmit;

	transmit.asInteger = 0;

	// enable update by unlocking internal register
	transmit.asInteger = AD5292_WRITE_DATA_TO_CONTROL_REG | AD5292_RESISTOR_UPDATE_ENABLE_MEM_MODE;
	hal_ad5293_spi_transmit(hspi1, &transmit, 2, AD5293_TIMEOUT, dig_pot_num);

	// write value
	transmit.asInteger = AD529X_WRITE_TO_RDAC | (value & 0x3FF);
	hal_ad5293_spi_transmit(hspi1, &transmit, 2, AD5293_TIMEOUT, dig_pot_num);

//	// lock internal register back
//	transmit.asInteger = AD5293_WRITE_CONTROL | AD5293_RESISTOR_LOCKED_MODE;
//	hal_ad5293_spi_transmit(hspi1, &transmit, 2, AD5293_TIMEOUT);

	// transmit.asInteger = AD5293_NOP;
	// hal_ad5293_spi_transmit(hspi1, &transmit, 2, AD5293_TIMEOUT, dig_pot_num);

}

void hal_ad5293_SPI6_Init(SPI_HandleTypeDef *hspi) {
	// if (HAL_SPI_DeInit(hspi) != HAL_OK)
	// {
	// 	Error_Handler(83);
	// }
	hspi->Instance = SPI6;
	hspi->Init.Mode = SPI_MODE_MASTER;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi->Init.NSS = SPI_NSS_SOFT;
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial = 0x0;
	hspi->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
	if (HAL_SPI_Init(	hspi) != HAL_OK)
	{
		Error_Handler(105);
	}
}
