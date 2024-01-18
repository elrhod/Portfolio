/*
 * ad9528_stm32_hal.h
 *
 *  Created on: Feb. 24, 2021
 *      Author: Eduardo
 *      Version: 0.1
 *      Description: This version includes configuration of pll1, pll2, clock outputs
 *      and synchronization command only. Based on the AD9528 Rev. E Data sheet;
 *      Version Note: TODO: This version needs to be better tested and documented
 */


#ifndef INC_AD9528_STM32_HAL_H_
#define INC_AD9528_STM32_HAL_H_

#include "stm32h7xx_hal.h"

////////////////////////////////////////////////
// Generic Defines
#define CS_ENABLE                  0
#define CS_DISABLE                 1
// This device has 14 clock ouputs
#define PLL_NUM_OF_CLOCKS          14
//// Serial Port Configuration
// Reg 0x0000 - SPI Configuration A
#define PLL_SOFT_RESET_COMMAND      0x81U
#define PLL_SPI_LSB_FIRST           0x42U
#define PLL_SPI_ADDRESS_ASCENSION   0x24U
#define PLL_SPI_SDO_ACTIVE          0x18U

// Reg 0x0001 - SPI Configuration B
#define PLL_READ_BUFFER_REGISTER    0x20U
#define PLL_RESET_SANS_REGMAP       0x04U

//// PLL1
// Reg 0x0106 and 0x0107  - PLL1 config
#define PLL_FORCE_HOLDOVER          0x0080U
#define PLL_DISABLE_HOLDOVER        0x2000U
#define PLL_CHARGE_PUMP_TRISTATE    0x0000U
#define PLL_CHARGE_PUMP_PUMP_DOWN   0x0100U
#define PLL_CHARGE_PUMP_PUMP_UP     0x0200U
#define PLL_CHARGE_PUMP_NORMAL      0x0300U

// Reg 0x0108 - PLL1 config - Input Receiver Control
#define PLL_FREQ_DETECTOR_POWERDOWN_DISABLED  0x000000U
#define PLL_FREQ_DETECTOR_POWERDOWN_ENABLED   0x000080U

#define PLL_REFB_SINGLE_ENDED_RECEIVER   0x000000U
#define PLL_REFB_DIFFERENTIAL_RECEIVER   0x000040U

#define PLL_REFA_SINGLE_ENDED_RECEIVER   0x000000U
#define PLL_REFA_DIFFERENTIAL_RECEIVER   0x000020U

#define PLL_REFB_RECEIVER_ENABLED        0x000010U
#define PLL_REFB_POWER_DOWN              0x000000U

#define PLL_REFA_POWER_DOWN              0x000000U
#define PLL_REFA_RECEIVER_ENABLED        0x000008U

#define PLL_VCXO_BOTH_RECEIVER_ENABLED         0x000000U
#define PLL_VCXO_RECEIVER_POWERDOWN_CTRL_EN    0x000004U

#define PLL_VCXO_SINGLE_ENDED_POS_PIN_SEL      0x000000U
#define PLL_VCXO_SINGLE_ENDED_NEG_PIN_SEL      0x000002U

#define PLL_VCXO_SINGLE_ENDED_RECEIVER_MODE    0x000000U
#define PLL_VCXO_DIFFERENTIAL_RECEIVER_MODE    0x000001U

// Reg 0x0109 - PLL1 config - Input Receiver Control
#define PLL_N1_FEEDBACK_DIVIDER_RESET     0x002000U
#define PLL_N1_FEEDBACK_DIVIDER_NORMAL    0x000000U

#define PLL_REFB_DIVIDER_RESET            0x001000U
#define PLL_REFB_DIVIDER_NORMAL           0x000000U

#define PLL_REFA_DIVIDER_RESET            0x000800U
#define PLL_REFA_DIVIDER_NORMAL           0x000000U

#define PLL_PLL1_DIVIDER_FEEDBACK_SOURCE_VCXO  0x000400U
#define PLL_PLL1_DIVIDER_FEEDBACK_SOURCE_PLL2  0x000000U

