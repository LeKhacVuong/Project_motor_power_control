/*
 * core.h
 *
 *  Created on: Sep 16, 2020
 *      Author: quangnd
 */

#ifndef BOARD_CORE_HAL_H_
#define BOARD_CORE_HAL_H_
#include "compiler_optimize.h"
#include "core_hw.h"
//#include "timer_hw.h"
#define HAL_SYSTICK_FREQ_Hz                             SYSTICK_FREQ_Hz

void core_hw_init(void) WEAK;

#endif /* BOARD_CORE_HAL_H_ */
