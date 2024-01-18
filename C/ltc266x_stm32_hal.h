/*
 * ltc2664_stm32_hal.h
 *
 *  Created on: Mar 1, 2021
 *      Author: Eduardo
 */

#ifndef SRC_LTC2664_STM32_HAL_H_
#define SRC_LTC2664_STM32_HAL_H_

#include "stm32h7xx_hal.h"
#define STM32H735ZG
#include "illu_sysb.h"

/* Private defines -----------------------------------------------------------*/
#define LTC2664_TIMEOUT            5000
#define LTC2668_TIMEOUT            5000

#define SPI_LTC2664_CS_Pin         GPIO_PIN_14
#define SPI_LTC2664_CS_GPIO_Port   GPIOD
/* USER CODE BEGIN Private defines */
#define CS_ENABLE                  0
#define CS_DISABLE                 1

/*    COMMAND CODES 24BITS   */
#define DAC_WRITE_CODE_TO_N              (uint32_t)0x000000
#define DAC_WRITE_CODE_TO_ALL            (uint32_t)0x800000
#define DAC_WRITE_SPAN_TO_N              (uint32_t)0x600000
#define DAC_WRITE_SPAN_TO_ALL            (uint32_t)0xE00000
#define DAC_UPDATE_N                     (uint32_t)0x100000  // POWER UP
#define DAC_UPDATE_ALL                   (uint32_t)0x900000  // POWER UP
#define DAC_WRITE_CODE_TO_N_UPDATE_N     (uint32_t)0x300000  // POWER UP
#define DAC_WRITE_CODE_TO_N_UPDATE_ALL   (uint32_t)0x200000  // POWER UP
#define DAC_WRITE_CODE_TO_ALL_UPDATE_ALL (uint32_t)0xA00000  // POWER UP
#define DAC_POWER_DOWN_N                 (uint32_t)0x400000
#define DAC_POWER_DOWN_CHIP              (uint32_t)0x500000
#define DAC_MONITOR_MUX                  (uint32_t)0xB00000
#define DAC_TOGGLE_SELECT                (uint32_t)0xC00000
#define DAC_GLOBAL_TOGGLE                (uint32_t)0xD00000
#define DAC_CONFIG                       (uint32_t)0x700000
#define DAC_NO_OPERATION                 (uint32_t)0xF00000

#define DAC_DAC0_ADDRESS                 (uint32_t)0x000000
#define DAC_DAC1_ADDRESS                 (uint32_t)0x010000
#define DAC_DAC2_ADDRESS                 (uint32_t)0x020000
#define DAC_DAC3_ADDRESS                 (uint32_t)0x030000
#define DAC_DAC4_ADDRESS                 (uint32_t)0x040000
#define DAC_DAC5_ADDRESS                 (uint32_t)0x050000
#define DAC_DAC6_ADDRESS                 (uint32_t)0x060000
#define DAC_DAC7_ADDRESS                 (uint32_t)0x070000
#define DAC_DAC8_ADDRESS                 (uint32_t)0x080000
#define DAC_DAC9_ADDRESS                 (uint32_t)0x090000
#define DAC_DAC10_ADDRESS                (uint32_t)0x0A0000
#define DAC_DAC11_ADDRESS                (uint32_t)0x0B0000
#define DAC_DAC12_ADDRESS                (uint32_t)0x0C0000
#define DAC_DAC13_ADDRESS                (uint32_t)0x0D0000
#define DAC_DAC14_ADDRESS                (uint32_t)0x0E0000
#define DAC_DAC15_ADDRESS                (uint32_t)0x0F0000

#define DAC_ALL_DACS 					 (uint32_t)0xF000000 // NOT A VALID ADDRESS FOR THE LTC266X. Used in functions only.

#define DAC_SPAN_CODE_INT_0_TO_5V_EXT_0_TO_2VREF     (uint32_t)0X000000
#define DAC_SPAN_CODE_INT_0_TO_10V_EXT_0_TO_4VREF    (uint32_t)0X000001
#define DAC_SPAN_CODE_INT_BIP_5V_EXT_BIP_2VREF       (uint32_t)0X000002
#define DAC_SPAN_CODE_INT_BIP_10V_EXT_BIP_4VREF      (uint32_t)0X000003
#define DAC_SPAN_CODE_INT_BIP_2_5V_EXT_BIP_VREF      (uint32_t)0X000004