#define PLL_REFB_SINGLE_ENDED_NEGATIVE_PIN_EN  0x000200U
#define PLL_REFB_SINGLE_ENDED_POSITIVE_PIN_EN  0x000000U

#define PLL_REFA_SINGLE_ENDED_NEGATIVE_PIN_EN  0x000100U
#define PLL_REFA_SINGLE_ENDED_POSITIVE_PIN_EN  0x000000U

// Reg 0x010A - PLL1 config - Holdover and Reference mode When feedback or input fail.
#define PLL_VCXO_HOLDOVER_HALF_VCC     0x080000U
#define PLL_VCXO_HOLDOVER_CHARGE_PUMP  0x000000U

#define PLL_PLL1_REFERENCE_STAY_REFB       0x000000U
#define PLL_PLL1_REFERENCE_REVERT_TO_REFA  0x010000U
#define PLL_PLL1_REFERENCE_SELECT_REFA     0x020000U
#define PLL_PLL1_REFERENCE_SELECT_REFB     0x030000U
#define PLL_PLL1_REFERENCE_REF_SEL_PIN     0x040000U // Depends on value of REF_SEL pin

// Reg 0x010B - PLL1 config - Charge Pump Fast Lock
#define PLL_PLL1_FAST_LOCK_ENABLE         0x80U
#define PLL_PLL1_FAST_LOCK_DISABLE        0x00U

//// PLL2
// Reg 0x0202 - PLL2 config - PLL2 Control
#define PLL_LOCK_DECTECTOR_POWERDOWN      0x80U
#define PLL_LOCK_DECTECTOR_ACTIVE         0x00U

#define PLL_FREQUENCY_DOUBLER_ENABLE      0x20U
#define PLL_FREQUENCY_DOUBLER_DISABLE     0x00U

// Reg 0x0203 - PLL2 VCO Control
#define PLL_PLL2_DOUBLER_R1_PATH_ENABLE   0x10U
#define PLL_PLL2_DOUBLER_R1_PATH_BYPASS   0x00U

#define PLL_PLL2_RESET_A_AND_B_DIVIDERS   0x08U
#define PLL_PLL2_RESET_NORMAL             0x00U

#define PLL_USE_PLL1VCXO_AS_VALID         0x00U
#define PLL_REFERENCE_IS_VALID            0x04U

#define PLL_PLL2_VCO_NORMAL               0x00U
#define PLL_PLL2_VCO_MIDSCALE             0x02U

#define PLL_PLL2_VCO_CALIBRATE_MANUAL     0x01U  // (this is not an autoclearing bit).
#define PLL_PLL2_VCO_CALIBRATE_RESET      0x00U

// Reg 0x0204 - PLL2 RF VCO Divider (M1)
#define PLL_PLL2_PFD_REF_FALLING_EDGE     0x20U
#define PLL_PLL2_PFD_REF_RISING_EDGE      0x00U

#define PLL_PLL2_PFD_FEEDBACK_FALLING_EDGE 0x10U
#define PLL_PLL2_PFD_FEEDBACK_RISING_EDGE  0x00U

#define PLL_PLL2_VCO_M1_DIVIDER_POWERDOWN  0x08U
#define PLL_PLL2_VCO_M1_DIVIDER_NORMAL     0x00U

#define PLL_PLL2_VCO_M1_DIVIDE_BY_3       0x03U
#define PLL_PLL2_VCO_M1_DIVIDE_BY_4       0x04U
#define PLL_PLL2_VCO_M1_DIVIDE_BY_5       0x05U

// Reg 0x0208 and  0x0209 - PLL2 R1 Reference Divider
#define PLL_PLL2_N2_DIVIDER_POWERDOWN     0x40U
#define PLL_PLL2_N2_DIVIDER_NORMAL        0x00U


