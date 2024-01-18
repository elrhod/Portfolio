/*
 * hal_spi.c
 *
 *  Created on: Feb. 24, 2021
 *      Author: Eduardo
 *      
 */

#include "ad9528_stm32_hal.h"
#include "illu_sysb.h"

void init_pll_struct(PLL_ConfigTypeDef *pll_cfg) {

	pll_cfg->pll1_config.charge_pump_control=0;
	pll_cfg->pll1_config.fast_lock=0;
	pll_cfg->pll1_config.feedback_n1_div=0;
	pll_cfg->pll1_config.input_receiver_control=0;
	pll_cfg->pll1_config.refa_div=1;
	pll_cfg->pll1_config.refb_div=1;

	pll_cfg->pll2_config.charge_pump_current=0;
	pll_cfg->pll2_config.feedback_VCO_CAL_div=0x04;
	pll_cfg->pll2_config.m1_VCO=0;
	pll_cfg->pll2_config.n2_feedback_div=0;
	pll_cfg->pll2_config.n2_phase_offset=0;
	pll_cfg->pll2_config.pll2_VCO_control=0;
	pll_cfg->pll2_config.pll2_control=0x03;
	pll_cfg->pll2_config.r1_input_div=0;

	for(int i=0; i<12; i++) {
		pll_cfg->output_config[i].channel_control=0;
		pll_cfg->output_config[i].coarse_delay=0;
		pll_cfg->output_config[i].fine_delay=0x40;
		pll_cfg->output_config[i].output_div=0x04;
		pll_cfg->output_config[i].output_voltage_standard=0;
	}
	pll_cfg->output_config[13].channel_control=0x40;
	pll_cfg->output_config[13].coarse_delay=0;
	pll_cfg->output_config[13].fine_delay=0x40;
	pll_cfg->output_config[13].output_div=0x00;
	pll_cfg->output_config[13].output_voltage_standard=0;

	pll_cfg->powerdown_config.power_down_control=0x000010;
	pll_cfg->powerdown_config.ldo_enable=0xFFFF;

}

