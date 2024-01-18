/*
 * ltc2664_stm32_hal.c
 *
 *  Created on: Mar 1, 2021
 *      Author: Eduardo
 */


#include "ltc266x_stm32_hal.h"
#include "illu_sysb.h"
#include <stdio.h>

#define ERROR_TRACE 1

/* TODO: change this define position to another file higher in the hierarchy that will be included here */
#ifdef ERROR_TRACE
#define ERROR_MSG(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define ERROR_MSG(fmt, args...)    /* Don't do anything in release builds */
#endif

void hal_ltc2664_spi_update_dac(SPI_HandleTypeDef *hspi1, uint32_t dac){
	DAC_ltc266x_cmd_t transmit;

	transmit.asInteger = 0;

	if(dac == DAC_ALL_DACS) {
		transmit.asInteger = DAC_UPDATE_ALL;
	} else {
		switch (dac) {
			case DAC_DAC0_ADDRESS:
			case DAC_DAC1_ADDRESS:
			case DAC_DAC2_ADDRESS:
			case DAC_DAC3_ADDRESS:
				transmit.asInteger = DAC_UPDATE_N | dac;
			default:
				send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
			break;
		}
	}

	hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);

}

void hal_ltc2664_spi_write_code(SPI_HandleTypeDef *hspi1, uint32_t code, uint32_t dac, uint32_t reg) {
	DAC_ltc266x_cmd_t transmit;

	transmit.asInteger = 0;

	switch (reg) {
		case DAC_WRITE_REGA:
			/* First, make sure that toggle bits are clear by writing '0' to all 4 toggle select bits.*/
			transmit.asInteger = DAC_TOGGLE_SELECT;  // no toggle bit set so it will write all '0's
			hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);

		    /* Second, send the code to selected DAC(s).*/
			if(dac == DAC_ALL_DACS) {
				transmit.asInteger = 0;
				transmit.asInteger |= DAC_WRITE_CODE_TO_ALL;
				transmit.asInteger |= code;
			} else {
				switch (dac) {
					case DAC_DAC0_ADDRESS:
					case DAC_DAC1_ADDRESS:
					case DAC_DAC2_ADDRESS:
					case DAC_DAC3_ADDRESS:
						transmit.asInteger = 0;
						transmit.asInteger |= DAC_WRITE_CODE_TO_N;
						transmit.asInteger |= dac;
						transmit.asInteger |= code;
						break;
					default:
						send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
					break;
				}
			}
			hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);
			break;
		case DAC_WRITE_REGB:

			if(dac == DAC_ALL_DACS) {
				/* First, make sure that toggle bits are set by writing '1' to corresponding DAC.*/
				transmit.asInteger = 0;
				transmit.asInteger = DAC_TOGGLE_SELECT | DAC_TOGGLE_ALL;
				hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);
				/* Second, send code to selected DAC(s).*/
				transmit.asInteger = 0;
				transmit.asInteger |= DAC_WRITE_CODE_TO_ALL;
				transmit.asInteger |= code;
				hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);

			} else {
				/* First, make sure that toggle bits are set by writing '1' to corresponding DAC.*/
				switch (dac) {
					case DAC_DAC0_ADDRESS:
						transmit.asInteger |= DAC_TOGGLE_SELECT | DAC_TOGGLE_DAC0;
						break;
					case DAC_DAC1_ADDRESS:
						transmit.asInteger |= DAC_TOGGLE_SELECT | DAC_TOGGLE_DAC1;
						break;
					case DAC_DAC2_ADDRESS:
						transmit.asInteger |= DAC_TOGGLE_SELECT | DAC_TOGGLE_DAC2;
						break;
					case DAC_DAC3_ADDRESS:
						transmit.asInteger |= DAC_TOGGLE_SELECT | DAC_TOGGLE_DAC3;
						break;
					default:
						send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
					break;
				}
				hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);
			    /* Second, send code to selected DAC(s).*/
				transmit.asInteger = 0;
				transmit.asInteger |= DAC_WRITE_CODE_TO_N;
				transmit.asInteger |= dac;
				transmit.asInteger |= code;
				hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);
			}
		default:
			send_error_msg("ltc2664_stm32_hal ERROR, selected register does not exist!");
		break;
	}
}