//// CLOCK DISTRIBUTION (REGISTER 0x300 TO REGISTER 0x0329)
// Output Channel Config.
#define PLL_CLOCK_PLL2_DIVIDER_OUTPUT     0x00U
#define PLL_CLOCK_PLL1_VCXO_OUTPUT        0x20U
#define PLL_CLOCK_SYSREF_PLL2_OUTPUT      0x40U
#define PLL_CLOCK_SYSREF_PLL1_OUTPUT      0x60U
#define PLL_CLOCK_INV_PLL1_VCXO_OUTPUT    0xA0U
#define PLL_CLOCK_SYSREF_INV_PLL1_OUTPUT  0xE0U
#define PLL_CLOCK_FINE_ANALOG_DELAY_EN    0x10U
#define PLL_CLOCK_FINE_ANALOG_DELAY_DIS   0x00U

#define PLL_CLOCK_LVDS_NORMAL             0x00U
#define PLL_CLOCK_LVDS_BOOST              0x40U
#define PLL_CLOCK_HSTL                    0x80U

// Reg 0x032A - Distribution Sync - Issues SYNC on transition of bit 0 from 1 to 0.
#define PLL_SYNC_OUTPUTS_SET                  0x01U
#define PLL_SYNC_OUTPUTS_RESET                0x00U
// Reg 0x032B and 0x032C - Ignore SYNC Enable
#define PLL_SYNC_CHANNEL0_IGNORE              0x0001U
#define PLL_SYNC_CHANNEL1_IGNORE              0x0002U
#define PLL_SYNC_CHANNEL2_IGNORE              0x0004U
#define PLL_SYNC_CHANNEL3_IGNORE              0x0008U
#define PLL_SYNC_CHANNEL4_IGNORE              0x0010U
#define PLL_SYNC_CHANNEL5_IGNORE              0x0020U
#define PLL_SYNC_CHANNEL6_IGNORE              0x0040U
#define PLL_SYNC_CHANNEL7_IGNORE              0x0080U
#define PLL_SYNC_CHANNEL8_IGNORE              0x0100U
#define PLL_SYNC_CHANNEL9_IGNORE              0x0200U
#define PLL_SYNC_CHANNEL10_IGNORE             0x0400U
#define PLL_SYNC_CHANNEL11_IGNORE             0x0800U
#define PLL_SYNC_CHANNEL12_IGNORE             0x1000U
#define PLL_SYNC_CHANNEL13_IGNORE             0x2000U
#define PLL_SYNC_PLL2_FEEDBACK_N2_IGNORE      0x4000U

#define PLL_SYNC_CHANNEL0              0x0000U
#define PLL_SYNC_CHANNEL1              0x0000U
#define PLL_SYNC_CHANNEL2              0x0000U
#define PLL_SYNC_CHANNEL3              0x0000U
#define PLL_SYNC_CHANNEL4              0x0000U
#define PLL_SYNC_CHANNEL5              0x0000U
#define PLL_SYNC_CHANNEL6              0x0000U
#define PLL_SYNC_CHANNEL7              0x0000U
#define PLL_SYNC_CHANNEL8              0x0000U
#define PLL_SYNC_CHANNEL9              0x0000U
#define PLL_SYNC_CHANNEL10             0x0000U
#define PLL_SYNC_CHANNEL11             0x0000U
#define PLL_SYNC_CHANNEL12             0x0000U
#define PLL_SYNC_CHANNEL13             0x0000U
#define PLL_SYNC_PLL2_FEEDBACK_N2      0x0000U

#define PLL_SYNC_ALL_CHANNELS          0x0000U
// Reg 0x500 - powerdown_control_enable
#define PLL_BYAS_GEN_POWERDOWN         0x000000U
#define PLL_BYAS_GEN_NORMAL            0x000010U

#define PLL_PLL2_NORMAL                0x000000U
#define PLL_PLL2_POWERDOWN             0x000008U

#define PLL_PLL1_NORMAL                0x000000U
#define PLL_PLL1_POWERDOWN             0x000004U

#define PLL_CLOCK_DISTRIBUTION_NORMAL     0x000000U
#define PLL_CLOCK_DISTRIBUTION_POWERDOWN  0x000002U