void hal_pll_spi_config(SPI_HandleTypeDef *hspi, PLL_ConfigTypeDef *pll_config) {

	// pll1 variables
	pll_bitfield_t pll1_refa_div;                // Regs 0x102 & 0x103
	pll_bitfield_t pll1_refb_div;  			// Regs 0x102 & 0x103
	pll_bitfield_t pll1_n1_div;    			// Regs 0x104 & 0x105
	pll_bitfield_t pll1_charge_pump_ctrl; 	// Regs 0x106 & 0x107
	pll_bitfield_t pll1_input_receiver_ctrl; 	// Regs 0x108 & 0x109 & 0x10A
	pll_bitfield_t pll1_fast_lock; 			// Regs 0x10B

	// pll2 variables
	pll_bitfield_t pll2_charge_pump_ctrl; 	// Regs 0x200
	pll_bitfield_t pll2_feedback_VCO_div; 	// Regs 0x201
	pll_bitfield_t pll2_control; 				// Regs 0x202
	pll_bitfield_t pll2_VCO_control; 			// Regs 0x203
	pll_bitfield_t pll2_VCO_divider; 			// Regs 0x204
	pll_bitfield_t pll2_input_divider_r1; 	// Regs 0x207
	pll_bitfield_t pll2_feedback_divider_n2; 	// Regs 0x208 & 0x209

	// clock distribution variables
	pll_bitfield_t clock_channel[14]; 			// [0] Regs 0x300 & 0x301 & 0x302
	                                            // [1] Regs 0x303 & 0x304 & 0x305
	                                            // [2] Regs 0x306 & 0x307 & 0x308
	                                            // [3] Regs 0x309 & 0x30A & 0x30B
	                                            // [4] Regs 0x30C & 0x30D & 0x30E
	                                            // [5] Regs 0x30F & 0x310 & 0x311
	                                            // [6] Regs 0x312 & 0x313 & 0x314
	                                            // [7] Regs 0x315 & 0x316 & 0x317
	                                            // [8] Regs 0x318 & 0x319 & 0x31A
	                                            // [9] Regs 0x31B & 0x31C & 0x31D
	                                            // [10] Regs 0x31E & 0x31F & 0x320
	                                            // [11] Regs 0x321 & 0x322 & 0x323
	                                            // [12] Regs 0x324 & 0x325 & 0x326
	                                            // [13] Regs 0x327 & 0x328 & 0x329

	// power-down variables
	pll_bitfield_t powerdown_ctrl;                 // Reg 0x500 & 0x501 & 0x502
	pll_bitfield_t output_channel_ldo_en;             // Reg 0x503 & 0x504

	pll1_refa_div.asInteger=0;  			// Regs 0x100 & 0x101
	pll1_refb_div.asInteger=0;  			// Regs 0x102 & 0x103
	pll1_n1_div.asInteger=0;    			// Regs 0x104 & 0x105
	pll1_charge_pump_ctrl.asInteger=0; 	// Regs 0x106 & 0x107
	pll1_input_receiver_ctrl.asInteger=0; 	// Regs 0x108 & 0x109 & 0x10A
	pll1_fast_lock.asInteger=0; 			// Regs 0x10B


	pll2_charge_pump_ctrl.asInteger=0; 	// Regs 0x200
	pll2_feedback_VCO_div.asInteger=0; 	// Regs 0x201
	pll2_control.asInteger=0; 				// Regs 0x202
	pll2_VCO_control.asInteger=0; 			// Regs 0x203
	pll2_VCO_divider.asInteger=0; 			// Regs 0x204
	pll2_input_divider_r1.asInteger=0; 	// Regs 0x207
	pll2_feedback_divider_n2.asInteger=0; 	// Regs 0x208 & 0x209


	powerdown_ctrl.asInteger = 0;                 // Reg 0x500 & 0x501 & 0x502
	output_channel_ldo_en.asInteger = 0;             // Reg 0x503 & 0x504

	for(int i=0; i<14; i++) {
		clock_channel[i].asInteger=0;
	}

	// pll1 map
	pll1_refa_div.asInteger = pll_config->pll1_config.refa_div;
	pll1_refb_div.asInteger = pll_config->pll1_config.refb_div;
	pll1_n1_div.asInteger = pll_config->pll1_config.feedback_n1_div;
	pll1_charge_pump_ctrl.asBytes[0] = pll_config->pll1_config.charge_pump_control & 0xFF;
	pll1_charge_pump_ctrl.asBytes[1] = pll_config->pll1_config.charge_pump_control>>8 & 0x3F;
	pll1_input_receiver_ctrl.asBytes[0] = pll_config->pll1_config.input_receiver_control & 0xFF;
	pll1_input_receiver_ctrl.asBytes[1] = pll_config->pll1_config.input_receiver_control>>8 & 0x3F;
	pll1_input_receiver_ctrl.asBytes[2] = pll_config->pll1_config.input_receiver_control>>16 & 0x0F;
	pll1_fast_lock.asBytes[0] = pll_config->pll1_config.fast_lock  & 0xFF;

	// pll2 map
	pll2_charge_pump_ctrl.asBytes[0] = pll_config->pll2_config.charge_pump_current;
	pll2_feedback_VCO_div.asBytes[0] = pll_config->pll2_config.feedback_VCO_CAL_div & 0xFF;
	pll2_control.asBytes[0] = pll_config->pll2_config.pll2_control & 0xA3;
	pll2_VCO_control.asBytes[0] = pll_config->pll2_config.pll2_VCO_control & 0x1F;
	pll2_VCO_divider.asBytes[0] = pll_config->pll2_config.m1_VCO & 0x3F;
	pll2_input_divider_r1.asBytes[0] = pll_config->pll2_config.r1_input_div & 0x1F;
	pll2_feedback_divider_n2.asBytes[0] = (pll_config->pll2_config.n2_feedback_div - 1) & 0xFF;
	pll2_feedback_divider_n2.asBytes[1] = pll_config->pll2_config.n2_phase_offset & 0x7F;

	// clock distribution map
	hal_pll_spi_config_clock_distribution_map(clock_channel, pll_config->output_config);

	// Power-down configuration map
	powerdown_ctrl.asBytes[0] = pll_config->powerdown_config.power_down_control & 0x1F;
	powerdown_ctrl.asBytes[1] = pll_config->powerdown_config.power_down_control>>8 & 0xFF;
	powerdown_ctrl.asBytes[2] = pll_config->powerdown_config.power_down_control>>16 & 0x3F;
	output_channel_ldo_en.asBytes[0] = pll_config->powerdown_config.ldo_enable & 0xFF;
	output_channel_ldo_en.asBytes[1] = pll_config->powerdown_config.ldo_enable>>8 & 0xFF;

	// pll1 Writing
	hal_write_pll_register(hspi, 0x100, pll1_refa_div.asBytes[0]);
	hal_write_pll_register(hspi, 0x101, pll1_refa_div.asBytes[1]);
	hal_write_pll_register(hspi, 0x102, pll1_refb_div.asBytes[0]);
	hal_write_pll_register(hspi, 0x103, pll1_refb_div.asBytes[1]);
	hal_write_pll_register(hspi, 0x104, pll1_n1_div.asBytes[0]);
	hal_write_pll_register(hspi, 0x105, pll1_n1_div.asBytes[1]);
	hal_write_pll_register(hspi, 0x106, pll1_charge_pump_ctrl.asBytes[0]);
	hal_write_pll_register(hspi, 0x107, pll1_charge_pump_ctrl.asBytes[1]);
	hal_write_pll_register(hspi, 0x108, pll1_input_receiver_ctrl.asBytes[0]);
	hal_write_pll_register(hspi, 0x109, pll1_input_receiver_ctrl.asBytes[1]);
	hal_write_pll_register(hspi, 0x10A, pll1_input_receiver_ctrl.asBytes[2]);
	hal_write_pll_register(hspi, 0x10B, pll1_fast_lock.asBytes[0]);

	// pll2 Writing
	hal_write_pll_register(hspi, 0x200, pll2_charge_pump_ctrl.asBytes[0]);
	hal_write_pll_register(hspi, 0x201, pll2_feedback_VCO_div.asBytes[0]);
	hal_write_pll_register(hspi, 0x202, pll2_control.asBytes[0]);
	hal_write_pll_register(hspi, 0x203, pll2_VCO_control.asBytes[0]);
	hal_write_pll_register(hspi, 0x204, pll2_VCO_divider.asBytes[0]);
	hal_write_pll_register(hspi, 0x207, pll2_input_divider_r1.asBytes[0]);
	hal_write_pll_register(hspi, 0x208, pll2_feedback_divider_n2.asBytes[0]);
	hal_write_pll_register(hspi, 0x209, pll2_feedback_divider_n2.asBytes[1]);

	// clock distribution writing *** pre configuring only channel 0
	// ** OBS.: Channel 13 should be left with default configurations as its used to generate MCU base clock.
	// for(int i=0; i<14; i++) {
	int i=0;
		for(int j=0; j<3; j++) {
			hal_write_pll_register(hspi, 0x300+(3*i+j),clock_channel[i].asBytes[j]);
		}
	// }

	// power-down writing
	hal_write_pll_register(hspi, 0x500, powerdown_ctrl.asBytes[0]);
	hal_write_pll_register(hspi, 0x501, powerdown_ctrl.asBytes[1]);
	hal_write_pll_register(hspi, 0x502, powerdown_ctrl.asBytes[2]);
	hal_write_pll_register(hspi, 0x503, output_channel_ldo_en.asBytes[0]);
	hal_write_pll_register(hspi, 0x504, output_channel_ldo_en.asBytes[1]);

	hal_write_pll_calibrate_vco(hspi);
	hal_write_pll_io_update(hspi);
}

