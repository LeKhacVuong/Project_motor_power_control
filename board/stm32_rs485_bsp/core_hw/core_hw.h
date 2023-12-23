/*
 * core_hw.h
 *
 *  Created on: Sep 16, 2020
 *      Author: quangnd
 */

#ifndef BOARD_STM32_HALLIB_BSP_CORE_HW_H_
#define BOARD_STM32_HALLIB_BSP_CORE_HW_H_

#define SYSTICK_FREQ_Hz     (1000UL)

#include "stm32f1xx_hal.h"
#include "core.h"

//extern IWDG_HandleTypeDef hiwdg;

void core_hw_init(void);

#endif /* BOARD_STM32_HALLIB_BSP_CORE_HW_H_ */
