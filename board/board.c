#include "board.h"

void board_init(void){
	HAL_Init();
	core_hw_init();
	uart_hw_init();
//	can_hardware_init();
}
