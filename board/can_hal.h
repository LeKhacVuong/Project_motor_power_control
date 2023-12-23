/*
 * can_hal.h
 *
 *  Created on: Aug 20, 2020
 *      Author: quangnd
 */

#ifndef BOARD_CAN_HAL_H_
#define BOARD_CAN_HAL_H_
#include "can_hardware.h"
#include "compiler_optimize.h"

#define HAL_CAN_ENABLE_IRQ			CAN_ENABLE_IRQ
#define HAL_CAN_DISABLE_IRQ			CAN_DISABLE_IRQ

void can_hardware_init(void) WEAK;
void can_send(CAN_Hw* p_hw, uint8_t* buff) WEAK;
void can_receive(CAN_Hw* p_hw, uint8_t* buff) WEAK;
void can_set_receive_handle(CAN_Hw* p_hw,void (*receive_handle)(CAN_Hw* p_hw)) WEAK;

#endif /* BOARD_CAN_HAL_H_ */