void hal_ltc2664_spi_set_span(SPI_HandleTypeDef *hspi1, uint32_t span_code, uint32_t dac) {
	DAC_ltc266x_cmd_t transmit;

	transmit.asInteger =0;

	if(dac == DAC_ALL_DACS) {
		transmit.asInteger |= DAC_WRITE_SPAN_TO_ALL;
		transmit.asInteger |= span_code;
	} else {
		switch (dac) {
		case DAC_DAC0_ADDRESS:
		case DAC_DAC1_ADDRESS:
		case DAC_DAC2_ADDRESS:
		case DAC_DAC3_ADDRESS:
			switch (span_code) {
				case DAC_SPAN_CODE_INT_0_TO_5V_EXT_0_TO_2VREF:
				case DAC_SPAN_CODE_INT_0_TO_10V_EXT_0_TO_4VREF:
				case DAC_SPAN_CODE_INT_BIP_5V_EXT_BIP_2VREF:
				case DAC_SPAN_CODE_INT_BIP_10V_EXT_BIP_4VREF:
				case DAC_SPAN_CODE_INT_BIP_2_5V_EXT_BIP_VREF:
					transmit.asInteger |= DAC_WRITE_SPAN_TO_N;
					transmit.asInteger |= span_code;
					transmit.asInteger |= dac;
				break;
				default:
					send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
				break;
			}
			default:
				send_error_msg("ltc2664_stm32_hal ERROR, selected span code does not exist!");
			break;
		}
	}

	hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);

}

void hal_ltc2664_spi_toggle_select(SPI_HandleTypeDef *hspi1, uint32_t select_code, uint32_t reg){

	DAC_ltc266x_cmd_t transmit;
	transmit.asInteger = 0;

	switch (reg) {
		case DAC_WRITE_REGA:
			transmit.asInteger = DAC_TOGGLE_SELECT & (~select_code);
			break;
		case DAC_WRITE_REGB:
			transmit.asInteger = DAC_TOGGLE_SELECT | select_code;
			break;
	}

	hal_ltc2664_spi_transmit(hspi1, &transmit, 3, LTC2664_TIMEOUT);

}

void hal_ltc2664_spi_transmit(SPI_HandleTypeDef *hspi1, DAC_ltc266x_cmd_t *transmit_data, uint16_t size, uint32_t timeout){
	DAC_ltc266x_cmd_t transmit;

	//Invert order of bytes to send
	transmit.asBytes[0] = transmit_data->asBytes[2];
	transmit.asBytes[1] = transmit_data->asBytes[1];
	transmit.asBytes[2] = transmit_data->asBytes[0];

	// Enable pll chip select pin
    HAL_GPIO_WritePin(SPI_LTC2664_CS_GPIO_Port, SPI_LTC2664_CS_Pin, CS_ENABLE);
    // transmit data
    HAL_SPI_Transmit(hspi1, transmit.asBytes, size, timeout);
    // Disable pll chip select pin
    HAL_GPIO_WritePin(SPI_LTC2664_CS_GPIO_Port, SPI_LTC2664_CS_Pin, CS_DISABLE);
}

void hal_ltc2668_spi_update_dac(SPI_HandleTypeDef *hspi1, uint32_t dac){
	DAC_ltc266x_cmd_t transmit;

	transmit.asInteger = 0;

	if(dac == DAC_ALL_DACS) {
		transmit.asInteger = DAC_UPDATE_ALL;
	} else {
		switch (dac) {
			case DAC_DAC0_ADDRESS:
			case DAC_DAC1_ADDRESS:
			case DAC_DAC2_ADDRESS:
			case DAC_DAC3_ADDRESS:
			case DAC_DAC4_ADDRESS:
			case DAC_DAC5_ADDRESS:
			case DAC_DAC6_ADDRESS:
			case DAC_DAC7_ADDRESS:
			case DAC_DAC8_ADDRESS:
			case DAC_DAC9_ADDRESS:
			case DAC_DAC10_ADDRESS:
			case DAC_DAC11_ADDRESS:
			case DAC_DAC12_ADDRESS:
			case DAC_DAC13_ADDRESS:
			case DAC_DAC14_ADDRESS:
			case DAC_DAC15_ADDRESS:
				transmit.asInteger = DAC_UPDATE_N | dac;
			default:
				send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
			break;
		}
	}

	hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);

}

