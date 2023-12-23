/*
 * host_master.c
 *
 *  Created on: Apr 4, 2023
 *      Author: hoanpx
 */

#include <stdlib.h>
#include <string.h>

#include "host_master.h"
#include "sm_cmd.h"
#include "crc.h"


#define SM_PROTO_START_BYTE     0xAA
#define SM_PROTO_STOP_BYTE      0x55

#define SM_PROTO_START_BYTE_INDEX       0
#define SM_PROTO_LENGTH_HIGH_INDEX      1
#define SM_PROTO_LENGTH_LOW_INDEX       2
#define SM_PROTO_DEVICE_ADDR_INDEX      3
#define SM_PROTO_CMD_INDEX              4
#define SM_PROTO_CMD_DATA_INDEX         5
#define SM_PROTO_RET_INDEX              5

#define WORD(msb,lsb)                  ((msb)*256 + (lsb))

uint32_t send_count = 0;


struct sm_host{
    uint8_t m_addr;

    uint8_t m_buffer[SM_HOST_MAX_BUFFER];
    int32_t m_buffer_rear_index;
    int32_t m_buffer_front_index;

    uint8_t m_packet[SM_HOST_MAX_BUFFER];
    int32_t m_process_index;

    sm_host_send_if m_host_send_if;
    sm_host_cmd_callback_fn_t m_cmd_callback;
    void* m_arg;
};

static sm_host_t g_host_default;

sm_host_t* sm_host_create(uint8_t _addr, sm_host_send_if _host_send_if){
    if(!_host_send_if){
        return NULL;
    }
    sm_host_t* host = (sm_host_t*)malloc(sizeof(sm_host_t));
    if(!host){
        return NULL;
    }
    if(_addr > SM_HOST_MAX_ADDR){
        host->m_addr = SM_HOST_ADDR_DEFAULT;
    } else {
        host->m_addr = _addr;
    }
    host->m_host_send_if = _host_send_if;

    memset(host->m_buffer, 0, SM_HOST_MAX_BUFFER);
    memset(host->m_packet, 0, SM_HOST_MAX_BUFFER);
    host->m_buffer_rear_index = 0;
    host->m_process_index = 0;
    host->m_buffer_front_index = 0;
    host->m_cmd_callback = NULL;
    host->m_arg = NULL;

    return host;
}

sm_host_t* sm_host_create_default(uint8_t _addr, sm_host_send_if _host_send_if){
    if(!_host_send_if){
        return NULL;
    }
    if(_addr > SM_HOST_MAX_ADDR){
        g_host_default.m_addr = SM_HOST_ADDR_DEFAULT;
    } else {
        g_host_default.m_addr = _addr;
    }
    g_host_default.m_host_send_if = _host_send_if;

    memset(g_host_default.m_buffer, 0, SM_HOST_MAX_BUFFER);
    memset(g_host_default.m_packet, 0, SM_HOST_MAX_BUFFER);
    g_host_default.m_buffer_rear_index = 0;
    g_host_default.m_process_index = 0;
    g_host_default.m_buffer_front_index = 0;
    g_host_default.m_cmd_callback = NULL;
    g_host_default.m_arg = NULL;

    return &g_host_default;
}

int32_t sm_host_destroy(sm_host_t* _host){  //ko dùng
    if(_host){
        _host->m_arg = NULL;
        _host->m_cmd_callback = NULL;
        memset(_host->m_buffer, 0, SM_HOST_MAX_BUFFER);    //clear 1024 bit của packet
        memset(_host->m_packet, 0, SM_HOST_MAX_BUFFER);
        _host->m_buffer_rear_index = 0;
        _host->m_process_index = 0;
        _host->m_buffer_front_index = 0;

        free(_host);
        return 0;
    }
    return -1;
}

int32_t sm_host_set_addr(sm_host_t* _host, uint8_t _addr){
    if(!_host){
        return -1;
    }
    _host->m_addr = _addr;
}

int32_t sm_host_reg_handle(sm_host_t* _host, sm_host_cmd_callback_fn_t _callback, void* _arg){
    if(!_host || !_callback){
        return -1;
    }
    _host->m_cmd_callback = _callback;
    _host->m_arg = _arg;

    return 0;
}



int32_t sm_host_send_cmd(sm_host_t* _host, int32_t _cmd, const uint8_t* _data, int32_t _len){
    uint8_t packet[SM_HOST_MAX_BUFFER];
    int32_t len = 0;
    int32_t data_len = _len+1+2+1; // 1-cmd, 2-crc,1-addr
    int32_t index = 0;
    uint16_t crc;

    if(!_host) {
        return -1;
    }
    packet[len++] = SM_PROTO_START_BYTE;
    packet[len++] = (data_len >> 8) & 0xFF;
    packet[len++] = (data_len & 0xff);
    packet[len++] = _host->m_addr;
    packet[len++] = _cmd;

    for(index = 0; index < _len; index++){
        packet[len++] = _data[index];
    }

    crc = CRC_CalculateCRC16(packet, len);
    packet[len++] = (crc >> 8) & 0xFF;
    packet[len++] = (crc & 0xFF);

    packet[len++] = SM_PROTO_STOP_BYTE;

    if(_host->m_host_send_if(packet, len) <= 0){
        return -1;
    }

    return len;
}