#define PLL_CHIP_OPERATION_NORMAL      0x000000U
#define PLL_CHIP_OPERATION_POWERDOWN   0x000001U

// Reg 0x501 AND 0x502 - Channel Powerdown
#define PLL_CHANNEL0_POWERDOWN              0x000100U
#define PLL_CHANNEL1_POWERDOWN              0x000200U
#define PLL_CHANNEL2_POWERDOWN              0x000400U
#define PLL_CHANNEL3_POWERDOWN              0x000800U
#define PLL_CHANNEL4_POWERDOWN              0x001000U
#define PLL_CHANNEL5_POWERDOWN              0x002000U
#define PLL_CHANNEL6_POWERDOWN              0x004000U
#define PLL_CHANNEL7_POWERDOWN              0x008000U
#define PLL_CHANNEL8_POWERDOWN              0x010000U
#define PLL_CHANNEL9_POWERDOWN              0x020000U
#define PLL_CHANNEL10_POWERDOWN             0x040000U
#define PLL_CHANNEL11_POWERDOWN             0x080000U
#define PLL_CHANNEL12_POWERDOWN             0x100000U
#define PLL_CHANNEL13_POWERDOWN             0x200000U

// Reg 0x503 AND 0x504 - LDO Regulator Enable
#define PLL_CHANNEL0_LDO_NORMAL              0x0001U
#define PLL_CHANNEL1_LDO_NORMAL              0x0002U
#define PLL_CHANNEL2_LDO_NORMAL              0x0004U
#define PLL_CHANNEL3_LDO_NORMAL              0x0008U
#define PLL_CHANNEL4_LDO_NORMAL              0x0010U
#define PLL_CHANNEL5_LDO_NORMAL              0x0020U
#define PLL_CHANNEL6_LDO_NORMAL              0x0040U
#define PLL_CHANNEL7_LDO_NORMAL              0x0080U
#define PLL_CHANNEL8_LDO_NORMAL              0x0100U
#define PLL_CHANNEL9_LDO_NORMAL              0x0200U
#define PLL_CHANNEL10_LDO_NORMAL             0x0400U
#define PLL_CHANNEL11_LDO_NORMAL             0x0800U
#define PLL_CHANNEL12_LDO_NORMAL             0x1000U
#define PLL_CHANNEL13_LDO_NORMAL             0x2000U


////////////////////////////////////////////////
// TYPES DEFINITIONS
////////////////////////////////////////////////

// Union used to alow bit, byte, word and named registers
typedef union _pll_cmd_t {
    uint32_t asInteger;
    uint8_t asBytes[4];
    // LSB need to be first
    struct {
    	uint32_t DATA           :8;
    	uint32_t ADDRESS        :15;
    	uint32_t READ           :1;
    } registers;
} pll_cmd_t;

typedef union _pll_bitfield_t {
    int32_t asInteger;
    uint8_t asBytes[4];
	struct {
    	uint32_t bit0 :1;
    	uint32_t bit1 :1;
    	uint32_t bit2 :1;
    	uint32_t bit3 :1;
    	uint32_t bit4 :1;
    	uint32_t bit5 :1;
    	uint32_t bit6 :1;
    	uint32_t bit7 :1;
    	uint32_t bit8 :1;
    	uint32_t bit9 :1;
    	uint32_t bit10 :1;
    	uint32_t bit11 :1;
    	uint32_t bit12 :1;
    	uint32_t bit13 :1;
    	uint32_t bit14 :1;
    	uint32_t bit15 :1;
    	uint32_t bit16 :1;
    	uint32_t bit17 :1;
    	uint32_t bit18 :1;
    	uint32_t bit19 :1;
    	uint32_t bit20 :1;
    	uint32_t bit21 :1;
    	uint32_t bit22 :1;
    	uint32_t bit23 :1;
    	uint32_t bit24 :1;
    	uint32_t bit25 :1;
    	uint32_t bit26 :1;
    	uint32_t bit27 :1;
    	uint32_t bit28 :1;
    	uint32_t bit29 :1;
    	uint32_t bit30 :1;
    	uint32_t bit31 :1;
    } asBits;

    struct {
    	uint8_t bit0 :1;
    	uint8_t bit1 :1;
    	uint8_t bit2 :1;
    	uint8_t bit3 :1;
    	uint8_t bit4 :1;
    	uint8_t bit5 :1;
    	uint8_t bit6 :1;
    	uint8_t bit7 :1;
    } asReg8[4];
} pll_bitfield_t;