#define DAC_TOGGLE_DAC0                 (uint32_t)0X000001
#define DAC_TOGGLE_DAC1                 (uint32_t)0X000002
#define DAC_TOGGLE_DAC2                 (uint32_t)0X000004
#define DAC_TOGGLE_DAC3                 (uint32_t)0X000008
#define DAC_TOGGLE_ALL                  (uint32_t)0X00000F
#define DAC_TOGGLE_TGB                  (uint32_t)0X000001

#define DAC_WRITE_REGA                  (uint32_t)0X000000
#define DAC_WRITE_REGB                  (uint32_t)0X00000F

typedef union _DAC_ltc266x_cmd_t {
    uint32_t asInteger;
    uint8_t asBytes[4];
} DAC_ltc266x_cmd_t;

/**
  * @brief The update operation copies the contents of an input register to its associated DAC register
  * which controls the DAC output voltage or range (span). If the DAC associated toggle bit is '0', updates
  * value comes from register A, if it is '1' updates from register B.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  dac    : dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  */
void hal_ltc2664_spi_update_dac(SPI_HandleTypeDef *hspi1, uint32_t dac);

/**
  * @brief  Set the Toggle bits to select which register, whether A or B, will receive the code.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  select_code  : Bitcode that indicates which DACs toggle bit will be changed
  *                 dac 0 only : DAC_TOGGLE_DAC0
  *                 dac 1 only : DAC_TOGGLE_DAC1
  *                 dac 2 only : DAC_TOGGLE_DAC2
  *                 dac 3 only : DAC_TOGGLE_DAC3
  *                 all dacs   : DAC_TOGGLE_ALL
  * @param  reg     : register that will receive the code, whether A or B
  *                  Register A  :  DAC_WRITE_REGA
  *                  Register B  :  DAC_WRITE_REGB
  *
  */
void hal_ltc2664_spi_toggle_select(SPI_HandleTypeDef *hspi1, uint32_t select_code, uint32_t reg);

/**
  * @brief  Transmit an amount of data in blocking mode using ST HAL function.
  * @param  hspi    pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  transmit_data  : pointer to data struct
  * @param  Size    amount of data to be sent
  * @param  Timeout Timeout duration in ms
  */
void hal_ltc2664_spi_transmit(SPI_HandleTypeDef *hspi1, DAC_ltc266x_cmd_t *transmit_data, uint16_t size, uint32_t timeout);

/**
  * @brief Write code to register A or B according to @param reg of the corresponding DAC(s)
  * indicated at @param dac.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  code   : up to 12 bits of code to write to code register that will become DAC output.
  * @param  dac    : dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  * @param  reg     : register that will receive the code, whether A or B
  *                  Register A  :  DAC_WRITE_REGA
  *                  Register B  :  DAC_WRITE_REGB
  *
  */
void hal_ltc2664_spi_write_code(SPI_HandleTypeDef *hspi1, uint32_t code, uint32_t dac, uint32_t reg);

/**
  * @brief Write span value of the corresponding DAC(s) indicated at @param dac.
  * @param  hspi      : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  span_code :
  *                                           ------------------------------------------------
  *                                           | INTERNAL REFERENCE  |    EXTERNAL REFERENCE  |
  *                                           ------------------------------------------------
  * DAC_SPAN_CODE_INT_0_TO_5V_EXT_0_TO_2VREF  |    0V to 5V         |       0V to 2VREF      |
  * DAC_SPAN_CODE_INT_0_TO_10V_EXT_0_TO_4VREF |    0V to 10V        |       0V to 4VREF      |
  * DAC_SPAN_CODE_INT_SIM_5V_EXT_SIM_2VREF    |       ±5V           |         ±2VREF         |
  * DAC_SPAN_CODE_INT_SIM_10V_EXT_SIM_4VREF   |      ±10V           |         ±4VREF         |
  * DAC_SPAN_CODE_INT_SIM_2_5V_EXT_SIM_VREF   |     ±2.5V           |          ±VREF         |
  *                                           ------------------------------------------------
  *
  * @param  dac    : dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  *
  */
void hal_ltc2664_spi_set_span(SPI_HandleTypeDef *hspi1, uint32_t span_code, uint32_t dac);

