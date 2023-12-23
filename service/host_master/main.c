/*
 * main.c
 *
 *  Created on: Apr 4, 2023
 *      Author: PV
 */

#include "host_master.h"
#include "sm_cmd.h"
int32_t my_callback(int32_t _cmd, const uint8_t* _data, int32_t _len, void* ){

}
void main (void){
	sm_host_cmd_callback_fn_t my_callback_ptr = BP_response_callback;

}