void hal_ltc2668_spi_write_code(SPI_HandleTypeDef *hspi1, uint32_t code, uint32_t dac, uint32_t reg) {
	DAC_ltc266x_cmd_t transmit;

	transmit.asInteger = 0;

	switch (reg) {
		case DAC_WRITE_REGA:
			/* First, make sure that toggle bits are clear by writing '0' to all 4 toggle select bits.*/
			transmit.asInteger = DAC_TOGGLE_SELECT;  // no toggle bit set so it will write all '0's
			hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);

		    /* Second, send the code to selected DAC(s).*/
			if(dac == DAC_ALL_DACS) {
				transmit.asInteger = 0;
				transmit.asInteger |= DAC_WRITE_CODE_TO_ALL;
				transmit.asInteger |= code;
			} else {
				switch (dac) {
					case DAC_DAC0_ADDRESS:
					case DAC_DAC1_ADDRESS:
					case DAC_DAC2_ADDRESS:
					case DAC_DAC3_ADDRESS:
					case DAC_DAC4_ADDRESS:
					case DAC_DAC5_ADDRESS:
					case DAC_DAC6_ADDRESS:
					case DAC_DAC7_ADDRESS:
					case DAC_DAC8_ADDRESS:
					case DAC_DAC9_ADDRESS:
					case DAC_DAC10_ADDRESS:
					case DAC_DAC11_ADDRESS:
					case DAC_DAC12_ADDRESS:
					case DAC_DAC13_ADDRESS:
					case DAC_DAC14_ADDRESS:
					case DAC_DAC15_ADDRESS:
						transmit.asInteger = 0;
						transmit.asInteger |= DAC_WRITE_CODE_TO_N;
						transmit.asInteger |= dac;
						transmit.asInteger |= code;
						break;
					default:
						send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
					break;
				}
			}
			hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);
			break;
		case DAC_WRITE_REGB:

			if(dac == DAC_ALL_DACS) {
				/* First, make sure that toggle bits are set by writing '1' to corresponding DAC.*/
				transmit.asInteger = 0;
				transmit.asInteger = DAC_TOGGLE_SELECT | DAC_TOGGLE_ALL;
				hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);
				/* Second, send code to selected DAC(s).*/
				transmit.asInteger = 0;
				transmit.asInteger |= DAC_WRITE_CODE_TO_ALL;
				transmit.asInteger |= code;
				hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);

			} else {
				/* First, make sure that toggle bits are set by writing '1' to corresponding DAC.*/
				switch (dac) {
					case DAC_DAC0_ADDRESS:
					case DAC_DAC1_ADDRESS:
					case DAC_DAC2_ADDRESS:
					case DAC_DAC3_ADDRESS:
					case DAC_DAC4_ADDRESS:
					case DAC_DAC5_ADDRESS:
					case DAC_DAC6_ADDRESS:
					case DAC_DAC7_ADDRESS:
					case DAC_DAC8_ADDRESS:
					case DAC_DAC9_ADDRESS:
					case DAC_DAC10_ADDRESS:
					case DAC_DAC11_ADDRESS:
					case DAC_DAC12_ADDRESS:
					case DAC_DAC13_ADDRESS:
					case DAC_DAC14_ADDRESS:
					case DAC_DAC15_ADDRESS:
						transmit.asInteger |= DAC_TOGGLE_SELECT | dac;
						break;
					default:
						send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
					break;
				}
				hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);
			    /* Second, send code to selected DAC(s).*/
				transmit.asInteger = 0;
				transmit.asInteger |= DAC_WRITE_CODE_TO_N;
				transmit.asInteger |= dac;
				transmit.asInteger |= code;
				hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);
			}
		default:
			send_error_msg("ltc2664_stm32_hal ERROR, selected register does not exist!");
		break;
	}
}

