/*
 * ltc2335_stm32_hal.h
 *
 *  Created on: Mar 18, 2021
 *      Author: Eduardo
 */

#ifndef INC_LTC2335_STM32_HAL_H_
#define INC_LTC2335_STM32_HAL_H_

#include "stm32h7xx_hal.h"
// Timeout
#define LTC2335_TIMEOUT            5000

// Pins and constanst
#define SPI_LTC2335_CS_Pin GPIO_PIN_14
#define SPI_LTC2335_CS_GPIO_Port GPIOD
#define ADC_LTC2335_NUM_OF_CHANNELS 16
#define CS_ENABLE 0
#define CS_DISABLE 1

                                                                       //-----------------------------------------------------------------------------------
                                                                       // BINARY SoftSpan| ANALOG INPUT              | FULL SCALE RANGE    |BINARY FORMAT OF
                                                                       // CODE SS[2:0]   |  RANGE                    |                     |CONVERSION RESULT
// To set the softspan                                                 // ---------------|---------------------------|---------------------|----------------
#define ADC_INP_RANGE_BIP_2_5XVREFBUF               (uint8_t)0X07U     //    111         |±2.5 • VREFBUF             |5 • VREFBUF          |Two’s Complement
#define ADC_INP_RANGE_BIP_2_5XVREFBUF_OVER_1_024    (uint8_t)0X06U     //    110         |±2.5 • VREFBUF/1.024       |5 • VREFBUF/1.024    |Two’s Complement
#define ADC_INP_RANGE_0V_to_2_5XVREFBUF             (uint8_t)0X05U     //    101         |0V to 2.5 • VREFBUF        |2.5 • VREFBUF        |Straight Binary
#define ADC_INP_RANGE_0V_to_2_5XVREFBUF_OVER_1_024  (uint8_t)0X04U     //    100         |0V to 2.5 • VREFBUF/1.024  |2.5 • VREFBUF/1.024  |Straight Binary
#define ADC_INP_RANGE_BIP_1_25XVREFBUF              (uint8_t)0X03U     //    011         |±1.25 • VREFBUF            |2.5 • VREFBUF        |Two’s Complement
#define ADC_INP_RANGE_BIP_1_25XVREFBUF_OVER_1_024   (uint8_t)0X02U     //    010         |±1.25 • VREFBUF/1.024      |2.5 • VREFBUF/1.024  |Two’s Complement
#define ADC_INP_RANGE_0V_to_1_25XVREFBUF            (uint8_t)0X01U     //    001         |0V to 1.25 • VREFBUF       |1.25 • VREFBUF       |Straight Binary
#define ADC_INP_RANGE_0V_to_1_25XVREFBUF_OVER_1_024 (uint8_t)0X00U     //    000         |0V to 1.25 • VREFBUF/1.024 |1.25 • VREFBUF/1.024 |Straight Binary

// To validate (make valid) the control command (any command with '0' in this bit will be ignored by the IC)
#define ADC_CONTROL_WORD_VALIDATION                 (uint8_t)0X80U
// To select which channel is having its Span set and/or conversion started
#define ADC_SET_CHANNEL0     (uint8_t)0X00U
#define ADC_SET_CHANNEL1     (uint8_t)0X08U
#define ADC_SET_CHANNEL2     (uint8_t)0X10U
#define ADC_SET_CHANNEL3     (uint8_t)0X18U
#define ADC_SET_CHANNEL4     (uint8_t)0X20U
#define ADC_SET_CHANNEL5     (uint8_t)0X28U
#define ADC_SET_CHANNEL6     (uint8_t)0X30U
#define ADC_SET_CHANNEL7     (uint8_t)0X38U

// Sequencer Command Type
typedef struct {
	uint8_t has_command;
	uint8_t channel;
	uint8_t softspam_code;
} ADC_LTC2335SequencerCommandType;

// Sequence Read Type
typedef struct {
	uint8_t has_value;
	uint8_t channel;
	uint8_t softspam_code;
	uint16_t value;
} ADC_LTC2335SequenceReadType;

// Union
typedef union {
    uint32_t asInteger;
    uint8_t asBytes[4];
} ADC_LTC2335Spi32BitCommandType;

/**
  * @brief Programs the LTC2335 Sequencing reading. 
  * @param  hspi1         : pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  * @param  sequence_data : sequence of configuration for the sequential reading.
  * @param  size   	      : maximum size of the sequencer.
  *
  */
void hal_ltc2335_spi_sequencer_program(SPI_HandleTypeDef *hspi1, ADC_LTC2335SequencerCommandType *sequence_data, uint16_t size);

/**
  * @brief Transmit data to the LTC2335 ADC. 
  * @param  hspi1         : pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  * @param  transmit_data : pointer to the sequence of data that will be transmitted through the SPI.
  * @param  size   	      : size of the data that will be transmitted.
  *
  */
 void hal_ltc2335_spi_transmit(SPI_HandleTypeDef *hspi1, uint8_t *transmit_data, uint16_t size, uint32_t timeout);

#endif /* INC_LTC2335_STM32_HAL_H_ */