void hal_pll_spi_config_clock_distribution_map(pll_bitfield_t *clock_channel, Ouput_ConfigTypeDef *clock_config){

	for(int i=0; i<PLL_NUM_OF_CLOCKS; i++) {
		clock_channel[i].asBytes[0] = (clock_config[i].channel_control & 0xE0) | \
				                      (clock_config[i].fine_delay & 0x1F);
		clock_channel[i].asBytes[1] = (clock_config[i].output_voltage_standard & 0xC0) | \
				                      (clock_config[i].coarse_delay & 0x3F);
		if (clock_config[i].output_div>0) {
			clock_channel[i].asBytes[2] = (clock_config[i].output_div -1) & 0xFF;
		} else {
			clock_channel[i].asBytes[2] = clock_config[i].output_div & 0xFF;
		}

	}
}

void hal_write_pll_register(SPI_HandleTypeDef *hspi, uint16_t address, uint8_t value) {
	pll_cmd_t tmp, transmit;

	//load TX data to the tmp union
	tmp.registers.READ = 0;
	tmp.registers.ADDRESS = address;
	tmp.registers.DATA = value;

	//Invert order of bytes to send
	transmit.asBytes[0] = tmp.asBytes[2];
	transmit.asBytes[1] = tmp.asBytes[1];
	transmit.asBytes[2] = tmp.asBytes[0];

	// Enable pll chip select pin
    HAL_GPIO_WritePin(CFG_SPI_CS_AD9528_GPIO_Port, CFG_SPI_CS_AD9528_Pin, CS_ENABLE);
    // transmit data
    HAL_SPI_Transmit(hspi, transmit.asBytes, 3, 5000);
    // Disable pll chip select pin
    HAL_GPIO_WritePin(CFG_SPI_CS_AD9528_GPIO_Port, CFG_SPI_CS_AD9528_Pin, CS_DISABLE);

}

void hal_write_pll_calibrate_vco(SPI_HandleTypeDef *hspi) {
	hal_write_pll_register(hspi, 0x0203, 0x01); // PLL2 VCO Control [4:0]
}

void hal_write_pll_io_update(SPI_HandleTypeDef *hspi) {
	hal_write_pll_register(hspi, 0x000F, 0x01); // IO_UPDATE - Bit 0
}