// PLL1 Control and config, Charge Pump Holdover - Reg 0x0100 to Reg 0x010B
typedef struct {
	uint32_t refa_div;                // Real val is refa_div + 1.
	uint32_t refb_div;                // Real val is refb_div + 1.
	uint32_t feedback_n1_div;         // Real val is feedback_n1_div + 1.
	uint32_t charge_pump_control;
	uint32_t input_receiver_control;
	uint32_t fast_lock;
} PLL1_ConfigTypeDef;

// PLL2 Control and config - Reg 0x0200 to Reg 0x0209
typedef struct {
	uint32_t charge_pump_current;
	uint32_t feedback_VCO_CAL_div;    // Check Data sheet for values (There are Constraints)
	uint32_t pll2_control;
	uint32_t pll2_VCO_control;
	uint32_t m1_VCO;
	uint32_t r1_input_div;
	uint32_t n2_feedback_div;       // Real val is pll2_feedback_n2_div + 1.
	uint32_t n2_phase_offset;
} PLL2_ConfigTypeDef;

// Output CLOCK distribution - Reg 0x0300 to Reg 0x0329
typedef struct {
	uint32_t channel_control;
	uint32_t fine_delay;
	uint32_t output_voltage_standard;
	uint32_t coarse_delay;
	uint32_t output_div;
} Ouput_ConfigTypeDef;

// Power Control Configuration
typedef struct {
	uint32_t power_down_control;
	uint32_t ldo_enable;
} Power_Control_ConfigTypedef;

// Structure that includes all configuration structures
typedef struct {
	PLL1_ConfigTypeDef pll1_config;
	PLL2_ConfigTypeDef pll2_config;
	Ouput_ConfigTypeDef output_config[14];
	Power_Control_ConfigTypedef powerdown_config;
} PLL_ConfigTypeDef;

////////////////////////////////////////////////
// FUNCTION PROTOTYPES
////////////////////////////////////////////////

/**
  * @brief           initializes the pll structure with default initilization values of the AD9528.
  * @param pll_cfg   pointer to the PLL_ConfigTypeDef struct
  */
void init_pll_struct(PLL_ConfigTypeDef *pll_cfg);

/**
  * @brief 			   configures the AD9528 by sending the user defined ppl_config values through SPI
  * @param  hspi       pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  * @param pll_config  with the AD9528 user defined parameters
  */
void hal_pll_spi_config(SPI_HandleTypeDef *hspi, PLL_ConfigTypeDef *pll_config);

/**
  * @brief 			      configures the AD9528 clock distribution map
  * @param clock_channel  pointer to the clock channel array with fixed size that will be configured
  * @param clock_config   pointer to the clock config type what has the clock configuration
  */
void hal_pll_spi_config_clock_distribution_map(pll_bitfield_t *clock_channel, Ouput_ConfigTypeDef *clock_config);

/**
  * @brief 			  writes a value to a specific pll address (register) 
  * @param  hspi      pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  * @param address    register address that will be written 
  * @param value	  8-bit value that will be writt to the register given by address
  */
void hal_write_pll_register(SPI_HandleTypeDef *hspi, uint16_t address, uint8_t value);

/**
  * @brief             	sends command to trigger the VCO calibration
  * @param  hspi      	pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  */
void hal_write_pll_calibrate_vco(SPI_HandleTypeDef *hspi);

/**
  * @brief 				configures the AD9528 by sending the user defined ppl_config values through SPI
  * @param  hspi      	pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  */
void hal_write_pll_io_update(SPI_HandleTypeDef *hspi);

#endif /* INC_AD9528_STM32_HAL_H_ */