/**
  * @brief  Send CAN message received via UART
  * @param  host pointer to a sm_host_t structure
  * @param  command code
  * @param  return code
  * @param  data
  * @param  data length
  *
  * @retval uart data length
  *
  * note    call this function after receive a CAN message
  */
int32_t sm_host_send_response(sm_host_t *_host, int32_t _cmd, int32_t _ret, const uint8_t *_data, int32_t _len) {
	uint8_t packet[SM_HOST_MAX_BUFFER];
    int32_t len = 0;
    int32_t data_len = _len+1+2+1+1; // 1-cmd, 2-crc, 1-addr, 1-rets
    int32_t index = 0;
    uint16_t crc;

    if(!_host) {
        return -1;
    }
    packet[len++] = SM_PROTO_START_BYTE;            // write start byte to packet         (0xAA)
    packet[len++] = (data_len >> 8) & 0xFF;         // write 8 msb data length to packet
    packet[len++] = (data_len & 0xff);              // write 8 lsb data length to packet
    packet[len++] = _host->m_addr;                  // write master address to packet
    packet[len++] = _cmd;                           // write command code to packet
    packet[len++] = _ret;                           // write return code to packet

    for(index = 0; index < _len; index++){          // write CAN data to packet       (CAN ID, CAN data length, CAN data)
        packet[len++] = _data[index];
    }

    crc = CRC_CalculateCRC16(packet, len);          // calculate crc value for uart message
    packet[len++] = (crc >> 8) & 0xFF;              // write 8 msb crc to packet
    packet[len++] = (crc & 0xFF);                   // write 8 lsb crc to packet
    packet[len++] = SM_PROTO_STOP_BYTE;             // write start byte to packet          (0x55)

    if(_host->m_host_send_if(packet, len) <= 0){    // send packet via UART
        return -1;
    }
    return len;
}






void sm_host_process(sm_host_t* _host){
    sm_host_t* host = _host;
    int32_t ret = -1;
    int32_t cmd = 0;
    while (host->m_buffer_rear_index != host->m_buffer_front_index){
        host->m_packet[host->m_process_index] = host->m_buffer[host->m_buffer_front_index];

        if(host->m_packet[host->m_process_index] == SM_PROTO_STOP_BYTE  &&
                host->m_process_index == (WORD(host->m_packet[SM_PROTO_LENGTH_HIGH_INDEX], host->m_packet[SM_PROTO_LENGTH_LOW_INDEX]) + 3)){
            if(host->m_packet[SM_PROTO_DEVICE_ADDR_INDEX] != host->m_addr){
                return;
            }
            uint16_t crc = CRC_CalculateCRC16(&host->m_packet[SM_PROTO_START_BYTE_INDEX], host->m_process_index);
            if(!crc){

                if(host->m_cmd_callback){
                    cmd = host->m_packet[SM_PROTO_CMD_INDEX];
                    ret = host->m_cmd_callback(cmd,
                    							&host->m_packet[SM_PROTO_CMD_DATA_INDEX],
												host->m_process_index - 7,
												host->m_arg);
                }

            }else {
                return;
            }
            host->m_process_index = 0;

        }else if((host->m_packet[host->m_process_index] == SM_PROTO_START_BYTE && host->m_process_index == SM_PROTO_START_BYTE_INDEX) ||
                 (host->m_process_index > 0)){

            host->m_process_index++;
            if(host->m_process_index >= 32){
                host->m_process_index = 0;
            }
        }
        host->m_buffer_front_index++;
        if(host->m_buffer_front_index >= SM_HOST_MAX_BUFFER){
            host->m_buffer_front_index = 0;
        }
    }
}

int32_t sm_host_asyn_feed(const uint8_t* _data, int32_t _len, void* _user_data){   //copy data từ hmi vào m_buffer của host, đc gọi ở hàm ngắt nhận uart hmi
    sm_host_t* host = (sm_host_t*)_user_data;
    int32_t index = 0;
    if(!host){
        return -1;
    }
    for(index = 0; index < _len; index++){
        host->m_buffer[host->m_buffer_rear_index++] = _data[index];
        if(host->m_buffer_rear_index >= SM_HOST_MAX_BUFFER){
            host->m_buffer_rear_index = 0;
        }
    }
}



