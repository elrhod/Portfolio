/*
 * ad5293_stm32_hal.h
 *
 *  Created on: Mar 11, 2021
 *      Author: Eduardo
 */

#ifndef INC_AD5293_STM32_HAL_H_
#define INC_AD5293_STM32_HAL_H_

/* Includes */
#include "stm32h7xx_hal.h"
#define STM32H735ZG
#include "illu_sysb.h"

/* Macro Defines */
#define AD5293_TIMEOUT              5000
#define CS_ENABLE             		0
#define CS_DISABLE             		1
#define AD5293_CS_Pin              GPIO_PIN_14
#define AD5293_CS_GPIO_Port        GPIOD

#define AD529X_NOP                 (uint16_t)0X0000
#define AD529X_WRITE_TO_RDAC       (uint16_t)0X0400
#define AD529X_READ_RDAC           (uint16_t)0X0800
#define AD529X_READ_RDAC           (uint16_t)0X0800
#define AD5293_RESET_RDAC          (uint16_t)0X1000
#define AD5293_WRITE_CONTROL       (uint16_t)0X1800
#define AD5293_READ_CONTROL        (uint16_t)0X1C00
#define AD5293_POWER_DOWN          (uint16_t)0X2000
// Mode selection
#define AD5293_RESISTOR_PERFORMANCE_MODE     (uint16_t)0X0000
#define AD5293_RESISTOR_NORMAL_MODE          (uint16_t)0X0004
#define AD5293_RESISTOR_LOCKED_MODE          (uint16_t)0X0000
#define AD5293_RESISTOR_UPDATE_MODE          (uint16_t)0X0002

#define AD5292_RESISTOR_UPDATE_ENABLE_MEM_MODE  (uint16_t)0X0003
#define AD5292_STORE_RDAC_TO_20TP               (uint16_t)0X0C00
#define AD5292_REFRESH_RDAC_WITH_20TP           (uint16_t)0X1000
#define AD5292_READ_20TP_MEMORY                 (uint16_t)0X1400
#define AD5292_WRITE_DATA_TO_CONTROL_REG        (uint16_t)0X1800
#define AD5292_READ_CONTROL_REG                 (uint16_t)0X1C00
#define AD5292_SOFTWARE_SHUTDOWN                (uint16_t)0X1000

/* Typedefs */
typedef union _DIG_POT_ad5293_cmd_t {
    uint16_t asInteger;
    uint8_t asBytes[2];
} DIG_POT_ad5293_cmd_t;

/* Function Prototypes */
/**
  * @brief Transmit the Digital Potentiometer configuration data at @param transmit_data to the Digital Potentiometer specified by @param dig_pot_num. 
  * @param  hspi1           : pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  * @param  transmit_data   : pointer to the DIG_POT_ad5293_cmd_t type containing the data that will be transmitted.
  * @param  size            : size in bytes to transmit
  * @param  timeout         : timeout value used in a pooling attempt to transmit
  * @param  dig_pot_num     : digital potentiometer number that is receiving the transmission
  *
  */
void hal_ad5293_spi_transmit(SPI_HandleTypeDef *hspi1, DIG_POT_ad5293_cmd_t *transmit_data, uint16_t size, uint32_t timeout, uint8_t dig_pot_num);

/**
  * @brief Updates the value of the Digital Potentiometer specified by @param dig_pot_num. 
  * @param  hspi1         : pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  * @param  value         : 10 bit value that is used to update the potentiometer.
  * @param  dig_pot_num   : digital potentiometer number that is receiving the transmission
  *
  */
void hal_ad5293_spi_update_dig_pot(SPI_HandleTypeDef *hspi1, uint16_t value, uint8_t dig_pot_num);

/**
  * @brief Initializes the SPI6 to be used as the SPI for the Digital Potentiometer control. 
  * @param  hspi         : pointer to a SPI_HandleTypeDef structure that contains
  *                           the configuration information for SPI module.
  *
  * TODO: Change this function to allow the definition of which SPI is being configured.
  *       It will allow this component library to be used in other projects where that use a different SPI.
  */
void hal_ad5293_SPI6_Init(SPI_HandleTypeDef *hspi);

#endif /* INC_AD5293_STM32_HAL_H_ */