/**
  * @brief Write code to register A or B according to @param reg of the corresponding DAC(s)
  * indicated at @param dac.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  code   : up to 12 bits of code to write to code register that will become DAC output.
  * @param  dac    : dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  * @param  reg     : register that will receive the code, whether A or B
  *                  Register A  :  DAC_WRITE_REGA
  *                  Register B  :  DAC_WRITE_REGB
  *
  */
void hal_ltc2668_spi_write_code(SPI_HandleTypeDef *hspi1, uint32_t code, uint32_t dac, uint32_t reg);

/**
  * @brief  Set the Toggle bits to select which register, whether A or B, will receive the code.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  select_code  : Bitcode that indicates which DACs toggle bit will be changed
  *                 dac 0 only : DAC_TOGGLE_DAC0
  *                 dac 1 only : DAC_TOGGLE_DAC1
  *                 dac 2 only : DAC_TOGGLE_DAC2
  *                 dac 3 only : DAC_TOGGLE_DAC3
  *                 all dacs   : DAC_TOGGLE_ALL
  * @param  reg     : register that will receive the code, whether A or B
  *                  Register A  :  DAC_WRITE_REGA
  *                  Register B  :  DAC_WRITE_REGB
  *
  */
void hal_ltc2668_spi_toggle_select(SPI_HandleTypeDef *hspi1, uint32_t select_code, uint32_t reg);

/**
  * @brief Write span value of the corresponding DAC(s) indicated at @param dac.
  * @param  hspi       pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  span_code 
  *                                           ------------------------------------------------
  *                                           | INTERNAL REFERENCE  |    EXTERNAL REFERENCE  |
  *                                           ------------------------------------------------
  * DAC_SPAN_CODE_INT_0_TO_5V_EXT_0_TO_2VREF  |    0V to 5V         |       0V to 2VREF      |
  * DAC_SPAN_CODE_INT_0_TO_10V_EXT_0_TO_4VREF |    0V to 10V        |       0V to 4VREF      |
  * DAC_SPAN_CODE_INT_SIM_5V_EXT_SIM_2VREF    |       ±5V           |         ±2VREF         |
  * DAC_SPAN_CODE_INT_SIM_10V_EXT_SIM_4VREF   |      ±10V           |         ±4VREF         |
  * DAC_SPAN_CODE_INT_SIM_2_5V_EXT_SIM_VREF   |     ±2.5V           |          ±VREF         |
  *                                           ------------------------------------------------
  *
  * @param  dac      dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  *
  */
void hal_ltc2668_spi_set_span(SPI_HandleTypeDef *hspi1, uint32_t span_code, uint32_t dac);

/**
  * @brief Write code to register A or B according to @param reg of the corresponding DAC(s)
  * indicated at @param dac.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  code   : up to 12 bits of code to write to code register that will become DAC output.
  * @param  dac    : dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  * @param  reg     : register that will receive the code, whether A or B
  *                  Register A  :  DAC_WRITE_REGA
  *                  Register B  :  DAC_WRITE_REGB
  *
  */
void hal_ltc2668_spi_write_code(SPI_HandleTypeDef *hspi1, uint32_t code, uint32_t dac, uint32_t reg);

/**
  * @brief The update operation copies the contents of an input register to its associated DAC register
  * which controls the DAC output voltage or range (span). If the DAC associated toggle bit is '0', updates
  * value comes from register A, if it is '1' updates from register B.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  dac    : dac(s) that will be updated.
  *                  dac 0 only  :  DAC_DAC0_ADDRESS
  *                  dac 1 only  :  DAC_DAC1_ADDRESS
  *                  dac 2 only  :  DAC_DAC2_ADDRESS
  *                  dac 3 only  :  DAC_DAC3_ADDRESS
  *                  all dacs    :  DAC_ALL_DACS
  */
 void hal_ltc2668_spi_update_dac(SPI_HandleTypeDef *hspi1, uint32_t dac);

/**
  * @brief  Transmit an amount of data in blocking mode using ST HAL function.
  * @param  hspi    pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  transmit_data  : pointer to data struct
  * @param  Size    amount of data to be sent
  * @param  Timeout Timeout duration in ms
  */
void hal_ltc2668_spi_transmit(SPI_HandleTypeDef *hspi1, DAC_ltc266x_cmd_t *transmit_data, uint16_t size, uint32_t timeout);

#endif /* SRC_LTC2664_STM32_HAL_H_ */
