/*
 * host_master.h
 *
 *  Created on: Apr 4, 2023
 *      Author: hoanpx
 */

#ifndef SERVICE_HOST_MASTER_HOST_MASTER_H_
#define SERVICE_HOST_MASTER_HOST_MASTER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include "sm_cmd.h"

#define SM_HOST_ADDR_DEFAULT    0x01
#define SM_HOST_MAX_ADDR        0x7F
#define SM_HOST_MAX_BUFFER      1024*4

typedef struct sm_host sm_host_t;

typedef int32_t (*sm_host_cmd_callback_fn_t)(int32_t _cmd, const uint8_t* _data, int32_t _len, void* );

typedef int32_t (*sm_host_send_if)(const uint8_t* _data, int32_t _len);

sm_host_t* sm_host_create(uint8_t _addr, sm_host_send_if _host_send_if);//_addr = 0 vs uart
sm_host_t* sm_host_create_default(uint8_t _addr, sm_host_send_if _host_send_if);

int32_t sm_host_destroy(sm_host_t* _host);
int32_t sm_host_set_addr(sm_host_t* _host, uint8_t _addr);
int32_t sm_host_reg_handle(sm_host_t* _host, sm_host_cmd_callback_fn_t _callback, void* _arg);//init
int32_t sm_host_send_cmd(sm_host_t* _host, int32_t _cmd, const uint8_t* _data, int32_t _len); // use for HMI send
int32_t sm_host_send_response(sm_host_t* _host, int32_t _cmd, int32_t _ret, const uint8_t* _data, int32_t _len);/// gui ban tin can nhan duoc cho hmi



void sm_host_process(sm_host_t* _host);

/**
 * @brief This function is put in the Interrupt or other thread.
 * @param _data
 * @param _len
 * @param _user_data
 * @return
 */
extern int32_t sm_host_asyn_feed(const uint8_t* _data, int32_t _len, void* _user_data);// received all data to hmi ...

#ifdef __cplusplus
};
#endif




#endif /* SERVICE_HOST_MASTER_HOST_MASTER_H_ */