void hal_ltc2668_spi_set_span(SPI_HandleTypeDef *hspi1, uint32_t span_code, uint32_t dac) {
	DAC_ltc266x_cmd_t transmit;

	transmit.asInteger =0;

	if(dac == DAC_ALL_DACS) {
		transmit.asInteger |= DAC_WRITE_SPAN_TO_ALL;
		transmit.asInteger |= span_code;
	} else {
		switch (dac) {
		case DAC_DAC0_ADDRESS:
		case DAC_DAC1_ADDRESS:
		case DAC_DAC2_ADDRESS:
		case DAC_DAC3_ADDRESS:
		case DAC_DAC4_ADDRESS:
		case DAC_DAC5_ADDRESS:
		case DAC_DAC6_ADDRESS:
		case DAC_DAC7_ADDRESS:
		case DAC_DAC8_ADDRESS:
		case DAC_DAC9_ADDRESS:
		case DAC_DAC10_ADDRESS:
		case DAC_DAC11_ADDRESS:
		case DAC_DAC12_ADDRESS:
		case DAC_DAC13_ADDRESS:
		case DAC_DAC14_ADDRESS:
		case DAC_DAC15_ADDRESS:
			switch (span_code) {
				case DAC_SPAN_CODE_INT_0_TO_5V_EXT_0_TO_2VREF:
				case DAC_SPAN_CODE_INT_0_TO_10V_EXT_0_TO_4VREF:
				case DAC_SPAN_CODE_INT_BIP_5V_EXT_BIP_2VREF:
				case DAC_SPAN_CODE_INT_BIP_10V_EXT_BIP_4VREF:
				case DAC_SPAN_CODE_INT_BIP_2_5V_EXT_BIP_VREF:
					transmit.asInteger |= DAC_WRITE_SPAN_TO_N;
					transmit.asInteger |= span_code;
					transmit.asInteger |= dac;
				break;
				default:
					send_error_msg("ltc2664_stm32_hal ERROR, selected dac channel does not exist!");
				break;
			}
			default:
				send_error_msg("ltc2664_stm32_hal ERROR, selected span code does not exist!");
			break;
		}
	}

	hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);

}

void hal_ltc2668_spi_toggle_select(SPI_HandleTypeDef *hspi1, uint32_t select_code, uint32_t reg){

	DAC_ltc266x_cmd_t transmit;
	transmit.asInteger = 0;

	switch (reg) {
		case DAC_WRITE_REGA:
			transmit.asInteger = DAC_TOGGLE_SELECT & (~select_code);
			break;
		case DAC_WRITE_REGB:
			transmit.asInteger = DAC_TOGGLE_SELECT | select_code;
			break;
	}

	hal_ltc2668_spi_transmit(hspi1, &transmit, 3, LTC2668_TIMEOUT);

}

void hal_ltc2668_spi_transmit(SPI_HandleTypeDef *hspi1, DAC_ltc266x_cmd_t *transmit_data, uint16_t size, uint32_t timeout){
	DAC_ltc266x_cmd_t transmit;

	//Invert order of bytes to send
	transmit.asBytes[0] = transmit_data->asBytes[2];
	transmit.asBytes[1] = transmit_data->asBytes[1];
	transmit.asBytes[2] = transmit_data->asBytes[0];

	// Enable pll chip select pin
    HAL_GPIO_WritePin(CFG_SPI_CS_LTC2668_GPIO_Port, CFG_SPI_CS_LTC2668_Pin, CS_ENABLE);
    // transmit data
    HAL_SPI_Transmit(hspi1, transmit.asBytes, size, timeout);
    // Disable pll chip select pin
    HAL_GPIO_WritePin(CFG_SPI_CS_LTC2668_GPIO_Port, CFG_SPI_CS_LTC2668_Pin, CS_DISABLE);
}

